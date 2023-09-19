#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Rendering/Graphics/AttachmentFormat.h>
#include "../Graphics/RenderContext.h"
#include "../Graphics/RenderView.h"

namespace Coco::Rendering
{
	/// @brief Defines a single rendering operation
	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		/// @brief Gets the name of this pass. Used to link to individual render pass shaders
		/// @return This pass's name
		virtual string GetName() const = 0;

		/// @brief Gets the attachment formats that this pass uses
		/// @return This pass's attachments
		virtual std::span<const AttachmentFormat> GetInputAttachments() const = 0;

		/// @brief Executes this render pass
		/// @param context The context to use for rendering
		/// @param renderView The view being rendered
		virtual void Execute(RenderContext& context, const RenderView& renderView) = 0;
	};
}
