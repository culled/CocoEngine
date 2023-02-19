#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Abstract interface for all rendering-related operations
	/// </summary>
	struct COCOAPI IRenderPass
	{
		/// <summary>
		/// Gets the attachment descriptions required for this pass
		/// </summary>
		/// <returns>The attachment descriptions</returns>
		virtual List<struct AttachmentDescription> GetAttachmentDescriptions() = 0;
	};
}