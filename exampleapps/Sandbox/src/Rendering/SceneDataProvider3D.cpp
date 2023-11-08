#include "SceneDataProvider3D.h"
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Rendering/Gizmos/GizmoRender.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/RenderService.h>

SceneDataProvider3D::SceneDataProvider3D() :
	_boxTransform(Vector3(0.0, 4.0, 0.0), Quaternion::Identity, Vector3::One),
	_drawBounds(false)
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

	ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

	_mesh = resourceLibrary.Create<Mesh>("Mesh");
	VertexDataFormat format(VertexAttrFlags::Normal | VertexAttrFlags::Tangent | VertexAttrFlags::UV0);
	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);
	_mesh->Apply();

	vertices.clear();
	indices.clear();

	_boxMesh = resourceLibrary.Create<Mesh>("BoxMesh");

	MeshUtilities::CreateBox(Vector3::One, Vector3::Zero, vertices, indices);

	MeshUtilities::CalculateTangents(vertices, indices);

	_boxMesh->SetVertices(format, vertices);
	_boxMesh->SetIndices(indices, 0);
	_boxMesh->Apply();

	ImageSamplerDescription sampler = ImageSamplerDescription::LinearRepeat;
	sampler.LODBias = -1.0;
	sampler.MaxAnisotropy = 16;

	_texture = resourceLibrary.Create<Texture>("LargeBlocks", "textures/LargeBlocks.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);
	_normalTexture = resourceLibrary.Create<Texture>("LargeBlocks_N", "textures/LargeBlocks_N.png", ImageColorSpace::Linear, ImageUsageFlags::Sampled, sampler);

	_material = resourceLibrary.Create<Material>("Material");
	_material->SetValue("AlbedoTintColor", Color::White);
	_material->SetValue("AlbedoTexture", _texture);
	_material->SetValue("NormalTexture", _normalTexture);
}

void SceneDataProvider3D::SetDrawBounds(bool drawBounds)
{
	_drawBounds = drawBounds;
}

void SceneDataProvider3D::GatherSceneData(RenderView& renderView)
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
}
