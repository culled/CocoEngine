#include "SceneProvider3D.h"

#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Core/Engine.h>

using namespace Coco::Rendering;

SceneProvider3D::SceneProvider3D()
{
	CreateResources();
}

void SceneProvider3D::GatherSceneData(RenderView& renderView)
{
	//auto verts = _mesh->GetVertices();
	//
	//verts[0].Position.Y = Math::Sin(MainLoop::Get()->GetCurrentTick().Time) * 0.5 - 0.5;
	//_mesh->MarkDirty();
	
	const TickInfo& tick = MainLoop::Get()->GetCurrentTick();
	double t = Math::Sin(tick.Time) * 0.5 + 0.5;
	renderView.SetGlobalUniformValues(std::initializer_list<ShaderUniformValue>{ShaderUniformValue("TestGlobal", static_cast<float>(t))});
	
	t = Math::Cos(tick.Time * 3.0) * 0.5 + 0.5;
	_material->SetValue("TestShaderInstance", static_cast<float>(t));

	if (_mesh)
	{
		const Submesh* sm;
		_mesh->TryGetSubmesh(0, sm);
		renderView.AddRenderObject(0, 1, _mesh, *sm, _material, Matrix4x4::Identity);

		const Submesh* sm2;
		_mesh->TryGetSubmesh(1, sm2);
		renderView.AddRenderObject(1, 1, _mesh, *sm2, _material, Matrix4x4::Identity);
	}

	//if (_mesh && tick.Time > 5.0)
	//{
	//	Engine::Get()->GetResourceLibrary().Remove(_mesh->GetID());
	//	_mesh.reset();
	//}
}

void SceneProvider3D::CreateResources()
{
	ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

	//_mesh = resources.Create<Mesh>(ResourceID("SceneProvider3D::Mesh"));
	//
	//std::vector<VertexData> vertices{
	//	VertexData(Vector3(-0.5, -0.5, 0.0)),
	//	VertexData(Vector3(0.5, -0.5, 0.0)),
	//	VertexData(Vector3(0.5, 0.5, 0.0)),
	//	VertexData(Vector3(-0.5, 0.5, 0.0))
	//};
	//
	//std::unordered_map<uint32, std::vector<uint32>> indices{
	//	{0, {0, 1, 2}},
	//	{1, {2, 3, 0}}
	//};
	//
	//_mesh->SetVertices(VertexDataFormat(), vertices);
	//_mesh->SetIndices(indices);
	//
	//Engine::Get()->GetAssetManager().Save(_mesh, "meshes/test.cres");

	_mesh = Engine::Get()->GetAssetManager().GetOrLoadAs<Mesh>("meshes/test.cres");

	//_texture = resources.Create<Texture>(
	//	ResourceID("SceneProvider3D::Texture"),
	//	"textures/LargeBlocks.png", ImageColorSpace::sRGB,
	//	ImageUsageFlags::Sampled,
	//	ImageSamplerDescription::LinearRepeat);
	
	//Engine::Get()->GetAssetManager().Save(_texture, "textures/test.cres");

	_texture = Engine::Get()->GetAssetManager().GetOrLoadAs<Texture>("textures/test.cres");

	//_material = resources.Create<Material>(ResourceID("SceneProvider3D::Material"));
	//_material->SetValue("TestTexture", _texture);
	//Engine::Get()->GetAssetManager().Save(_material, "materials/test.cres");

	_material = Engine::Get()->GetAssetManager().GetOrLoadAs<Material>("materials/test.cres");
}
