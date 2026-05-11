//
// Created by cullen on 3/31/26.
//

#ifndef COCOENGINE_GIZMOS_H
#define COCOENGINE_GIZMOS_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/ProcessLoop/TickListener.h"
#include "Coco/Core/Types/Color.h"
#include "Coco/Rendering/RenderListener.h"

namespace Coco
{
    /// @brief A draw call for a gizmo shape
    struct GizmoDrawCall
    {
        /// @brief The transform of this shape
        Matrix4x4 Transform;

        /// @brief The color of this shape
        Vector4 DrawColor;

        /// @brief The submesh of the gizmo mesh
        uint8 SubmeshID;

        GizmoDrawCall(
            uint8 submeshID,
            const Vector4& color,
            const Matrix4x4& transform);
    };

    /// @brief Manages drawing gizmos in a scene
    class Gizmos
    {
    public:
        /// @brief The tick order when the gizmos will be cleared each frame
        static constexpr int ClearTickOrder = -6000;

        Gizmos(RenderService* renderService);
        ~Gizmos();

        /// @brief Clears all gizmo draw calls
        void Clear();

        /// @brief Draws a line between two points
        /// @param start The starting position of the line
        /// @param end The ending position of the line
        /// @param color The color of the line
        void DrawLine3D(const Vector3& start, const Vector3& end, const Color& color);

        /// @brief Draws a ray starting at an origin and pointing in a direction
        /// @param origin The point where the ray begins
        /// @param direction The direction and magnitude of the ray
        /// @param color The color of the ray
        void DrawRay3D(const Vector3& origin, const Vector3& direction, const Color& color);

        /// @brief Draws a wireframe box
        /// @param origin The center of the box
        /// @param rotation The rotation of the box
        /// @param size The size of the box in each axis
        /// @param color The color of the box
        void DrawBox3D(const Vector3& origin, const Quaternion& rotation, const Vector3& size, const Color& color);

        /// @brief Draws a circle. With an identity rotation, the circle will be aligned to the XY plane
        /// @param origin The center of the circle
        /// @param rotation The rotation of the circle
        /// @param radius The radius of the circle
        /// @param color The color of the circle
        void DrawCircle3D(const Vector3& origin, const Quaternion& rotation, float radius, const Color& color);

        /// @brief Draws a wireframe sphere
        /// @param origin The center of the sphere
        /// @param radius The radius of the sphere
        /// @param color The color of the sphere
        void DrawSphere3D(const Vector3& origin, float radius, const Color& color);

        /// @brief Draws a wireframe cone. With an identity rotation, the cone will point in the +Y axis
        /// @param origin The base of the cone
        /// @param rotation The rotation of the cone
        /// @param height The height of the cone
        /// @param radius The radius of the base of the cone
        /// @param color The color of the cone
        void DrawCone3D(const Vector3& origin, const Quaternion& rotation, float height, float radius, const Color& color);

        /// @brief Renders all current gizmo drawcalls
        /// @param graph The render graph
        /// @param scene The render scene
        void Render(RenderGraph& graph, RenderScene& scene);

    private:
        SharedPtr<Mesh> _mesh;
        uint8 _lineSubmesh;
        uint8 _boxSubmesh;
        uint8 _circleSubmesh;
        uint8 _sphereSubmesh;
        uint8 _coneSubmesh;
        Array<GizmoDrawCall> _drawCalls;
        TickListener _clearTickListener;

        /// @brief Creates the line mesh data
        /// @param outVerts Will be filled with vertex positions
        /// @param outIndices Will be filled with vertex indices
        static void CreateLineMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices);

        /// @brief Creates the wireframe box mesh data
        /// @param outVerts Will be filled with vertex positions
        /// @param outIndices Will be filled with vertex indices
        static void CreateBoxMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices);

        /// @brief Creates wireframe circle mesh data. Without any rotation, the circle is aligned to the XY plane
        /// @param rotation The rotation of the circle
        /// @param outVerts Will be filled with vertex positions
        /// @param outIndices Will be filled with vertex indices
        static void CreateCircleMesh(const Quaternion& rotation, ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices);

        /// @brief Creates the wireframe sphere mesh data
        /// @param outVerts Will be filled with vertex positions
        /// @param outIndices Will be filled with vertex indices
        static void CreateSphereMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices);

        /// @brief Creates the wireframe cone mesh data. The cone's base is aligned to the XZ plane
        /// @param outVerts Will be filled with vertex positions
        /// @param outIndices Will be filled with vertex indices
        static void CreateConeMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices);

        /// @brief Creates the gizmo resources
        void CreateResources();

        /// @brief Tick handler for the clear callback
        /// @param tickInfo The tick info
        void OnClearTick(const TickInfo& tickInfo);
    };
} // Coco

#endif //COCOENGINE_GIZMOS_H