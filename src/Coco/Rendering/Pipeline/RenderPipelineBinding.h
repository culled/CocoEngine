#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>

#include "IRenderPass.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A mapped attachment description
	/// </summary>
	struct MappedAttachmentDescription
	{
		Optional<AttachmentDescription> AttachmentDescription;
		int PipelineAttachmentIndex;
	};

	/// <summary>
	/// A binding between an IRenderPass and a RenderPipeline
	/// </summary>
	class COCOAPI RenderPipelineBinding
	{
	private:
		Ref<IRenderPass> _renderPass;
		List<MappedAttachmentDescription> _mappedAttachmentDescriptions;

	public:
		RenderPipelineBinding(Ref<IRenderPass> renderPass, const List<int>& passToPipelineAttachmentMapping);
		~RenderPipelineBinding();

		/// <summary>
		/// Gets the mapping between the pipeline attachments and this render pass's attachments
		/// </summary>
		/// <returns>The list of attachment mappings</returns>
		List<MappedAttachmentDescription> GetMappedAttachmentDescriptions() const { return _mappedAttachmentDescriptions; }
	};
}