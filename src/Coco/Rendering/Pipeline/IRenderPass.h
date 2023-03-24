#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/Resources/RenderContext.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include <Coco/Rendering/RenderingExceptions.h>

namespace Coco::Rendering
{
	/// @brief Abstract interface for all rendering-related operations
	class COCOAPI IRenderPass
	{
	public:
		virtual ~IRenderPass() = default;

		/// @brief Gets the name of the subpass used in shaders for rendering with this pass
		/// @return The pass name
		virtual string GetName() const noexcept = 0;

		/// @brief Gets the attachment descriptions required for this pass
		/// @return The attachment descriptions
		virtual List<AttachmentDescription> GetAttachmentDescriptions() noexcept = 0;

		/// @brief Called to perform rendering operations with a RenderContext
		/// @param renderContext The context to use for rendering
		virtual void Execute(RenderContext* renderContext) = 0;
	};
}