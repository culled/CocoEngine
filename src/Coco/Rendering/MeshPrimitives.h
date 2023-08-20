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
		/// @param offset The offset of the plane
		/// @param subdivisions The number of subdivisions for the plane
		/// @param flipDirection If true, the plane will face the -Z direction
		/// @return The plane mesh
		static Ref<Mesh> CreateXYPlane(const string& name, const Vector2& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates a mesh along the XZ plane
		/// @param size The size of the mesh
		/// @param offset The offset of the plane
		/// @param subdivisions The number of subdivisions for the plane
		/// @param flipDirection If true, the plane will face the -Y direction
		/// @return The plane mesh
		static Ref<Mesh> CreateXZPlane(const string& name, const Vector2& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates a mesh along the YZ plane
		/// @param size The size of the mesh
		/// @param offset The offset of the plane
		/// @param subdivisions The number of subdivisions for the plane
		/// @param flipDirection If true, the plane will face the -X direction
		/// @return The plane mesh
		static Ref<Mesh> CreateYZPlane(const string& name, const Vector2& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates a box mesh
		/// @param size The size of the box
		/// @param offset The offset of the box
		/// @param subdivisions The number of subdivisions for the box
		/// @param flipDirection If true, the faces will face towards the inside of the box
		/// @return The box mesh
		static Ref<Mesh> CreateBox(const string& name, const Vector3& size, const Vector3& offset = Vector3::Zero, uint subdivisions = 0, bool flipDirection = false);

		/// @brief Creates verticies for an XY grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		/// @param flipDirection If true, the grid will face the -Z direction
		static void CreateXYGrid(
			const Vector2& size, 
			const Vector3& offset, 
			List<Vector3>& positions, 
			List<Vector2>& uvs, 
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates verticies for an XZ grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		/// @param flipDirection If true, the grid will face the -Y direction
		static void CreateXZGrid(
			const Vector2& size, 
			const Vector3& offset, 
			List<Vector3>& positions, 
			List<Vector2>& uvs, 
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates verticies for an YZ grid
		/// @param size The size of the grid
		/// @param offset The offset of the grid center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the grid
		/// @param flipDirection If true, the grid will face the -X direction
		static void CreateYZGrid(
			const Vector2& size, 
			const Vector3& offset, 
			List<Vector3>& positions,
			List<Vector2>& uvs, 
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates vertices for a box
		/// @param size The size of the box
		/// @param offset The offset of the box center
		/// @param positions Will be filled with the vertex positions
		/// @param uvs Will be filled with the vertex UV coordinates
		/// @param indices Will be filled with the vertex indices
		/// @param subdivisions The number of subdivisions for the box
		/// @param flipDirection If true, the faces will face towards the inside of the box
		static void CreateBox(
			const Vector3& size, 
			const Vector3& offset, 
			List<Vector3>& positions, 
			List<Vector2>& uvs, 
			List<uint>& indices, 
			uint subdivisions = 0,
			bool flipDirection = false);

		/// @brief Creates a mesh from vertices
		/// @param positions The vertex positions
		/// @param uvs The vertex UVs
		/// @param indices The vertex indices
		/// @return A mesh with the given vertices
		static Ref<Mesh> CreateFromVertices(const string& name, const List<Vector3>& positions, const List<Vector2>& uvs, const List<uint>& indices);
	};
}