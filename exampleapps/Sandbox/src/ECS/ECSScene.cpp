#include "ECSScene.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>
#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Rendering/BuiltIn/BuiltInShaderRenderPass.h>

#include <Coco/Core/Engine.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

ECSScene::ECSScene()
{
	ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

	_scene = resources.Create<Scene>(ResourceID("ECSScene::Scene"));
	_box = _scene->CreateEntity("Box");
	_box.AddComponent<Transform3DComponent>();
	
	std::vector<VertexData> vertices;
	std::vector<uint32> indices;
	MeshUtilities::CreateBox(Vector3::One * 0.5, Vector3::Zero, vertices, indices);

	SharedRef<Mesh> boxMesh = resources.Create<Mesh>(ResourceID("ECSScene::BoxMesh"));
	boxMesh->SetVertices(VertexDataFormat(VertexAttrFlags::UV0), vertices);
	boxMesh->SetIndices(indices);

	SharedRef<Texture> tex = resources.Create<Texture>(
			ResourceID("ECSScene::Texture"),
			"textures/LargeBlocks.png", ImageColorSpace::sRGB,
			ImageUsageFlags::Sampled,
			ImageSamplerDescription::LinearRepeat);

	SharedRef<Material> mat = resources.Create<Material>(ResourceID("ECSScene::BoxMaterial"));
	mat->SetValue("ColorTexture", tex);
	//mat->SetValue("TintColor", Color::Red);

	std::unordered_map<uint32, SharedRef<Material>> materials = { {0, mat} };
	_box.AddComponent<MeshRendererComponent>(boxMesh, materials, BuiltInShaderRenderPass::UnlitVisibilityGroup);
	
	_camera = _scene->CreateEntity("Camera");
	_camera.AddComponent<Transform3DComponent>(Vector3::Backward, Quaternion::Identity, Vector3::One);
	CameraComponent& camera = _camera.AddComponent<CameraComponent>();
	camera.SetClearColor(Color(0.1, 0.2, 0.3, 1.0));
	//camera.SetSampleCount(MSAASamples::Four);
	
	//Engine::Get()->GetAssetManager().Save(_scene, "scenes/test.cres");

	//_scene = Engine::Get()->GetAssetManager().GetOrLoadAs<Scene>("scenes/test.cres");
}

void ECSScene::Update(const TickInfo& tickInfo)
{
	_box.GetComponent<Transform3DComponent>().Rotate(Vector3::Up, tickInfo.DeltaTime, TransformSpace::Global);
}
