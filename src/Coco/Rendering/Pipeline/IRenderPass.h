#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/Resources/RenderContext.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include <Coco/Rendering/RenderingExceptions.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Abstract interface for all rendering-related operations
	/// </summary>
	class COCOAPI IRenderPass
	{
	public:
		virtual ~IRenderPass() = default;

		/// <summary>
		/// Gets the name of the subpass used in shaders for rendering with this pass
		/// </summary>
		/// <returns>The pass name</returns>
		virtual string GetName() const noexcept = 0;

		/// <summary>
		/// Gets the attachment descriptions required for this pass
		/// </summary>
		/// <returns>The attachment descriptions</returns>
		virtual List<AttachmentDescription> GetAttachmentDescriptions() noexcept = 0;

		/// <summary>
		/// Called to perform rendering operations with a RenderContext
		/// </summary>
		/// <param name="renderContext">The context to use for rendering</param>
		virtual void Execute(RenderContext* renderContext) = 0;
	};
}