#include "Renderpch.h"
#include "AttachmentCache.h"

#include "../RenderService.h"

namespace Coco::Rendering
{
	PipelineImageCache::PipelineImageCache() :
		PipelineVersion(0),
		Images{}
	{}

	AttachmentCache::AttachmentCache() : 
		_pipelineImageCaches{}
	{}

	std::vector<Ref<Image>> AttachmentCache::GetImages(const CompiledRenderPipeline& pipeline, const SizeInt& backbufferSize, std::span<Ref<Image>> backbuffers)
	{
		const std::vector<AttachmentFormat>& attachmentFormats = pipeline.InputAttachments;

		uint64 pipelineID = pipeline.PipelineID;

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

		std::vector<Ref<Image>> images(pipeline.InputAttachments.size());
		std::vector<bool> backbuffersMatched(backbuffers.size());

		for (uint8 i = 0; i < images.size(); i++)
		{
			const AttachmentFormat& attachmentFormat = attachmentFormats.at(i);
			bool foundMatch = false;

			// Look for an unmatched backbuffer that matches the attachment format
			for (size_t b = 0; b < backbuffers.size(); b++)
			{
				if (backbuffersMatched.at(b))
					continue;

				if (attachmentFormat.IsCompatible(backbuffers[b]->GetDescription()))
				{
					images.at(i) = backbuffers[b];
					backbuffersMatched[b] = true;
					foundMatch = true;
					break;
				}
			}

			if (foundMatch)
				continue;

			// Look for a valid cached image
			auto imageIt = cache.Images.find(i);
			if (imageIt != cache.Images.end())
			{
				Ref<Image>& cachedImage = imageIt->second;

				if (cachedImage.IsValid())
				{
					ImageDescription cachedDescription = cachedImage->GetDescription();
					if (attachmentFormat.IsCompatible(cachedDescription) &&
						cachedDescription.Width == backbufferSize.Width &&
						cachedDescription.Height == backbufferSize.Height)
					{
						images.at(i) = cachedImage;
						foundMatch = true;
						continue;
					}
				}
				
				// The cached image can't work with the attachment anymore, so release it
				cache.Images.erase(imageIt);
			}

			if (foundMatch)
				continue;

			// We need to create an image
			ImageDescription attachmentImageDescription(
				backbufferSize.Width, backbufferSize.Height,
				1,
				attachmentFormat.PixelFormat,
				attachmentFormat.ColorSpace,
				ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled | ImageUsageFlags::TransferDestination | ImageUsageFlags::TransferDestination
			);

			Ref<Image> createdImage = rendering->GetDevice().CreateImage(attachmentImageDescription);
			images.at(i) = createdImage;
			cache.Images.try_emplace(i, createdImage);
		}

		return images;
	}
}