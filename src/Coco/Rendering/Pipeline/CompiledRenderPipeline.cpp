#include "Renderpch.h"
#include "CompiledRenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const CompiledPipelineAttachment CompiledPipelineAttachment::Empty = CompiledPipelineAttachment();

	CompiledPipelineAttachment::CompiledPipelineAttachment() :
		PixelFormat(ImagePixelFormat::Unknown),
		ColorSpace(ImageColorSpace::Unknown),
		Options()
	{}

	CompiledPipelineAttachment::CompiledPipelineAttachment(const AttachmentFormat& attachment, AttachmentOptionFlags options) :
		PixelFormat(attachment.PixelFormat),
		ColorSpace(attachment.ColorSpace),
		Options(options)
	{}

	bool CompiledPipelineAttachment::operator==(const CompiledPipelineAttachment& other) const
	{
		return PixelFormat == other.PixelFormat &&
			ColorSpace == other.ColorSpace &&
			Options == other.Options;
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