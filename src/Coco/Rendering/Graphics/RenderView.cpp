#include "RenderView.h"

#include "Resources/Buffer.h"
#include "Resources/Image.h"
#include "Resources/ImageSampler.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Mesh.h"
#include <Coco/Core/Engine.h>
#include "../RenderingService.h"

namespace Coco::Rendering
{
	RenderView::RenderView(
		const RectInt& viewportRect, 
		const Matrix4x4& projection,
		const Matrix4x4& projection2D,
		const Matrix4x4& view,
		const Vector3& viewPosition,
		const List<RenderTarget>& renderTargets
	) noexcept :
		ViewportRect(viewportRect),
		Projection(projection), 
		Projection2D(projection2D),
		View(view), 
		ViewPosition(viewPosition),
		RenderTargets(renderTargets),
		Shaders{},
		Textures{},
		Materials{},
		Meshs{},
		Objects{}
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

	void RenderView::AddRenderObject(Ref<Mesh> mesh, uint submeshIndex, Ref<IMaterial> material, const Matrix4x4& modelMatrix)
	{	
		if (!mesh.IsValid())
		{
			LogError(RenderingService::Get()->GetLogger(), "Mesh is not valid");
			return;
		}

		if (submeshIndex >= mesh->GetSubmeshCount())
		{
			LogError(RenderingService::Get()->GetLogger(), FormattedString(
				"Submesh index ({}) is higher than the number of submeshes ({})", 
				submeshIndex, mesh->GetSubmeshCount()
			));
			return;
		}

		if (!material.IsValid())
		{
			LogError(RenderingService::Get()->GetLogger(), "Material is not valid");
			return;
		}

		AddMesh(mesh);
		AddMaterial(material);

		Objects.Construct(mesh->ID, submeshIndex, material->GetID(), modelMatrix);
	}

	void RenderView::AddShader(Ref<Shader> shader)
	{
		if (Shaders.contains(shader->ID))
			return;

		Shaders.try_emplace(shader->ID, shader->ID, shader->GetVersion(), shader->GetGroupTag());
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

	void RenderView::AddMaterial(Ref<IMaterial> material)
	{
		const ResourceID& materialID = material->GetID();

		if (Materials.contains(materialID))
			return;

		Ref<Shader> shader = material->GetShader();
		ResourceID shaderID = shader.IsValid() ? shader->ID : Resource::InvalidID;

		if (shader.IsValid())
			AddShader(shader);

		const ShaderUniformData& uniformData = material->GetUniformData();
		Materials.try_emplace(materialID, materialID, material->GetMaterialVersion(), shaderID, uniformData);

		ResourceLibrary* library = Engine::Get()->GetResourceLibrary();

		for (const auto& texKVP : uniformData.Textures)
		{
			Ref<Texture> texture = library->GetResource<Texture>(texKVP.second);

			AddTexture(texture);
		}
	}

	void RenderView::AddMesh(Ref<Mesh> mesh)
	{
		if (Meshs.contains(mesh->ID))
			return;

		List<SubmeshData> submeshDatas;

		for (const auto& submesh : mesh->GetSubmeshes())
			submeshDatas.Construct(submesh.IndexBufferOffset, submesh.IndexCount);

		Meshs.try_emplace(mesh->ID, mesh->ID, mesh->GetVersion(), mesh->GetVertexBuffer(), mesh->GetVertexCount(), mesh->GetIndexBuffer(), submeshDatas);
	}
}