#pragma once

#include <Coco/Core/Core.h>
#include "RenderPass.h"

namespace Coco::Rendering
{
	/// @brief A binding between a RenderPipeline and a RenderPass
	struct RenderPassBinding
	{
		/// @brief The RenderPass
		SharedRef<RenderPass> Pass;
		
		/// @brief Mapped indices between the pipeline attachments and the pass attachments
		std::unordered_map<uint8, uint8> PipelineToPassIndexMapping;

		RenderPassBinding();
		RenderPassBinding(SharedRef<RenderPass> renderPass, const std::unordered_map<uint8, uint8>& indexMapping);
	};
}