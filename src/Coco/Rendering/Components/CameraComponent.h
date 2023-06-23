#pragma once

/*#include <Coco/Core/Scene/Components/EntityComponent.h>
#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include "CameraComponentTypes.h"

namespace Coco::Rendering
{
	class Image;
	class RenderPipeline;
	class RenderingService;

	/// @brief A cached set of images for a pipeline
	class ImageCache final : public CachedResource
	{
	public:
		WeakRef<RenderPipeline> PipelineRef;
		UnorderedMap<int, WeakManagedRef<Image>> Images;

		ImageCache(const Ref<RenderPipeline>& pipeline);
		~ImageCache() final = default;

		bool IsInvalid() const noexcept final { return PipelineRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Updates this cache of images
		/// @param images The images to cache
		void Update(const UnorderedMap<int, WeakManagedRef<Image>>& images);
	};

	/// @brief A camera component that can render a scene from a perspective
	class COCOAPI CameraComponent : public EntityComponent
	{
	private:
		ProjectionType _projectionType = ProjectionType::Custom;

		Matrix4x4 _projectionMatrix = Matrix4x4::Identity;
		Matrix4x4 _viewMatrix = Matrix4x4::Identity;

		double _nearClipDistance = 0.1;
		double _farClipDistance = 100.0;

		double _perspectiveFieldOfView = 90.0;
		double _orthographicSize = 10.0;
		double _aspectRatio = 1.0;

		bool _isProjectionMatrixDirty = true;

		List<WeakManagedRef<Image>> _renderTargetOverrides;
		UnorderedMap<ResourceID, ImageCache> _imageCache;

	public:
		CameraComponent(SceneEntity* entity);
		virtual ~CameraComponent() = default;

		/// @brief Gets the type of projection that this camera is using
		/// @return The type of projection that this camera is using
		ProjectionType GetProjectionType() const noexcept { return _projectionType; }

		/// @brief Sets this camera to use a perspective projection
		/// @param fieldOfView The vertical field of view (in degrees)
		/// @param aspectRatio The aspect ratio
		/// @param nearClipDistance The near clip distance
		/// @param farClipDistance The far clip distance
		void SetPerspectiveProjection(double fieldOfView, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept;

		/// @brief Sets this camera to use an orthographic projection
		/// @param size The size of the projection
		/// @param aspectRatio The aspect ratio
		/// @param nearClipDistance The near clip distance
		/// @param farClipDistance The far clip distance
		void SetOrthographicProjection(double size, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept;

		/// @brief Sets this camera to use a custom projection matrix
		/// @param projection The custom projection matrix
		void SetCustomProjection(const Matrix4x4& projection) noexcept;

		/// @brief Gets this camera's current projection matrix
		/// @return The current projection matrix
		Matrix4x4 GetProjectionMatrix() noexcept;

		/// @brief Sets this camera's current view matrix
		/// @param view The view matrix
		void SetViewMatrix(const Matrix4x4& view) noexcept { _viewMatrix = view; }

		/// @brief Gets this camera's current view matrix
		/// @return The current view matrix
		Matrix4x4 GetViewMatrix() const noexcept { return _viewMatrix; }

		/// @brief Sets this camera's aspect ratio
		/// @param aspectRatio The new aspect ratio
		void SetAspectRatio(double aspectRatio) noexcept;

		/// @brief Gets this camera's current aspect ratio
		/// @return This camera's current aspect ratio
		double GetAspectRatio() const noexcept { return _aspectRatio; }

		/// @brief Sets this camera's near clip plane distance
		/// @param nearClipDistance The new near clip plane distance
		void SetNearClipDistance(double nearClipDistance) noexcept;

		/// @brief Gets this camera's current near clip plane distance
		/// @return This camera's current near clip plane distance
		double GetNearClipDistance() const noexcept { return _nearClipDistance; }

		/// @brief Sets this camera's far clip plane distance
		/// @param farClipDistance The new far clip plane distance
		void SetFarClipDistance(double farClipDistance) noexcept;

		/// @brief Gets this camera's current far clip plane distance
		/// @return This camera's current far clip plane distance
		double GetFarClipDistance() const noexcept { return _farClipDistance; }

		/// @brief Sets the rendertarget overrides to use
		/// @param renderTargets The rendertargets to use
		void SetRenderTargetOverrides(const List<WeakManagedRef<Image>>& renderTargets) { _renderTargetOverrides = renderTargets; }

		/// @brief Gets rendertargets that match the given pipeline's attachment layout
		/// @param pipeline The pipeline
		/// @return A list of render targets
		List<WeakManagedRef<Image>> GetRenderTargets(const Ref<RenderPipeline>& pipeline, const SizeInt& size);

	private:
		/// @brief Updates the internal projection matrix based on the projection type
		void UpdateProjectionMatrix() noexcept;

		/// @brief Ensures there is an active rendering service and returns it
		/// @return The active rendering service
		RenderingService* EnsureRenderingService() const;
	};
}*/