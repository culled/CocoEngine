#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Singleton.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/BoundingBox.h>
#include <Coco/Core/Types/ViewFrustum.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "../Providers/SceneDataProvider.h"
#include "../MeshTypes.h"

namespace Coco::Rendering
{
	class Mesh;
	class Shader;
	class Material;

	/// @brief A draw call for a gizmo shape
	struct GizmoDrawCall
	{
		/// @brief The offset of the first index in the gizmo mesh buffer
		uint64 FirstIndexOffset;

		/// @brief The number of indices to draw in the gizmo mesh buffer
		uint64 IndexCount;

		/// @brief The color of this shape
		Color Color;

		/// @brief The transform of this shape
		Matrix4x4 Transform;

		GizmoDrawCall(
			uint64 firstIndexOffset,
			uint64 indexCount,
			const Coco::Color& color,
			const Matrix4x4& transform);
	};

	/// @brief Provides gizmo rendering support
	class GizmoRender :
		public Singleton<GizmoRender>,
		public SceneDataProvider
	{
		friend class GizmoRenderPass;

	public:
		static const int sLateTickPriority;

	private:
		static const VertexDataFormat _sVertexFormat;
		static const uint64 _sVisibilityGroup;
		static const string _sShaderName;

		ManagedRef<TickListener> _lateTickListener;

		std::vector<GizmoDrawCall> _drawCalls;
		std::pair<uint64, uint64> _lineIndexInfo;
		std::pair<uint64, uint64> _boxIndexInfo;
		std::pair<uint64, uint64> _sphereIndexInfo;

		SharedRef<Mesh> _mesh;

	public:
		GizmoRender();
		~GizmoRender();

		// Inherited via SceneDataProvider
		void GatherSceneData(RenderView& renderView) override;

		/// @brief Draws a line between start and end
		/// @param start The start position
		/// @param end The end position
		/// @param color The line color
		void DrawLine3D(const Vector3& start, const Vector3& end, const Color& color);

		/// @brief Draws a ray that starts at origin and points in the given direction
		/// @param origin The ray origin
		/// @param direction The direction of the ray
		/// @param color The ray color
		void DrawRay3D(const Vector3& origin, const Vector3& direction, const Color& color);

		/// @brief Draws a wire box
		/// @param origin The center of the box
		/// @param rotation The rotation of the box
		/// @param size The size of the box on each axis
		/// @param color The color of the box
		void DrawWireBox(const Vector3& origin, const Quaternion& rotation, const Vector3& size, const Color& color);

		/// @brief Draws a 1x1x1 wire box using a transform
		/// @param transform The transform
		/// @param color The color of the box
		void DrawWireBox(const Matrix4x4& transform, const Color& color);

		/// @brief Draws a bounding box transformed by the given transform
		/// @param box The bounding box
		/// @param transform The transform
		/// @param color The color of the box
		void DrawWireBounds(const BoundingBox& box, const Matrix4x4& transform, const Color& color);

		/// @brief Draws a wire sphere
		/// @param radius The radius of the sphere
		/// @param position The position of the sphere
		/// @param color The color of the sphere
		void DrawWireSphere(double radius, const Vector3& position, const Color& color);

		/// @brief Draws a wire view frustum
		/// @param frustum The view frustum
		/// @param color The color of the frustum
		void DrawFrustum(const ViewFrustum& frustum, const Color& color);

		/// @brief Draws a wire grid
		/// @param position The position of the grid center
		/// @param rotation The rotation of the grid
		/// @param size The total size of the grid
		/// @param squares The number of squares along each side of the grid
		/// @param color The color of the grid
		void DrawGrid(const Vector3& position, const Quaternion& rotation, double size, int squares, const Color& color);

	private:
		/// @brief Sets up the debug mesh
		void SetupMesh();

		/// @brief Handles the late tick
		/// @param tickInfo The tick info
		void HandleLateTick(const TickInfo& tickInfo);
	};
}