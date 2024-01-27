#include "Renderpch.h"
#include "AttachmentCache.h"

#include "Pipeline/CompiledRenderPipeline.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Image.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	CachedAttachments::CachedAttachments(GraphicsDevice& device) :
		Device(device),
		PipelineVersion(0),
		Images(),
		LastUseTime(0.0)
	{}

	CachedAttachments::~CachedAttachments()
	{
		PurgeAllImages();
	}

	uint64 CachedAttachments::MakeKey(const CompiledRenderPipeline& pipeline, uint64 rendererID, const SizeInt& size)
	{
		return Math::CombineHashes(rendererID, pipeline.PipelineID, size.Width, size.Height);
	}

	void CachedAttachments::Use()
	{
		LastUseTime = MainLoop::Get()->GetCurrentTick().UnscaledTime;
	}

	bool CachedAttachments::ShouldPurge(double staleThreshold) const
	{
		return MainLoop::Get()->GetCurrentTick().UnscaledTime - LastUseTime > staleThreshold;
	}

	void CachedAttachments::TryPurgeImage(uint32 imageIndex)
	{
		auto it = Images.find(imageIndex);

		if (it == Images.end())
			return;

		Ref<Image> image = it->second;
		Images.erase(it);
		Device.TryReleaseResource(image->ID);
	}

	void CachedAttachments::PurgeAllImages()
	{
		auto it = Images.begin();
		while (it != Images.end())
		{
			Ref<Image> image = it->second;
			it = Images.erase(it);
			Device.TryReleaseResource(image->ID);
		}
	}

	const double AttachmentCache::ResourcePurgePeriod = 1.0;
	const double AttachmentCache::StaleResourceThreshold = 1.5;
	const int AttachmentCache::ResourcePurgeTickPriority = -20000;

	AttachmentCache::AttachmentCache() :
		_purgeTickListener(CreateManagedRef<TickListener>(this, &AttachmentCache::HandlePurgeTick, ResourcePurgeTickPriority)),
		_cache()
	{
		_purgeTickListener->SetTickPeriod(ResourcePurgePeriod, false);
		MainLoop::Get()->AddTickListener(_purgeTickListener);
	}

	AttachmentCache::~AttachmentCache()
	{
		_cache.clear();
		MainLoop::Get()->RemoveTickListener(_purgeTickListener);
	}

	std::vector<RenderTarget> AttachmentCache::GetRenderTargets(
        const CompiledRenderPipeline& pipeline, 
        uint64 rendererID, 
        const SizeInt& backbufferSize, 
        MSAASamples msaaSamples, 
        std::span<Ref<Image>> backbuffers)
    {
        uint64 key = CachedAttachments::MakeKey(pipeline, rendererID, backbufferSize);

		RenderService* rendering = RenderService::Get();
		CocoAssert(rendering, "RenderService singleton was null")

        CachedAttachments& attachmentCache = _cache.try_emplace(key, rendering->GetDevice()).first->second;
		attachmentCache.Use();

        if (pipeline.PipelineVersion != attachmentCache.PipelineVersion)
        {
            attachmentCache.Images.clear();
            attachmentCache.PipelineVersion = pipeline.PipelineVersion;
        }

        std::vector<RenderTarget> rts(pipeline.Attachments.size());
        std::vector<bool> backbuffersMatched(backbuffers.size());

        msaaSamples = static_cast<MSAASamples>(Math::Min(msaaSamples, rendering->GetDevice().GetFeatures().MaximumMSAASamples));

		for (uint32 i = 0; i < rts.size(); i++)
		{
			const RenderPassAttachment& attachmentFormat = pipeline.Attachments.at(i);
			bool foundMainMatch = false;

			// Look for an unmatched backbuffer that matches the attachment format
			for (uint64 b = 0; b < backbuffers.size(); b++)
			{
				if (backbuffersMatched.at(b))
					continue;

				Ref<Image>& backbufferImage = backbuffers[b];
				const ImageDescription& backbufferDesc = backbufferImage->GetDescription();

				if (attachmentFormat.IsCompatible(backbufferDesc))
				{
					if (msaaSamples == backbufferDesc.SampleCount)
					{
						rts.at(i).MainImage = backbuffers[b];
						foundMainMatch = true;
					}
					else
					{
						rts.at(i).ResolveImage = backbuffers[b];
					}

					backbuffersMatched[b] = true;
					break;
				}
			}

			if (!foundMainMatch)
			{
				rts.at(i).MainImage = GetOrCreateImage(rendering->GetDevice(), attachmentCache, backbufferSize, msaaSamples, pipeline, i);
			}
		}

		return rts;
    }

	uint64 AttachmentCache::PurgeUnusedAttachments()
	{
		RenderService* rendering = RenderService::Get();
		CocoAssert(rendering, "RenderService singleton was null")

		uint64 purged = std::erase_if(_cache,
			[](const auto& kvp)
			{
				return kvp.second.ShouldPurge(StaleResourceThreshold);
			});

		if (purged > 0)
		{
			CocoTrace("Purged {} CachedAttachments", purged)
		}

		return purged;
	}

	void AttachmentCache::HandlePurgeTick(const TickInfo& tickInfo)
	{
		PurgeUnusedAttachments();
	}

	Ref<Image> AttachmentCache::GetOrCreateImage(
		GraphicsDevice& device,
		CachedAttachments& cache, 
		const SizeInt& size, 
		MSAASamples msaaSamples, 
		const CompiledRenderPipeline& pipeline, 
		uint32 attachmentIndex)
	{
		const RenderPassAttachment& attachment = pipeline.Attachments.at(attachmentIndex);
		std::unordered_map<uint32, Ref<Image>>& map = cache.Images;
		
		auto imageIt = map.find(attachmentIndex);
		if (imageIt != map.end())
		{
			Ref<Image> cachedImage = imageIt->second;

			if (cachedImage.IsValid())
			{
				const ImageDescription& cachedDescription = cachedImage->GetDescription();

				if (attachment.IsCompatible(cachedDescription) &&
					cachedDescription.Width == size.Width &&
					cachedDescription.Height == size.Height &&
					cachedDescription.SampleCount == msaaSamples)
				{
					return cachedImage;
				}
			}

			// The cached image can't work with the attachment anymore, so release it
			map.erase(imageIt);
			device.TryReleaseResource(cachedImage->ID);
		}

		// We need to create an image
		ImageDescription attachmentImageDescription = ImageDescription::Create2D(
			size.Width, size.Height,
			attachment.PixelFormat,
			attachment.ColorSpace,
			ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled | ImageUsageFlags::TransferDestination | ImageUsageFlags::TransferSource,
			false,
			msaaSamples
		);

		Ref<Image> createdImage = device.CreateImage(attachmentImageDescription);
		map.try_emplace(attachmentIndex, createdImage);

		return createdImage;
	}
}