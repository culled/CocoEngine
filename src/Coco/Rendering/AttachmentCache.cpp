#include "Renderpch.h"
#include "AttachmentCache.h"

#include "Pipeline/CompiledRenderPipeline.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Image.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	CachedAttachments::CachedAttachments() :
		PipelineVersion(0),
		Images(),
		LastUseTime(0.0)
	{}

	uint64 CachedAttachments::MakeKey(const CompiledRenderPipeline& pipeline, uint64 rendererID, const SizeInt& size)
	{
		return Math::CombineHashes(rendererID, pipeline.PipelineID, size.Width, size.Height);
	}

	void CachedAttachments::Use()
	{
		LastUseTime = MainLoop::Get()->GetCurrentTick().Time;
	}

	bool CachedAttachments::ShouldPurge(double staleThreshold) const
	{
		return MainLoop::Get()->GetCurrentTick().Time - LastUseTime > staleThreshold;
	}

	void CachedAttachments::TryPurgeImage(GraphicsDevice& device, uint32 imageIndex)
	{
		auto it = Images.find(imageIndex);

		if (it == Images.end())
			return;

		Ref<Image> image = it->second;
		Images.erase(it);
		device.TryReleaseResource(image->ID);
	}

	void CachedAttachments::PurgeAllImages(GraphicsDevice& device)
	{
		auto it = Images.begin();
		while (it != Images.end())
		{
			Ref<Image> image = it->second;
			it = Images.erase(it);
			device.TryReleaseResource(image->ID);
		}
	}

    std::vector<RenderTarget> AttachmentCache::GetRenderTargets(
        const CompiledRenderPipeline& pipeline, 
        uint64 rendererID, 
        const SizeInt& backbufferSize, 
        MSAASamples msaaSamples, 
        std::span<Ref<Image>> backbuffers)
    {
        uint64 key = CachedAttachments::MakeKey(pipeline, rendererID, backbufferSize);

        CachedAttachments& attachmentCache = _cache.try_emplace(key).first->second;
		attachmentCache.Use();

        if (pipeline.PipelineVersion != attachmentCache.PipelineVersion)
        {
            attachmentCache.Images.clear();
            attachmentCache.PipelineVersion = pipeline.PipelineVersion;
        }

        RenderService* rendering = RenderService::Get();
        CocoAssert(rendering, "RenderService singleton was null")

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

		GraphicsDevice& device = rendering->GetDevice();

		const double staleThreshold = 5.0;

		uint64 purged = 0;

		auto it = _cache.begin();
		while (it != _cache.end())
		{
			CachedAttachments& cache = it->second;

			if (cache.ShouldPurge(staleThreshold))
			{
				cache.PurgeAllImages(device);
				it = _cache.erase(it);
				purged++;
			}
			else
			{
				++it;
			}
		}

		return purged;
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