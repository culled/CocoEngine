#pragma once

#include "../Renderpch.h"
#include "RenderPassBinding.h"

namespace Coco::Rendering
{
	/// @brief A compiled RenderPipeline
	struct CompiledRenderPipeline
	{
		/// @brief The hash of the pipeline that compiled this
		uint64 PipelineHash;

		/// @brief The version
		uint64 Version;

		/// @brief RenderPasses in this pipeline
		std::vector<RenderPassBinding> RenderPasses;

		/// @brief The attachments in this pipeline
		std::vector<AttachmentFormat> InputAttachments;

		CompiledRenderPipeline(uint64 pipelineHash);
	};
}