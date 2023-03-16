#pragma once

#include <Coco/Core/Core.h>
#include "Mesh.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A static class for creating mesh primitives
	/// </summary>
	class COCOAPI MeshPrimitives
	{
	public:
		/// <summary>
		/// Creates a mesh along the XY plane
		/// </summary>
		/// <param name="size">The size of the mesh</param>
		/// <param name="subdivisions">The number of subdivisions for the plane</param>
		/// <returns>The plane mesh</returns>
		static Ref<Mesh> CreateXYPlane(const Vector2& size, uint subdivisions = 0);

		/// <summary>
		/// Creates a mesh along the XZ plane
		/// </summary>
		/// <param name="size">The size of the mesh</param>
		/// <param name="subdivisions">The number of subdivisions for the plane</param>
		/// <returns>The plane mesh</returns>
		static Ref<Mesh> CreateXZPlane(const Vector2& size, uint subdivisions = 0);

		/// <summary>
		/// Creates a mesh along the YZ plane
		/// </summary>
		/// <param name="size">The size of the mesh</param>
		/// <param name="subdivisions">The number of subdivisions for the plane</param>
		/// <returns>The plane mesh</returns>
		static Ref<Mesh> CreateYZPlane(const Vector2& size, uint subdivisions = 0);

		/// <summary>
		/// Creates a box mesh
		/// </summary>
		/// <param name="size">The size of the box</param>
		/// <param name="subdivisions">The number of subdivisions for the box</param>
		/// <returns>The box mesh</returns>
		static Ref<Mesh> CreateBox(const Vector3& size, uint subdivisions = 0);

		/// <summary>
		/// Creates verticies for an XY grid
		/// </summary>
		/// <param name="size">The size of the grid</param>
		/// <param name="offset">The offset of the grid center</param>
		/// <param name="positions">The list of vertex positions</param>
		/// <param name="uvs">The list of vertex UVs</param>
		/// <param name="indices">The list of vertex indices</param>
		/// <param name="subdivisions">The number of subdivisions for the grid</param>
		static void CreateXYGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// <summary>
		/// Creates verticies for an XZ grid
		/// </summary>
		/// <param name="size">The size of the grid</param>
		/// <param name="offset">The offset of the grid center</param>
		/// <param name="positions">The list of vertex positions</param>
		/// <param name="uvs">The list of vertex UVs</param>
		/// <param name="indices">The list of vertex indices</param>
		/// <param name="subdivisions">The number of subdivisions for the grid</param>
		static void CreateXZGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// <summary>
		/// Creates verticies for an YZ grid
		/// </summary>
		/// <param name="size">The size of the grid</param>
		/// <param name="offset">The offset of the grid center</param>
		/// <param name="positions">The list of vertex positions</param>
		/// <param name="uvs">The list of vertex UVs</param>
		/// <param name="indices">The list of vertex indices</param>
		/// <param name="subdivisions">The number of subdivisions for the grid</param>
		static void CreateYZGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// <summary>
		/// Creates a box mesh
		/// </summary>
		/// <param name="size">The size of the box</param>
		/// <param name="offset">The offset of the box center</param>
		/// <param name="positions">The list of vertex positions</param>
		/// <param name="uvs">The list of vertex UVs</param>
		/// <param name="indices">The list of vertex indices</param>
		/// <param name="subdivisions">The number of subdivisions for the box</param>
		static void CreateBox(const Vector3& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions = 0);

		/// <summary>
		/// Creates a mesh from vertices
		/// </summary>
		/// <param name="positions">The vertex positions</param>
		/// <param name="uvs">The vertex UVs</param>
		/// <param name="indices">The vertex UVs</param>
		/// <returns>A mesh with the given vertices</returns>
		static Ref<Mesh> CreateFromVertices(const List<Vector3>& positions, const List<Vector2>& uvs, const List<uint>& indices);
	};
}