#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Version.h>

namespace Coco::Rendering
{
	class GraphicsDevice;

	/// @brief A platform that can perform graphics operations 
	class GraphicsPlatform
	{
		friend class RenderService;
	public:
		virtual ~GraphicsPlatform() = default;

		/// @brief Gets the name
		/// @return The name
		virtual const string& GetName() const = 0;

		/// @brief Gets the API version
		/// @return The API version
		virtual Version GetAPIVersion() const = 0;

		/// @brief Creates an orthographic projection matrix
		/// @param left The value that corresponds to the left side of the screen
		/// @param right The value that corresponds to the right side of the screen
		/// @param bottom The value that corresponds to the bottom of the screen
		/// @param top The value that corresponds to the top of the screen
		/// @param nearClip The near clip distance
		/// @param farClip The far clip distance
		/// @return An orthographic projection matrix
		virtual Matrix4x4 CreateOrthographicProjection(
			double left, double right,
			double bottom, double top,
			double nearClip, double farClip) = 0;

		/// @brief Creates an orthographic projection matrix
		/// @param size The vertical height of the matrix
		/// @param aspectRatio The screen aspect ratio (width / height)
		/// @param nearClip The near clip distance
		/// @param farClip The far clip distance
		/// @return An orthographic projection matrix
		virtual Matrix4x4 CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip) = 0;

		/// @brief Creates a perspective projection matrix
		/// @param verticalFOVRadians The vertical field of view, in radians
		/// @param aspectRatio The screen aspect ratio (width / height)
		/// @param nearClip The near clip distance
		/// @param farClip The far clip distance
		/// @return A perspective projection matrix
		virtual Matrix4x4 CreatePerspectiveProjection(
			double verticalFOVRadians,
			double aspectRatio,
			double nearClip, double farClip) = 0;

	protected:
		/// @brief Creates the GraphicsDevice
		/// @return The GraphicsDevice
		virtual UniqueRef<GraphicsDevice> CreateDevice() const = 0;
	};
}