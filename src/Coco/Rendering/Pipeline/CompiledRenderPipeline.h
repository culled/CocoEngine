#pragma once

#include <Coco/Core/Resources/Resource.h>
#include "../Graphics/RenderPassAttachment.h"
#include "RenderPassPipelineBinding.h"

namespace Coco::Rendering
{
	/// @brief A compiled representation of a RenderPipeline
	struct CompiledRenderPipeline
	{
		/// @brief The id of the pipeline that compiled this
		ResourceID PipelineID;

		/// @brief The version of the pipeline
		ResourceVersion PipelineVersion;

		/// @brief RenderPasses in this pipeline
		std::vector<RenderPassPipelineBinding> RenderPasses;

		/// @brief The attachments in this pipeline
		std::vector<RenderPassAttachment> Attachments;

		CompiledRenderPipeline(const ResourceID& pipelineID, const ResourceVersion& pipelineVersion);

		/// @brief Gets the attachments for a RenderPass at the given index in this pipeline
		/// @param passIndex The index of the RenderPass in this pipeline
		/// @return The attachment
		std::span<const RenderPassAttachment> GetPassAttachments(uint32 passIndex) const;
	};
}