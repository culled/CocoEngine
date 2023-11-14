#include "SceneDataProvider3D.h"
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Rendering/Gizmos/GizmoRender.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Material.h>

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/ECS/Components/NativeScriptComponent.h>
#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>
#include "../Scripts/MovingBox.h"
#include <unordered_map>

using namespace Coco::ECS;

/*void SceneDataProvider3D::GatherSceneData(RenderView& renderView)
{
	double t = MainLoop::cGet()->GetCurrentTick().Time;
	_boxTransform.LocalRotation = Quaternion(Vector3(Math::Sin(t), Math::Cos(t * 0.8 + 20.0), Math::Sin(t * 1.2 - 30.0)));
	_boxTransform.Recalculate();

	renderView.AddRenderObject(0, *_mesh, 0, Matrix4x4::Identity, 1, _material.get());
	renderView.AddRenderObject(0, *_boxMesh, 0, _boxTransform.GlobalTransform, 1, _material.get());

	GizmoRender* gizmo = GizmoRender::Get();

	if (gizmo && _drawBounds)
	{
		gizmo->DrawWireBounds(_boxMesh->GetBounds(), _boxTransform.GlobalTransform, Color::Yellow);
		gizmo->DrawWireBounds(_mesh->GetBounds(), Matrix4x4::Identity, Color::Yellow);

		gizmo->DrawRay3D(Vector3::Zero, Vector3::Backward, Color::Blue);
		gizmo->DrawRay3D(Vector3::Zero, Vector3::Up, Color::Green);
		gizmo->DrawRay3D(Vector3::Zero, Vector3::Right, Color::Red);

		gizmo->DrawWireSphere(0.5, Vector3(2.0, 0.0, 0.0), Color::Cyan);
	}

	renderView.AddDirectionalLight(Vector3(-0.2, -0.5, -0.3).Normalized(), Color::White, 1.0);
	//renderView.AddDirectionalLight(Vector3(0.2, -0.5, 0.3).Normalized(), Color::Green, 1.0);

	renderView.AddPointLight(Vector3(1.0, 3.0, 0.0), Color::Red, 3.0);
	renderView.AddPointLight(Vector3(0.0, 2.0, 4.0), Color::Green, 3.0);
}*/

void SceneDataProvider3D::SetupScene(SharedRef<ECS::Scene> scene)
{
	std::vector<VertexData> vertices;
	std::vector<uint32> indices;
	MeshUtilities::CreateXYGrid(Vector2::One, Vector3(0.0, 0.0, -0.5), vertices, indices);
	MeshUtilities::CreateXZGrid(Vector2::One, Vector3(0.0, -0.5, 0.0), vertices, indices);
	MeshUtilities::CreateZYGrid(Vector2::One, Vector3(-0.5, 0.0, 0.0), vertices, indices);

	MeshUtilities::CreateBox(Vector3(1.0, 2.0, 3.0), Vector3(0.0, 0.0, 5.0), vertices, indices);

	MeshUtilities::CreateXYTriangleFan(1.0, 16, Vector3(-5.0, 0.0, -1.0), vertices, indices);
	MeshUtilities::CreateXZTriangleFan(1.0, 16, Vector3(-5.0, -1.0, 0.0), vertices, indices);
	MeshUtilities::CreateZYTriangleFan(1.0, 16, Vector3(-6.0, 0.0, 0.0), vertices, indices);

	MeshUtilities::CreateCone(1.0, 0.5, 16, Vector3(5.0, 0.0, 0.0), vertices, indices);

	MeshUtilities::CreateUVSphere(16, 8, 1.0, Vector3(5.0, 0.0, 3.0), vertices, indices);

	MeshUtilities::CalculateTangents(vertices, indices);

	Entity objectsEntity = scene->CreateEntity("Objects");
	objectsEntity.AddComponent<Transform3DComponent>();

	ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

	SharedRef<Mesh> objectsMesh = resourceLibrary.Create<Mesh>("Objects Mesh");
	//VertexDataFormat format(VertexAttrFlags::Normal | VertexAttrFlags::Tangent | VertexAttrFlags::UV0);
	VertexDataFormat format(VertexAttrFlags::UV0);
	objectsMesh->SetVertices(format, vertices);
	objectsMesh->SetIndices(indices, 0);
	objectsMesh->Apply();

	ImageSamplerDescription sampler = ImageSamplerDescription::LinearRepeat;
	sampler.LODBias = -1.0;
	sampler.MaxAnisotropy = 16;

	SharedRef<Texture> texture = resourceLibrary.Create<Texture>("LargeBlocks", "textures/LargeBlocks.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);
	SharedRef<Texture> normalTexture = resourceLibrary.Create<Texture>("LargeBlocks_N", "textures/LargeBlocks_N.png", ImageColorSpace::Linear, ImageUsageFlags::Sampled, sampler);

	SharedRef<Material> material = resourceLibrary.Create<Material>("Material");
	//material->SetValue("AlbedoTintColor", Color::White);
	//material->SetValue("AlbedoTexture", texture);
	//material->SetValue("NormalTexture", normalTexture);
	material->SetValue("TintColor", Color::White);
	material->SetValue("ColorTexture", texture);

	std::unordered_map<uint32, SharedRef<MaterialDataProvider>> materialMap = { { 0, std::static_pointer_cast<MaterialDataProvider>(material) } };

	objectsEntity.AddComponent<MeshRendererComponent>(objectsMesh, materialMap);

	vertices.clear();
	indices.clear();

	MeshUtilities::CreateBox(Vector3::One, Vector3::Zero, vertices, indices);

	MeshUtilities::CalculateTangents(vertices, indices);

	SharedRef<Mesh> boxMesh = resourceLibrary.Create<Mesh>("Box Mesh");
	boxMesh->SetVertices(format, vertices);
	boxMesh->SetIndices(indices, 0);
	boxMesh->Apply();

	Entity boxEntity = scene->CreateEntity("Box");
	boxEntity.AddComponent<Transform3DComponent>(Vector3(0.0, 4.0, 0.0), Quaternion::Identity, Vector3::One);

	boxEntity.AddComponent<NativeScriptComponent>().Attach<MovingBox>();

	boxEntity.AddComponent<MeshRendererComponent>(boxMesh, materialMap);
}
