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
		const Matrix4x4& projection, 
		const Matrix4x4& view,
		const List<RenderTarget>& renderTargets
	) noexcept :
		ViewportRect(viewportRect), Projection(projection), View(view), RenderTargets(renderTargets)
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

		Objects.Construct(mesh->ID, material->ID, modelMatrix);
	}

	void RenderView::AddShader(Ref<Shader> shader)
	{
		if (Shaders.contains(shader->ID))
			return;

		Shaders.try_emplace(shader->ID, shader->ID, shader->GetVersion());
		ShaderRenderData& shaderData = Shaders.at(shader->ID);

		for (const Subshader& subshader : shader->GetSubshaders())
		{
			shaderData.Subshaders.try_emplace(subshader.PassName, subshader);
		}
	}

	void RenderView::AddTexture(Ref<Texture> texture)
	{
		if (Textures.contains(texture->ID))
			return;

		Textures.try_emplace(texture->ID, texture->ID, texture->GetVersion(), texture->GetImage(), texture->GetSampler());
	}

	void RenderView::AddMaterial(Ref<Material> material)
	{
		if (Materials.contains(material->ID))
			return;

		Ref<Shader> shader = material->GetShader();
		ResourceID shaderID = shader.IsValid() ? shader->ID : Resource::InvalidID;

		if (shader.IsValid())
			AddShader(shader);

		Materials.try_emplace(material->ID, material->ID, material->GetVersion(), shaderID, material->GetBufferData());
		MaterialRenderData& materialData = Materials.at(material->ID);

		for (const auto& kvp : material->GetSubshaderBindings())
			materialData.SubshaderBindings.try_emplace(kvp.first, kvp.second);

		for (const auto& kvp : material->GetTextureProperties())
		{
			Ref<Texture> texture = kvp.second;

			if (texture.IsValid())
			{
				AddTexture(texture);
				materialData.Samplers.try_emplace(kvp.first, texture->ID);
			}
			else
			{
				materialData.Samplers.try_emplace(kvp.first, Resource::InvalidID);
			}
		}
	}

	void RenderView::AddMesh(Ref<Mesh> mesh)
	{
		if (Meshs.contains(mesh->ID))
			return;

		Meshs.try_emplace(mesh->ID, mesh->ID, mesh->GetVersion(), mesh->GetVertexBuffer(), mesh->GetVertexCount(), mesh->GetIndexBuffer(), mesh->GetIndexCount());
	}
}