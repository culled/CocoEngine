#pragma once

#include "../../ECSpch.h"
#include "../EntityComponent.h"
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/ViewFrustum.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineTypes.h>
#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>

namespace Coco::Rendering
{
    class Image;
    class RenderPipeline;
    class GraphicsPresenter;
}

namespace Coco::ECS
{
    /// @brief Types of camera projections
    enum class CameraProjectionType
    {
        Perspective,
        Orthographic
    };

    /// @brief A component that allows a scene to be rendered from its perspective
    class CameraComponent :
        public EntityComponent
    {
        friend class CameraComponentSerializer;

    private:
        Color _clearColor;
        CameraProjectionType _projectionType;

        double _perspectiveNearClip;
        double _perspectiveFarClip;
        double _perspectiveFOV;

        double _orthoNearClip;
        double _orthoFarClip;
        double _orthoSize;

        Rendering::MSAASamples _sampleCount;
        int _priority;

    public:
        CameraComponent(const Entity& owner);

        /// @brief Sets this camera to use a perspective projection
        /// @param verticalFOVRadians The vertical field of view, in radians
        /// @param nearClip The distance to the near clip plane
        /// @param farClip The distance to the far clip plane
        void SetPerspectiveProjection(double verticalFOVRadians, double nearClip, double farClip);

        /// @brief Sets this camera to use an orthographic projection
        /// @param size The vertical size of the orthographic frustum
        /// @param nearClip The distance to the near clip plane
        /// @param farClip The distance to the far clip plane
        void SetOrthographicProjection(double size, double nearClip, double farClip);

        /// @brief Gets the projection matrix for this camera
        /// @param aspectRatio The aspect ratio of the matrix
        /// @return The projection matrix
        Matrix4x4 GetProjectionMatrix(double aspectRatio) const;

        /// @brief Gets the view frustum for this camera
        /// @param aspectRatio The aspect ratio of the frustum
        /// @param position The origin of the frustum
        /// @param rotation The rotation of the camera
        /// @return The view frustum
        ViewFrustum GetViewFrustum(double aspectRatio, const Vector3& position, const Quaternion& rotation) const;

        /// @brief Sets the projection type of this camera
        /// @param projectionType The projection type
        void SetProjectionType(CameraProjectionType projectionType);

        /// @brief Gets the projection type of this camera
        /// @return The projection type
        CameraProjectionType GetProjectionType() const { return _projectionType; }

        /// @brief Sets this camera's priority
        /// @param priority The priority
        void SetPriority(int priority);

        /// @brief Gets this camera's priority
        /// @return The priority
        int GetPriority() const { return _priority; }

        void SetNearClip(double nearClip);

        /// @brief Gets the near clip distance for this camera
        /// @return The near clip distance
        double GetNearClip() const;

        void SetFarClip(double farClip);

        /// @brief Gets the far clip distance for this camera
        /// @return The far clip distance
        double GetFarClip() const;

        /// @brief Sets the clear color of this camera
        /// @param clearColor The clear color
        void SetClearColor(const Color& clearColor);

        /// @brief Gets the clear color of this camera
        /// @return The clear color
        const Color& GetClearColor() const { return _clearColor; }

        void SetPerspectiveFOV(double verticalFOVRadians);
        double GetPerspectiveFOV() const { return _perspectiveFOV; }

        void SetOrthographicSize(double verticalSize);
        double GetOrthographicSize() const { return _orthoSize; }

        /// @brief Sets the MSAA sample count of this camera
        /// @param sampleCount The number of MSAA samples
        void SetSampleCount(Rendering::MSAASamples sampleCount);

        /// @brief Gets the MSAA sample count of this camera
        /// @return The number of MSAA samples
        Rendering::MSAASamples GetSampleCount() const { return _sampleCount; }

        /// @brief Renders from this camera's perspective
        /// @param framebuffers The framebuffers to render with
        /// @param pipeline The RenderPipeline to use
        /// @param layoutOverride The layout override for the global uniforms
        void Render(
            std::span<Ref<Rendering::Image>> framebuffers,
            Rendering::RenderPipeline& pipeline,
            std::optional<Rendering::GlobalShaderUniformLayout> layoutOverride = std::optional<Rendering::GlobalShaderUniformLayout>());

        /// @brief Renders from this camera's perspective
        /// @param presenter The presenter to render to
        /// @param pipeline The RenderPipeline to use
        /// @param layoutOverride The layout override for the global uniforms
        void Render(
            Ref<Rendering::GraphicsPresenter> presenter,
            Rendering::RenderPipeline& pipeline,
            std::optional<Rendering::GlobalShaderUniformLayout> layoutOverride = std::optional<Rendering::GlobalShaderUniformLayout>());
    };
}