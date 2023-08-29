#pragma once

#include <Coco/Core/Core.h>

#include "Mesh.h"

namespace Coco::Rendering
{
	/// @brief A static class for creating mesh primitives
	class COCOAPI MeshPrimitives
	{
	public:
		/// @brief Creates a mesh along the XY plane
		/// @param name The name of the mesh
		/// @param size The size of the mesh
		/// @param offset The offset of the plane
		/// @param subdivisions The number of subdivisions for the plane
		/// @param flipDirection If true, the plane will face the -Z direction
		/// @return The plane mesh
		static Ref<Mesh> CreateXYPlane(const string& name, const Vector2& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates a mesh along the XZ plane
		/// @param name The name of the mesh
		/// @param size The size of the mesh
		/// @param offset The offset of the plane
		/// @param subdivisions The number of subdivisions for the plane
		/// @param flipDirection If true, the plane will face the -Y direction
		/// @return The plane mesh
		static Ref<Mesh> CreateXZPlane(const string& name, const Vector2& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates a mesh along the YZ plane
		/// @param name The name of the mesh
		/// @param size The size of the mesh
		/// @param offset The offset of the plane
		/// @param subdivisions The number of subdivisions for the plane
		/// @param flipDirection If true, the plane will face the -X direction
		/// @return The plane mesh
		static Ref<Mesh> CreateYZPlane(const string& name, const Vector2& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates a box mesh
		/// @param name The name of the mesh
		/// @param size The size of the box
		/// @param offset The offset of the box
		/// @param subdivisions The number of subdivisions for the box
		/// @param flipDirection If true, the faces will face towards the inside of the box
		/// @return The box mesh
		static Ref<Mesh> CreateBox(const string& name, const Vector3& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates a cone mesh
		/// @param name The name of the mesh
		/// @param height The height of the cone
		/// @param radius The radius of the cone's base
		/// @param baseVertexCount The number of vertices for the cone's base
		/// @param offset The offset of the cone
		/// @param flipDirection If true, the faces will face towards the inside of the cone
		/// @return The cone mesh
		static Ref<Mesh> CreateCone(const string& name, double height, double radius, int baseVertexCount, const Vector3& offset = Vector3::Zero, bool flipDirection = false);

		/// @brief Creates a UV sphere mesh
		/// @param name The name of the mesh
		/// @param slices The number of vertical slices
		/// @param stacks The number of horizontal slices
		/// @param radius The radius
		/// @param offset The world-space offset
		/// @param flipDirection If true, the faces will face inside the sphere
		/// @return The sphere mesh
		static Ref<Mesh> CreateUVSphere(const string& name, uint slices, uint stacks, double radius, const Vector3& offset = Vector3::Zero, bool flipDirection = false);

		/// @brief Creates verticies for an XY grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param vertices Will be filled with the vertex data
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		/// @param flipDirection If true, the grid will face the -Z direction
		static void CreateXYGrid(
			const Vector2& size, 
			const Vector3& offset, 
			List<VertexData>& vertices,
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates verticies for an XZ grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param vertices Will be filled with the vertex data
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		/// @param flipDirection If true, the grid will face the -Y direction
		static void CreateXZGrid(
			const Vector2& size, 
			const Vector3& offset,
			List<VertexData>& vertices,
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates verticies for an YZ grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param vertices Will be filled with the vertex data
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		/// @param flipDirection If true, the grid will face the -X direction
		static void CreateYZGrid(
			const Vector2& size, 
			const Vector3& offset,
			List<VertexData>& vertices,
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates vertices for a box
		/// @param size The size of the box
		/// @param offset The offset of the box center
		/// @param vertices Will be filled with the vertex data
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the box
		/// @param flipDirection If true, the faces will face towards the inside of the box
		static void CreateBox(
			const Vector3& size, 
			const Vector3& offset,
			List<VertexData>& vertices,
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates vertices for a cone
		/// @param height The height of the cone
		/// @param radius The radius of the cone's base
		/// @param baseVertexCount The number of vertices for the cone's base
		/// @param offset The offset of the cone
		/// @param vertices Will be filled with the vertex data
		/// @param indices Will be filled with the vertex indices
		/// @param flipDirection If true, the faces will face towards the inside of the cone
		static void CreateCone(
			double height,
			double radius,
			int baseVertexCount,
			const Vector3& offset,
			List<VertexData>& vertices,
			List<uint>& indices,
			bool flipDirection = false);

		/// @brief Creates a circular triangle fan on the XY plane
		/// @param radius The radius of the circle
		/// @param vertexCount The number of vertices in the circle
		/// @param offset The offset of the circle
		/// @param vertices Will be filled with the vertex data
		/// @param indices Will be filled with the vertex indices
		/// @param flipDirection If true, the faces will face in the -Z direction
		static void CreateXYTriangleFan(
			double radius,
			int vertexCount,
			const Vector3& offset,
			List<VertexData>& vertices,
			List<uint>& indices,
			bool flipDirection = false);

		/// @brief Creates a UV sphere
		/// @param slices The number of vertical slices
		/// @param stacks The number of horizontal slices
		/// @param radius The radius
		/// @param offset The world-space offset
		/// @param vertices Will be filled with the vertex data
		/// @param indices Will be filled with vertex indices
		/// @param flipDirection If true the faces will face inside the sphere
		static void CreateUVSphere(
			uint slices, uint stacks, 
			double radius,
			const Vector3& offset,
			List<VertexData>& vertices,
			List<uint>& indices,
			bool flipDirection = false);

		/// @brief Creates a mesh from vertices
		/// @param name The name of the mesh
		/// @param vertices The vertex data
		/// @param indices The vertex indices
		/// @param calculateNormals If true, normals will be automatically calculated
		/// @param calculateTangents If true, tangents will be automatically calculated
		/// @return A mesh with the given vertices
		static Ref<Mesh> CreateFromVertices(
			const string& name,
			const List<VertexData>& vertices,
			const List<uint>& indices,
			bool calculateNormals = false,
			bool calculateTangents = false);
	};
}