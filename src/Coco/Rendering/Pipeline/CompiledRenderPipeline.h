#pragma once

#include "../Renderpch.h"
#include "RenderPassBinding.h"

namespace Coco::Rendering
{
	/// @brief A compiled RenderPipeline
	struct CompiledRenderPipeline
	{
		/// @brief The id of the pipeline that compiled this
		uint64 PipelineID;

		/// @brief The version of the pipeline
		uint64 Version;

		/// @brief RenderPasses in this pipeline
		std::vector<RenderPassBinding> RenderPasses;

		/// @brief The attachments in this pipeline
		std::vector<AttachmentFormat> InputAttachments;

		/// @brief True if this pipeline supports MSAA
		bool SupportsMSAA;

		CompiledRenderPipeline(uint64 pipelineID);
	};
}