#include "RenderView.h"

#include "Resources/Buffer.h"
#include "Resources/Image.h"
#include "Resources/ImageSampler.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Mesh.h"

namespace Coco::Rendering
{
	RenderView::RenderView(
		const RectInt& viewportRect, 
		const Color& clearColor, 
		const Matrix4x4& projection, 
		const Matrix4x4& view,
		const List<Ref<Image>>& renderTargets
	) noexcept :
		ViewportRect(viewportRect), ClearColor(clearColor), Projection(projection), View(view), RenderTargets(renderTargets)
	{}

	RenderView::~RenderView()
	{
		RenderTargets.Clear();
		Shaders.clear();
		Textures.clear();
		Materials.clear();
		Meshs.clear();
		Objects.Clear();
	}

	void RenderView::AddRenderObject(Ref<Mesh> mesh, Ref<Material> material, const Matrix4x4& modelMatrix)
	{
		AddMaterial(material);
		AddMesh(mesh);

		Objects.Add(ObjectRenderData(mesh->GetID(), material->GetID(), modelMatrix));
	}

	void RenderView::AddShader(Ref<Shader> shader)
	{
		if (Shaders.contains(shader->GetID()))
			return;

		Shaders.emplace(shader->GetID(), ShaderRenderData(shader->GetID(), shader->GetVersion()));
		ShaderRenderData& shaderData = Shaders.at(shader->GetID());

		for (const Subshader& subshader : shader->GetSubshaders())
		{
			shaderData.Subshaders.emplace(subshader.PassName, subshader);
		}
	}

	void RenderView::AddTexture(Ref<Texture> texture)
	{
		if (Textures.contains(texture->GetID()))
			return;

		Textures.emplace(texture->GetID(), TextureRenderData(texture->GetID(), texture->GetVersion(), texture->GetImage(), texture->GetSampler()));
	}

	void RenderView::AddMaterial(Ref<Material> material)
	{
		if (Materials.contains(material->GetID()))
			return;

		Ref<Shader> shader = material->GetShader();
		ResourceID shaderID = shader.IsValid() ? shader->GetID() : Resource::InvalidID;

		if (shader.IsValid())
			AddShader(shader);

		Materials.emplace(material->GetID(), MaterialRenderData(material->GetID(), material->GetVersion(), shaderID, material->GetBufferData()));
		MaterialRenderData& materialData = Materials.at(material->GetID());

		for (const auto& kvp : material->GetSubshaderBindings())
			materialData.SubshaderBindings.emplace(kvp.first, kvp.second);

		for (const auto& kvp : material->GetTextureProperties())
		{
			Ref<Texture> texture = kvp.second;

			if (texture.IsValid())
			{
				AddTexture(texture);
				materialData.Samplers.emplace(kvp.first, texture->GetID());
			}
			else
			{
				materialData.Samplers.emplace(kvp.first, Resource::InvalidID);
			}
		}
	}

	void RenderView::AddMesh(Ref<Mesh> mesh)
	{
		if (Meshs.contains(mesh->GetID()))
			return;

		Meshs.emplace(mesh->GetID(), MeshRenderData(mesh->GetID(), mesh->GetVersion(), mesh->GetVertexBuffer(), mesh->GetVertexCount(), mesh->GetIndexBuffer(), mesh->GetIndexCount()));
	}
}