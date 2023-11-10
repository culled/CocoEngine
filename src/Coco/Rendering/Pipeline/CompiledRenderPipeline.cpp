#include "Renderpch.h"
#include "CompiledRenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const CompiledPipelineAttachment CompiledPipelineAttachment::Empty = CompiledPipelineAttachment();

	CompiledPipelineAttachment::CompiledPipelineAttachment() :
		PixelFormat(ImagePixelFormat::Unknown),
		ColorSpace(ImageColorSpace::Unknown),
		Clear(false),
		PreserveAfterRender(false)
	{}

	CompiledPipelineAttachment::CompiledPipelineAttachment(const AttachmentFormat& attachment, AttachmentClearMode clearMode) :
		PixelFormat(attachment.PixelFormat),
		ColorSpace(attachment.ColorSpace),
		Clear(clearMode == AttachmentClearMode::Clear),
		PreserveAfterRender(attachment.PreserveAfterRender)
	{}

	bool CompiledPipelineAttachment::operator==(const CompiledPipelineAttachment& other) const
	{
		return PixelFormat == other.PixelFormat &&
			ColorSpace == other.ColorSpace &&
			Clear == other.Clear &&
			PreserveAfterRender == other.PreserveAfterRender;
	}

	bool CompiledPipelineAttachment::IsCompatible(const AttachmentFormat& format) const
	{
		return AreCompatible(PixelFormat, format.PixelFormat) &&
			ColorSpace == format.ColorSpace;
	}

	bool CompiledPipelineAttachment::IsCompatible(const ImageDescription& description) const
	{
		return AreCompatible(PixelFormat, description.PixelFormat) &&
			ColorSpace == description.ColorSpace;
	}

	CompiledRenderPipeline::CompiledRenderPipeline(uint64 pipelineID) :
		PipelineID(pipelineID),
		Version(0),
		RenderPasses{},
		Attachments{}
	{}
}