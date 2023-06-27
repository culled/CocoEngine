#pragma once

#include "../EntityComponent.h"
#include <Coco/Core/Resources/ResourceCache.h>
#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Resources/ResourceLibrary.h>

#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/Providers/ICameraDataProvider.h>

namespace Coco::Rendering
{
	class Image;
	class RenderPipeline;
}

namespace Coco::ECS
{

	/// @brief A cached set of images for a pipeline
	class PipelineImageCache final : public Resource
	{
	private:
		ResourceVersion _pipelineVersion;

	public:
		Ref<Rendering::RenderPipeline> Pipeline;
		UnorderedMap<int, Ref<Rendering::Image>> Images;

		PipelineImageCache(ResourceID id, const string& name, uint64_t tickLifetime, const Ref<Rendering::RenderPipeline>& pipeline);
		~PipelineImageCache() final = default;

		DefineResourceType(PipelineImageCache)

		bool IsValid() const noexcept { return Pipeline.IsValid(); }
		bool NeedsUpdate() const noexcept;

		/// @brief Updates this cache of images
		/// @param images The images to cache
		void Update(const UnorderedMap<int, Ref<Rendering::Image>>& images);
	};

	/// @brief A camera component that can render a scene from a perspective
	class COCOAPI CameraComponent : public EntityComponent, public Rendering::ICameraDataProvider
	{
	public:
		/// @brief Types of camera projections
		enum class ProjectionType
		{
			Perspective,
			Orthographic,
			Custom
		};

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

		List<Ref<Rendering::Image>> _renderTargetOverrides;
		ManagedRef<ResourceCache<PipelineImageCache>> _imageCache;

	public:
		CameraComponent() = default;
		CameraComponent(EntityID owner);

		virtual ~CameraComponent() = default;

		CameraComponent(CameraComponent&& other) noexcept : EntityComponent(std::move(other))
		{
			_projectionType = other._projectionType;

			_projectionMatrix = std::move(other._projectionMatrix);
			_viewMatrix = std::move(other._viewMatrix);

			_nearClipDistance = other._nearClipDistance;
			_farClipDistance = other._farClipDistance;

			_perspectiveFieldOfView = other._perspectiveFieldOfView;
			_orthographicSize = other._orthographicSize;
			_aspectRatio = other._aspectRatio;

			_isProjectionMatrixDirty = other._isProjectionMatrixDirty;

			_renderTargetOverrides = std::move(other._renderTargetOverrides);
			_imageCache = std::move(other._imageCache);
		}

		CameraComponent& operator=(CameraComponent&& other) noexcept
		{
			_projectionType = other._projectionType;

			_projectionMatrix = std::move(other._projectionMatrix);
			_viewMatrix = std::move(other._viewMatrix);

			_nearClipDistance = other._nearClipDistance;
			_farClipDistance = other._farClipDistance;

			_perspectiveFieldOfView = other._perspectiveFieldOfView;
			_orthographicSize = other._orthographicSize;
			_aspectRatio = other._aspectRatio;

			_isProjectionMatrixDirty = other._isProjectionMatrixDirty;

			_renderTargetOverrides = std::move(other._renderTargetOverrides);
			_imageCache = std::move(other._imageCache);

			return *this;
		}

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
		const Matrix4x4& GetProjectionMatrix() noexcept;

		/// @brief Sets this camera's current view matrix
		/// @param view The view matrix
		void SetViewMatrix(const Matrix4x4& view) noexcept { _viewMatrix = view; }

		/// @brief Gets this camera's current view matrix
		/// @return The current view matrix
		const Matrix4x4& GetViewMatrix() const noexcept { return _viewMatrix; }

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
		void SetRenderTargetOverrides(const List<Ref<Rendering::Image>>& renderTargets) { _renderTargetOverrides = renderTargets; }

		/// @brief Gets rendertargets that match the given pipeline's attachment layout
		/// @param pipeline The pipeline
		/// @return A list of render targets
		List<Ref<Rendering::Image>> GetRenderTargets(Ref<Rendering::RenderPipeline> pipeline, const SizeInt& size);

		ManagedRef<Rendering::RenderView> GetRenderView(
			Ref<Rendering::RenderPipeline> pipeline,
			const SizeInt& backbufferSize,
			const List<Ref<Rendering::Image>>& backbuffers) final;

	private:
		/// @brief Updates the internal projection matrix based on the projection type
		void UpdateProjectionMatrix() noexcept;

		/// @brief Ensures there is an active rendering service and returns it
		/// @return The active rendering service
		Rendering::RenderingService* EnsureRenderingService() const;
	};
}