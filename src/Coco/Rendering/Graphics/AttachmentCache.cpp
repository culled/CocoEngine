#include "Renderpch.h"
#include "AttachmentCache.h"

#include "../RenderService.h"

namespace Coco::Rendering
{
	PipelineImageCache::PipelineImageCache() :
		PipelineVersion(0),
		Images{}
	{}

	uint64 PipelineImageCache::MakeKey(const CompiledRenderPipeline& pipeline, uint64 rendererID)
	{
		return Math::CombineHashes(rendererID, pipeline.PipelineID);
	}

	AttachmentCache::AttachmentCache() : 
		_pipelineImageCaches{}
	{}

	std::vector<RenderTarget> AttachmentCache::CreateRenderTargets(
		const CompiledRenderPipeline& pipeline,
		uint64 rendererID,
		const SizeInt& backbufferSize,
		MSAASamples msaaSamples, 
		std::span<Ref<Image>> backbuffers)
	{
		const std::vector<AttachmentFormat>& attachmentFormats = pipeline.InputAttachments;

		uint64 pipelineID = PipelineImageCache::MakeKey(pipeline, rendererID);

		auto it = _pipelineImageCaches.find(pipelineID);

		if (it == _pipelineImageCaches.end())
		{
			it = _pipelineImageCaches.try_emplace(pipelineID).first;
		}

		PipelineImageCache& cache = it->second;

		if (pipeline.Version != cache.PipelineVersion)
		{
			cache.Images.clear();
			cache.PipelineVersion = pipeline.Version;
		}

		RenderService* rendering = RenderService::Get();
		if (!rendering)
			throw std::exception("No active RenderService found");

		std::vector<RenderTarget> rts(pipeline.InputAttachments.size());
		std::vector<bool> backbuffersMatched(backbuffers.size());

		if (pipeline.SupportsMSAA)
			msaaSamples = static_cast<MSAASamples>(Math::Min(msaaSamples, rendering->GetDevice().GetFeatures().MaximumMSAASamples));
		else
			msaaSamples = MSAASamples::One;

		for (uint8 i = 0; i < rts.size(); i++)
		{
			const AttachmentFormat& attachmentFormat = attachmentFormats.at(i);
			bool foundMainMatch = false;

			// Look for an unmatched backbuffer that matches the attachment format
			for (size_t b = 0; b < backbuffers.size(); b++)
			{
				if (backbuffersMatched.at(b))
					continue;

				if (attachmentFormat.IsCompatible(backbuffers[b]->GetDescription()))
				{
					if (msaaSamples == MSAASamples::One)
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
				rts.at(i).MainImage = GetOrCreateImage(cache, backbufferSize, msaaSamples, attachmentFormat, i);
			}
		}

		return rts;
	}

	Ref<Image> AttachmentCache::GetOrCreateImage(
		PipelineImageCache& imageCache, 
		const SizeInt& size, 
		MSAASamples msaaSamples,
		const AttachmentFormat& attachmentFormat, 
		uint8 attachmentIndex)
	{
		std::unordered_map<uint8, Ref<Image>>& map = imageCache.Images;

		auto imageIt = map.find(attachmentIndex);
		if (imageIt != map.end())
		{
			Ref<Image>& cachedImage = imageIt->second;

			if (cachedImage.IsValid())
			{
				ImageDescription cachedDescription = cachedImage->GetDescription();
				if (attachmentFormat.IsCompatible(cachedDescription) &&
					cachedDescription.Width == size.Width &&
					cachedDescription.Height == size.Height &&
					cachedDescription.SampleCount == msaaSamples)
				{
					return cachedImage;
				}
			}

			// The cached image can't work with the attachment anymore, so release it
			map.erase(imageIt);
		}

		// We need to create an image
		ImageDescription attachmentImageDescription(
			size.Width, size.Height,
			attachmentFormat.PixelFormat,
			attachmentFormat.ColorSpace,
			ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled | ImageUsageFlags::TransferDestination | ImageUsageFlags::TransferDestination,
			false,
			msaaSamples
		);

		Ref<Image> createdImage = RenderService::Get()->GetDevice().CreateImage(attachmentImageDescription);
		map.try_emplace(attachmentIndex, createdImage);

		return createdImage;
	}
}