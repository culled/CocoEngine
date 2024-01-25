#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Refs.h>
#include "Graphics/RenderViewTypes.h"
#include "Graphics/GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class Image;
	struct CompiledRenderPipeline;

	/// @brief An image cache for a pipeline
	struct CachedAttachments
	{
		/// @brief The version of the pipeline that this cache was created for
		uint64 PipelineVersion;

		/// @brief The map of pipeline attachment indices to images
		std::unordered_map<uint32, Ref<Image>> Images;

		double LastUseTime;

		CachedAttachments();

		static uint64 MakeKey(const CompiledRenderPipeline& pipeline, uint64 rendererID, const SizeInt& size);

		void Use();
		bool ShouldPurge(double staleThreshold) const;
		void TryPurgeImage(GraphicsDevice& device, uint32 imageIndex);
		void PurgeAllImages(GraphicsDevice& device);
	};

	class AttachmentCache
	{
	public:
		std::vector<RenderTarget> GetRenderTargets(
			const CompiledRenderPipeline& pipeline,
			uint64 rendererID,
			const SizeInt& backbufferSize,
			MSAASamples msaaSamples,
			std::span<Ref<Image>> backbuffers);

		uint64 PurgeUnusedAttachments();

	private:
		std::unordered_map<uint64, CachedAttachments> _cache;

		Ref<Image> GetOrCreateImage(
			GraphicsDevice& device,
			CachedAttachments& cache,
			const SizeInt& size,
			MSAASamples msaaSamples,
			const CompiledRenderPipeline& pipeline,
			uint32 attachmentIndex);
	};
}