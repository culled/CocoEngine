#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Matrix.h>
#include "../MeshTypes.h"

namespace Coco::Rendering
{
	class Mesh;
	class Material;
	class Image;

	/// @brief Sorting modes for render objects
	enum class RenderObjectSortMode
	{
		FrontToBack,
		BackToFront
	};

	struct RenderTarget
	{
		/// @brief The actual image that will be rendered to
		Ref<Image> MainImage;

		/// @brief If using MSAA, this is the image that will be resolved to from the main image
		Ref<Image> ResolveImage;

		/// @brief Clear values for clearing the image (if it needs to be cleared)
		Vector4 ClearValue;

		RenderTarget();
		RenderTarget(Ref<Image> mainImage, const Vector4& clearValue);

		~RenderTarget();

		/// @brief Sets clear values for all render targets using the clear color if it is a color target and the depth/stencil value otherwise
		/// @param renderTargets The render targets
		/// @param clearColor The color for clearing color targets
		/// @param clearDepth The depth value for clearing depth targets
		/// @param clearStencil The stencil value for clearing stencil targets
		static void SetClearValues(std::span<RenderTarget> renderTargets, const Color& clearColor, double clearDepth, uint8 clearStencil);

		/// @brief Sets the clear value from a color
		/// @param clearColor The clear color
		void SetColorClearValue(const Color& clearColor);

		/// @brief Sets the clear value for a depth image
		/// @param depthClearValue The depth to clear to
		void SetDepthClearValue(double depthClearValue);

		/// @brief Sets the clear values for a depth/stencil image
		/// @param depthClearValue The depth to clear to
		/// @param stencilClearValue The value to clear the stencil to
		void SetDepthStencilClearValue(double depthClearValue, uint8 stencilClearValue);
	};

	struct RenderObjectData
	{
		uint64 ID;

		uint64 VisibilityGroups;

		SharedRef<Rendering::Mesh> Mesh;
		Rendering::Submesh Submesh;

		SharedRef<Rendering::Material> Material;

		Matrix4x4 ModelMatrix;
		BoundingBox Bounds;

		std::any ExtraData;

		RenderObjectData(
			uint64 id,
			uint64 visibilityGroups,
			SharedRef<Rendering::Mesh> mesh,
			const Rendering::Submesh& submesh,
			SharedRef<Rendering::Material> material,
			const Matrix4x4& modelMatrix,
			const BoundingBox& bounds,
			std::any extraData);
	};
}