#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Version.h>
#include <Coco/Core/Types/Matrix.h>
#include "GraphicsDeviceTypes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;

	/// @brief A graphics platform
	class GraphicsPlatform
	{
	public:
		virtual ~GraphicsPlatform() = default;

		/// @brief Gets the name of this platform
		/// @return This platform's name
		virtual const char* GetName() const = 0;

		/// @brief Gets the version of this platform's api
		/// @return The api version
		virtual Version GetAPIVersion() const = 0;

		/// @brief Creates a GraphicsDevice
		/// @param createParams Parameters for creating a GraphicsDevice
		/// @return A GraphicsDevice
		virtual UniqueRef<GraphicsDevice> CreateDevice(const GraphicsDeviceCreateParams& createParams) = 0;

		/// @brief Creates an orthographic projection matrix
		/// @param left The left side of the view frustrum
		/// @param right The right side of the view frustrum
		/// @param bottom The bottom side of the view frustrum
		/// @param top The top side of the view frustrum
		/// @param nearClip The distance to the near clipping plane
		/// @param farClip The distance to the far clipping plane
		/// @return An orthographic projection matrix
		virtual Matrix4x4 CreateOrthographicProjection(
			double left, double right, 
			double bottom, double top,
			double nearClip, double farClip) = 0;

		/// @brief Creates an orthographic projection matrix
		/// @param size The vertical size of the view frustrum
		/// @param aspectRatio The aspect ratio
		/// @param nearClip The distance to the near clipping plane
		/// @param farClip The distance to the far clipping plane
		/// @return An orthographic projection matrix
		virtual Matrix4x4 CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip) = 0;

		/// @brief Creates a perspective matrix
		/// @param verticalFOVRadians The vertical field of view (in radians)
		/// @param aspectRatio The aspect ratio
		/// @param nearClip The distance to the near clipping plane
		/// @param farClip The distance to the far clipping plane
		/// @return A perspective projection matrix
		virtual Matrix4x4 CreatePerspectiveProjection(
			double verticalFOVRadians, 
			double aspectRatio, 
			double nearClip, double farClip) = 0;
	};
}