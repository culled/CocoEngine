#pragma once

#include "../Renderpch.h"
#include "RenderPassBinding.h"
#include "../Graphics/AttachmentFormat.h"
#include "RenderPipelineTypes.h"

namespace Coco::Rendering
{
	/// @brief An attachment for a CompiledRenderPipeline
	struct CompiledPipelineAttachment
	{
		static const CompiledPipelineAttachment Empty;

		/// @brief The pixel format for this attachment
		ImagePixelFormat PixelFormat;

		/// @brief The color space for this attachment
		ImageColorSpace ColorSpace;

		/// @brief If true, this attachment's data will be cleared once the render starts
		bool Clear;

		/// @brief If true, this attachment's data will be preserved for subsequent renders
		bool PreserveAfterRender;

		CompiledPipelineAttachment();
		CompiledPipelineAttachment(const AttachmentFormat& attachment, AttachmentClearMode clearMode);

		bool operator==(const CompiledPipelineAttachment& other) const;

		/// @brief Determines if the given AttachmentFormat is compatible with this attachment
		/// @param format The attachment format
		/// @return True if an image made for the given attachment can be used with this attachment
		bool IsCompatible(const AttachmentFormat& format) const;

		/// @brief Determines if an image made with the given description can be used with this attachment
		/// @param description The image description
		/// @return True if an image with the given description can be used with this attachment
		bool IsCompatible(const ImageDescription& description) const;
	};

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
		std::vector<CompiledPipelineAttachment> Attachments;

		CompiledRenderPipeline(uint64 pipelineID);
	};
}