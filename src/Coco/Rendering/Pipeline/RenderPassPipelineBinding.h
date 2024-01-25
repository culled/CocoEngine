#pragma once
#include <Coco/Core/Types/Refs.h>
#include "RenderPass.h"

namespace Coco::Rendering
{
	/// @brief A binding between a RenderPipeline and an individual RenderPass
	struct RenderPassPipelineBinding
	{
		/// @brief The RenderPass
		SharedRef<RenderPass> Pass;

		/// @brief Mapped indices between the pipeline attachment indices and the pass attachment indices
		std::unordered_map<uint32, uint32> PipelineToPassAttachmentMapping;

		RenderPassPipelineBinding(SharedRef<RenderPass> renderPass, const std::unordered_map<uint32, uint32>& attachmentMapping);
	};

}