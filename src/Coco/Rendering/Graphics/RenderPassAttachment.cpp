#include "Renderpch.h"
#include "RenderPassAttachment.h"

namespace Coco::Rendering
{
	RenderPassAttachment::RenderPassAttachment() :
		RenderPassAttachment(ImagePixelFormat::Unknown, ImageColorSpace::Unknown, AttachmentUsageFlags::None)
	{}

	RenderPassAttachment::RenderPassAttachment(
		ImagePixelFormat pixelFormat, 
		ImageColorSpace colorSpace, 
		AttachmentUsageFlags usageFlags,
		AttachmentOptionsFlags optionsFlags) :
		PixelFormat(pixelFormat),
		ColorSpace(colorSpace),
		UsageFlags(usageFlags),
		OptionsFlags(optionsFlags)
	{}

	bool RenderPassAttachment::operator==(const RenderPassAttachment & other) const
	{
		return PixelFormat == other.PixelFormat &&
			ColorSpace == other.ColorSpace && 
			UsageFlags == other.UsageFlags;
	}

	bool RenderPassAttachment::IsCompatible(const RenderPassAttachment& other) const
	{
		return PixelFormat == other.PixelFormat &&
			ColorSpace == other.ColorSpace;
	}

	bool RenderPassAttachment::IsCompatible(const ImageDescription& imageDescription) const
	{
		return PixelFormat == imageDescription.PixelFormat &&
			ColorSpace == imageDescription.ColorSpace;
	}
}