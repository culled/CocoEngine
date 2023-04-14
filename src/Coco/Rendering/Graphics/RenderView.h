#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "../ShaderTypes.h"
#include "../MaterialTypes.h"

namespace Coco::Rendering
{
	class Buffer;
	class Image;
	class ImageSampler;
	class Shader;
	class Mesh;
	class Material;
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

		ShaderRenderData(ResourceID id, ResourceVersion version) : 
			ID(id), Version(version)
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
		WeakManagedRef<Image> Image;

		/// @brief The sampler that the texture uses
		WeakManagedRef<ImageSampler> Sampler;

		TextureRenderData(ResourceID id, ResourceVersion version, const WeakManagedRef<Rendering::Image>& image, const WeakManagedRef<ImageSampler>& sampler) : 
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
		List<uint8_t> UniformData;

		/// @brief Bindings between subshaders and their places in this material's uniform data
		UnorderedMap<string, SubshaderUniformBinding> SubshaderBindings;

		/// @brief The textures that this material samples
		UnorderedMap<string, ResourceID> Samplers;

		MaterialRenderData(ResourceID id, ResourceVersion version, ResourceID shaderID, const List<uint8_t>& uniformData) : 
			ID(id), Version(version), ShaderID(shaderID), UniformData(uniformData)
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
		WeakManagedRef<Buffer> VertexBuffer;

		/// @brief The number of vertices in the vertex buffer
		uint64_t VertexCount;

		/// @brief The index buffer that the mesh uses
		WeakManagedRef<Buffer> IndexBuffer;

		/// @brief The number of vertex indices in the index buffer
		uint64_t IndexCount;

		MeshRenderData(
			ResourceID id, 
			ResourceVersion version, 
			const WeakManagedRef<Buffer>& vertexBuffer, 
			uint64_t vertexCount, 
			const WeakManagedRef<Buffer>& indexBuffer, 
			uint64_t indexCount) :
			ID(id), Version(version), VertexBuffer(vertexBuffer), VertexCount(vertexCount), IndexBuffer(indexBuffer), IndexCount(indexCount)
		{}
	};

	/// @brief Rendering data for an object
	struct ObjectRenderData
	{
		/// @brief The mesh data that this object uses
		ResourceID MeshData;

		/// @brief The material data that this object uses
		ResourceID MaterialData; // TODO: multiple materials per mesh
		
		/// @brief The model matrix for this object
		Matrix4x4 ModelMatrix;

		ObjectRenderData(ResourceID meshData, ResourceID materialData, const Matrix4x4& modelMatrix) :
			MeshData(meshData), MaterialData(materialData), ModelMatrix(modelMatrix)
		{}
	};

	/// @brief Contains all neccessary information to render a scene from a specific view
	struct COCOAPI RenderView
	{
		/// @brief The viewport rectangle
		const RectInt ViewportRect;

		/// @brief The clear color for the render
		const Color ClearColor;

		/// @brief The projection matrix used for rendering
		const Matrix4x4 Projection;

		/// @brief The view matrix used for rendering
		const Matrix4x4 View;

		/// @brief The render targets used for rendering
		List<WeakManagedRef<Image>> RenderTargets;

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
			const Color& clearColor, 
			const Matrix4x4& projection, 
			const Matrix4x4& view, 
			const List<WeakManagedRef<Image>>& renderTargets
		) noexcept;
		
		/// @brief Adds an object to be rendered
		/// @param mesh The object's mesh
		/// @param material The object's material
		/// @param modelMatrix The model matrix for the object
		void AddRenderObject(const Ref<Mesh>& mesh, const Ref<Material>& material, const Matrix4x4& modelMatrix);

		/// @brief Adds a shader that will be used for rendering
		/// @param shader The shader
		void AddShader(const Ref<Shader>& shader);

		/// @brief Adds a texture that will be used for rendering
		/// @param texture The texture
		void AddTexture(const Ref<Texture>& texture);

		/// @brief Adds a material that will be used for rendering
		/// @param material The material
		void AddMaterial(const Ref<Material>& material);

		/// @brief Adds a mesh that will be used for rendering
		/// @param mesh The mesh
		void AddMesh(const Ref<Mesh>& mesh);
	};
}