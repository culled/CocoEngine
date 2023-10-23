#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Vector.h>
#include "MeshTypes.h"

namespace Coco::Rendering
{
	/// @brief Static class for various mesh utilities
	class MeshUtilities
	{
	public:
		/// @brief Creates a flat grid on the X-Y plane, with default faces facing in the +Z direction
		/// @param size The size of the grid
		/// @param offset The position of the center of the grid
		/// @param format The format of the vertices to add
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param subdivisions The number of cuts along each side of the grid
		/// @param flipDirection If true, the faces & normals will point in the -Z direction
		static void CreateXYGrid(
			const Vector2& size,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			uint32 subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates a flat grid on the X-Z plane, with default faces facing in the +Y direction
		/// @param size The size of the grid
		/// @param offset The position of the center of the grid
		/// @param format The format of the vertices to add
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param subdivisions The number of cuts along each side of the grid
		/// @param flipDirection If true, the faces & normals will point in the -Y direction
		static void CreateXZGrid(
			const Vector2& size,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			uint32 subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates a flat grid on the Z-Y plane, with default faces facing in the +X direction
		/// @param size The size of the grid
		/// @param offset The position of the center of the grid
		/// @param format The format of the vertices to add
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param subdivisions The number of cuts along each side of the grid
		/// @param flipDirection If true, the faces & normals will point in the -X direction
		static void CreateZYGrid(
			const Vector2& size,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			uint32 subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates a box, with default faces facing outward
		/// @param size The size of the box on each axis
		/// @param offset The position of the center of the box
		/// @param format The format of the vertices to add 
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param subdivisions The number of cuts along each side of the box
		/// @param flipDirection If true, the faces & normals will point towards the inside of the box
		static void CreateBox(
			const Vector3& size,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			uint32 subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates a triangle fan on the X-Y plane, with default faces facing in the +Z direction
		/// @param radius The radius of the circle
		/// @param vertexCount The number of vertices in the circle
		/// @param offset The position of the center of the circle
		/// @param format The format of the vertices to add  
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param flipDirection If true, the faces & normals will point towards the -Z direction
		static void CreateXYTriangleFan(
			double radius,
			uint32 vertexCount,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			bool flipDirection = false);

		/// @brief Creates a triangle fan on the X-Z plane, with default faces facing in the +Y direction
		/// @param radius The radius of the circle
		/// @param vertexCount The number of vertices in the circle
		/// @param offset The position of the center of the circle
		/// @param format The format of the vertices to add  
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param flipDirection If true, the faces & normals will point towards the -Y direction
		static void CreateXZTriangleFan(
			double radius,
			uint32 vertexCount,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			bool flipDirection = false);

		/// @brief Creates a triangle fan on the Z-Y plane, with default faces facing in the +X direction
		/// @param radius The radius of the circle
		/// @param vertexCount The number of vertices in the circle
		/// @param offset The position of the center of the circle
		/// @param format The format of the vertices to add  
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param flipDirection If true, the faces & normals will point towards the -X direction
		static void CreateZYTriangleFan(
			double radius,
			uint32 vertexCount,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			bool flipDirection = false);

		/// @brief Creates a cone, with default faces facing outward
		/// @param height The height of the cone
		/// @param radius The radius of the cone's base
		/// @param baseVertexCount The number of vertices in the base
		/// @param offset The position of the center of the base of the cone
		/// @param format The format of the vertices to add  
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param flipDirection If true, the faces & normals will point inwards
		static void CreateCone(
			double height,
			double radius,
			uint32 baseVertexCount,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			bool flipDirection = false);

		/// @brief Creates a UV sphere, with default faces facing outward
		/// @param slices The number of vertical slices
		/// @param stacks The number of horizontal stacks
		/// @param radius The radius of the sphere
		/// @param offset The position of the center of the sphere
		/// @param format The format of the vertices to add  
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param flipDirection If true, the faces & normals will point inwards
		static void CreateUVSphere(
			uint32 slices, uint32 stacks,
			double radius,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			bool flipDirection = false);

		/// @brief Calculates normals for vertices
		/// @param vertices The vertices 
		/// @param indices The vertex indices
		static void CalculateNormals(std::vector<VertexData>& vertices, std::span<const uint32> indices);

		/// @brief Calculates tangents for vertices
		/// @param vertices The vertices
		/// @param indices The vertex indices
		static void CalculateTangents(std::vector<VertexData>& vertices, std::span<const uint32> indices);
	};
}