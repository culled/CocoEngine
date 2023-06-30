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
		/// @param size The size of the mesh
		/// @param subdivisions The number of subdivisions for the plane
		/// @return The plane mesh
		static Ref<Mesh> CreateXYPlane(const Vector2& size, uint subdivisions = 0);

		/// @brief Creates a mesh along the XZ plane
		/// @param size The size of the mesh
		/// @param subdivisions The number of subdivisions for the plane
		/// @return The plane mesh
		static Ref<Mesh> CreateXZPlane(const Vector2& size, uint subdivisions = 0);

		/// @brief Creates a mesh along the YZ plane
		/// @param size The size of the mesh
		/// @param subdivisions The number of subdivisions for the plane
		/// @return The plane mesh
		static Ref<Mesh> CreateYZPlane(const Vector2& size, uint subdivisions = 0);

		/// @brief Creates a box mesh
		/// @param size The size of the box
		/// @param subdivisions The number of subdivisions for the box
		/// @return The box mesh
		static Ref<Mesh> CreateBox(const Vector3& size, uint subdivisions = 0);

		/// @brief Creates verticies for an XY grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		static void CreateXYGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// @brief Creates verticies for an XZ grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		static void CreateXZGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// @brief Creates verticies for an YZ grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		static void CreateYZGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// @brief Creates vertices for a box
		/// @param size The size of the box
		/// @param offset The offset of the box center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the box
		static void CreateBox(const Vector3& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// @brief Creates a mesh from vertices
		/// @param positions The vertex positions
		/// @param uvs The vertex UVs
		/// @param indices The vertex indices
		/// @return A mesh with the given vertices
		static Ref<Mesh> CreateFromVertices(const List<Vector3>& positions, const List<Vector2>& uvs, const List<uint>& indices, const string& name = "Mesh");
	};
}