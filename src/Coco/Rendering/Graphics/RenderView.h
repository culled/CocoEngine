#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Resources/Resource.h>
#include "../RenderTarget.h"
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "../ShaderTypes.h"

namespace Coco::Rendering
{
	class Buffer;
	class Image;
	class ImageSampler;
	class Shader;
	class Mesh;
	class IMaterial;
	class Texture;

	/// @brief Rendering data for a shader
	struct ShaderRenderData
	{
		/// @brief The ID of the shader
		ResourceID ID;

		/// @brief The version of the shader
		ResourceVersion Version;

		/// @brief The subshaders that this shader uses
		UnorderedMap<string, Subshader> Subshaders;

		/// @brief The group that this shader is related to
		string GroupTag;

		ShaderRenderData() :
			ID(Resource::InvalidID), Version(0), GroupTag{}, Subshaders{}
		{}

		ShaderRenderData(const ResourceID& id, ResourceVersion version, const string& groupTag) : 
			ID(id), Version(version), GroupTag(groupTag), Subshaders{}
		{}
	};

	/// @brief Rendering data for a texture
	struct TextureRenderData
	{
		/// @brief The ID of the texture
		ResourceID ID;

		/// @brief The version of the texture
		ResourceVersion Version;

		/// @brief The image that the texture uses
		Ref<Image> Image;

		/// @brief The sampler that the texture uses
		Ref<ImageSampler> Sampler;

		TextureRenderData(const ResourceID& id, ResourceVersion version, const Ref<Rendering::Image>& image, const Ref<ImageSampler>& sampler) : 
			ID(id), Version(version), Image(image), Sampler(sampler)
		{}
	};

	/// @brief Rendering data for a material
	struct MaterialRenderData
	{
		/// @brief The ID of the material
		ResourceID ID;

		/// @brief The version of the material
		ResourceVersion Version;

		/// @brief The ID of the shader that this material uses
		ResourceID ShaderID;

		/// @brief The uniform data of this material
		ShaderUniformData UniformData;

		MaterialRenderData() :
			ID(Resource::InvalidID), Version(0), ShaderID(Resource::InvalidID), UniformData{}
		{}

		MaterialRenderData(const ResourceID& id, ResourceVersion version, const ResourceID& shaderID, const ShaderUniformData& uniformData) :
			ID(id), Version(version), ShaderID(shaderID), UniformData(uniformData)
		{}
	};

	struct SubmeshData
	{
		/// @brief The offset of the first index of the index buffer
		uint64_t FirstIndexOffset;

		/// @brief The number of indices
		uint64_t IndexCount;

		SubmeshData(uint64_t firstIndexOffset, uint64_t indexCount) :
			FirstIndexOffset(firstIndexOffset),
			IndexCount(indexCount)
		{}
	};

	/// @brief Rendering data for a mesh
	struct MeshRenderData
	{
		/// @brief The ID of the mesh
		ResourceID ID;

		/// @brief The version of the mesh
		ResourceVersion Version;

		/// @brief The vertex buffer that the mesh uses
		Ref<Buffer> VertexBuffer;

		/// @brief The number of vertices in the vertex buffer
		uint64_t VertexCount;

		/// @brief The index buffer that the mesh uses
		Ref<Buffer> IndexBuffer;

		/// @brief The submeshes within this mesh
		List<SubmeshData> Submeshes;
		
		MeshRenderData(
			const ResourceID& id, 
			ResourceVersion version, 
			const Ref<Buffer>& vertexBuffer, 
			uint64_t vertexCount, 
			const Ref<Buffer>& indexBuffer, 
			const List<SubmeshData>& submeshes) :
			ID(id), 
			Version(version), 
			VertexBuffer(vertexBuffer), 
			VertexCount(vertexCount), 
			IndexBuffer(indexBuffer),
			Submeshes(submeshes)
		{}
	};

	/// @brief Rendering data for an object
	struct ObjectRenderData
	{
		/// @brief The mesh data that this object uses
		ResourceID MeshData;

		/// @brief The index of the submesh that this object uses
		uint SubmeshIndex;

		/// @brief The material data that this object uses
		ResourceID MaterialData;
		
		/// @brief The model matrix for this object
		Matrix4x4 ModelMatrix;

		ObjectRenderData(const ResourceID& meshData, uint submeshIndex, const ResourceID& materialData, const Matrix4x4& modelMatrix) :
			MeshData(meshData), SubmeshIndex(submeshIndex), MaterialData(materialData), ModelMatrix(modelMatrix)
		{}
	};

	/// @brief Contains all neccessary information to render a scene from a specific view
	struct COCOAPI RenderView
	{
		/// @brief The viewport rectangle
		const RectInt ViewportRect;

		/// @brief The projection matrix used for rendering
		const Matrix4x4 Projection;

		/// @brief The projection matrix used for 2D rendering
		const Matrix4x4 Projection2D;

		/// @brief The view matrix used for rendering
		const Matrix4x4 View;

		/// @brief The position the view is rendering from
		const Vector3 ViewPosition;

		/// @brief The render targets used for rendering
		List<RenderTarget> RenderTargets;

		/// @brief The shaders used for rendering
		UnorderedMap<ResourceID, ShaderRenderData> Shaders;

		/// @brief The textures used for rendering
		UnorderedMap<ResourceID, TextureRenderData> Textures;

		/// @brief The materials used for rendering
		UnorderedMap<ResourceID, MaterialRenderData> Materials;

		/// @brief The meshes used for rendering
		UnorderedMap<ResourceID, MeshRenderData> Meshs;

		/// @brief The objects being rendered
		List<ObjectRenderData> Objects;

		RenderView(
			const RectInt& viewportRect,  
			const Matrix4x4& projection,
			const Matrix4x4& projection2D,
			const Matrix4x4& view, 
			const Vector3& viewPosition,
			const List<RenderTarget>& renderTargets
		) noexcept;

		~RenderView();
		
		/// @brief Adds an object to be rendered
		/// @param mesh The object's mesh
		/// @param submeshIndex The index of the submesh to be rendered
		/// @param material The object's material
		/// @param modelMatrix The model matrix for the object
		void AddRenderObject(Ref<Mesh> mesh, uint submeshIndex, Ref<IMaterial> material, const Matrix4x4& modelMatrix);

		/// @brief Adds a shader that will be used for rendering
		/// @param shader The shader
		void AddShader(Ref<Shader> shader);

		/// @brief Adds a texture that will be used for rendering
		/// @param texture The texture
		void AddTexture(Ref<Texture> texture);

		/// @brief Adds a material that will be used for rendering
		/// @param material The material
		void AddMaterial(Ref<IMaterial> material);

		/// @brief Adds a mesh that will be used for rendering
		/// @param mesh The mesh
		void AddMesh(Ref<Mesh> mesh);
	};
}