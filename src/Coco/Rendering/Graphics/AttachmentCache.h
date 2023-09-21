#pragma once

#include "../Pipeline/CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief An image cache for a pipeline
	struct PipelineImageCache
	{
		/// @brief The version of the pipeline that this cache was created for
		uint64 PipelineVersion;

		/// @brief The map of pipeline attachment indices to images
		std::unordered_map<uint8, Ref<Image>> Images;

		PipelineImageCache();
	};

	/// @brief Holds a cache of images used for rendering
	class AttachmentCache
	{
	private:
		std::unordered_map<uint64, PipelineImageCache> _pipelineImageCaches;

	public:
		AttachmentCache();

		/// @brief Gets/creates images required for the given pipeline
		/// @param pipeline The pipeline being used to render
		/// @param backbufferSize The size of the images
		/// @param backbuffers The provided backbuffers
		/// @return Images for the pipeline's attachments
		std::vector<Ref<Image>> GetImages(const CompiledRenderPipeline& pipeline, const SizeInt& backbufferSize, std::span<Ref<Image>> backbuffers);
	};
}