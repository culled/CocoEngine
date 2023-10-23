#pragma once

#include "../Renderpch.h"
#include "GraphicsPipelineTypes.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Size.h>

namespace Coco::Rendering
{
	struct CompiledRenderPipeline;
	struct RenderTarget;
	struct AttachmentFormat;
	class Image;

	/// @brief An image cache for a pipeline
	struct PipelineImageCache
	{
		/// @brief The version of the pipeline that this cache was created for
		uint64 PipelineVersion;

		/// @brief The map of pipeline attachment indices to images
		std::unordered_map<uint8, Ref<Image>> Images;

		PipelineImageCache();

		static uint64 MakeKey(const CompiledRenderPipeline& pipeline, uint64 rendererID);
	};

	/// @brief Holds a cache of images used for rendering
	class AttachmentCache
	{
	private:
		std::unordered_map<uint64, PipelineImageCache> _pipelineImageCaches;

	public:
		AttachmentCache();

		/// @brief Creates render targets required for the given pipeline
		/// @param pipeline The pipeline being used to render
		/// @param rendererID The ID of the renderer
		/// @param backbufferSize The size of the images
		/// @param msaaSamples The number of MSAA samples for the attachments
		/// @param backbuffers The provided backbuffers
		/// @return RenderTargets with the neccessary images for the pipeline's attachments
		std::vector<RenderTarget> CreateRenderTargets(
			const CompiledRenderPipeline& pipeline, 
			uint64 rendererID,
			const SizeInt& backbufferSize, 
			MSAASamples msaaSamples,
			std::span<Ref<Image>> backbuffers);

	private:
		/// @brief Get/creates an image for an attachment 
		/// @param imageCache The image cache to use
		/// @param size The size of the image
		/// @param msaaSamples The number of MSAA samples for the image
		/// @param attachmentFormat The attachment format
		/// @param attachmentIndex The index of the attachment
		/// @return An image
		Ref<Image> GetOrCreateImage(
			PipelineImageCache& imageCache, 
			const SizeInt& size,
			MSAASamples msaaSamples,
			const AttachmentFormat& attachmentFormat,
			uint8 attachmentIndex);
	};
}