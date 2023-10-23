#include "SceneDataProvider3D.h"
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Rendering/Gizmos/GizmoRender.h>
#include <Coco/Rendering/Resources/BuiltInShaders.h>
#include <Coco/Core/Engine.h>

SceneDataProvider3D::SceneDataProvider3D() :
	_boxTransform(Vector3(0.0, 4.0, 0.0), Quaternion::Identity, Vector3::One),
	_drawBounds(false)
{
	VertexDataFormat format{};
	format.HasNormals = true;
	format.HasTangents = true;
	format.HasUV0 = true;

	std::vector<VertexData> vertices;
	std::vector<uint32> indices;
	MeshUtilities::CreateXYGrid(Vector2::One, Vector3(0.0, 0.0, -0.5), format, vertices, indices);
	MeshUtilities::CreateXZGrid(Vector2::One, Vector3(0.0, -0.5, 0.0), format, vertices, indices);
	MeshUtilities::CreateZYGrid(Vector2::One, Vector3(-0.5, 0.0, 0.0), format, vertices, indices);

	MeshUtilities::CreateBox(Vector3(1.0, 2.0, 3.0), Vector3(0.0, 0.0, 5.0), format, vertices, indices);

	MeshUtilities::CreateXYTriangleFan(1.0, 16, Vector3(-5.0, 0.0, -1.0), format, vertices, indices);
	MeshUtilities::CreateXZTriangleFan(1.0, 16, Vector3(-5.0, -1.0, 0.0), format, vertices, indices);
	MeshUtilities::CreateZYTriangleFan(1.0, 16, Vector3(-6.0, 0.0, 0.0), format, vertices, indices);

	MeshUtilities::CreateCone(1.0, 0.5, 16, Vector3(5.0, 0.0, 0.0), format, vertices, indices);

	MeshUtilities::CreateUVSphere(16, 8, 1.0, Vector3(5.0, 0.0, 3.0), format, vertices, indices);

	MeshUtilities::CalculateTangents(vertices, indices);

	ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

	_mesh = resourceLibrary.Create<Mesh>("Mesh");
	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);
	_mesh->Apply();

	vertices.clear();
	indices.clear();

	_boxMesh = resourceLibrary.Create<Mesh>("BoxMesh");

	MeshUtilities::CreateBox(Vector3::One, Vector3::Zero, format, vertices, indices);

	MeshUtilities::CalculateTangents(vertices, indices);

	_boxMesh->SetVertices(format, vertices);
	_boxMesh->SetIndices(indices, 0);
	_boxMesh->Apply();
	
	GraphicsPipelineState pipelineState{};

	const EngineFileSystem& fs = Engine::Get()->GetFileSystem();

	const char* shaderFilePath = "shaders/built-in/Lit.cshader";
	if (fs.FileExists(shaderFilePath))
	{
		_shader = resourceLibrary.GetOrLoad<Shader>(shaderFilePath);
	}
	else
	{
		_shader = resourceLibrary.Create<Shader>("Shader", "");
		_shader->AddVariant(BuiltInShaders::LitVariant);

		resourceLibrary.Save(shaderFilePath, _shader, true);
	}

	ImageSamplerDescription sampler = ImageSamplerDescription::LinearRepeat;
	sampler.LODBias = -1.0;
	sampler.MaxAnisotropy = 16;

	const char* colorTexFilePath = "textures/LargeBlocks.ctexture";
	if (fs.FileExists(colorTexFilePath))
	{
		_texture = resourceLibrary.GetOrLoad<Texture>(colorTexFilePath);
	}
	else
	{
		_texture = resourceLibrary.Create<Texture>("LargeBlocks", "assets/textures/LargeBlocks.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);
		resourceLibrary.Save(colorTexFilePath, _texture, true);
	}

	const char* normalTexFilePath = "textures/LargeBlocks_N.ctexture";
	if (fs.FileExists(normalTexFilePath))
	{
		_normalTexture = resourceLibrary.GetOrLoad<Texture>(normalTexFilePath);
	}
	else
	{
		_normalTexture = resourceLibrary.Create<Texture>("LargeBlocks_N", "assets/textures/LargeBlocks_N.png", ImageColorSpace::Linear, ImageUsageFlags::Sampled, sampler);
		resourceLibrary.Save(normalTexFilePath, _normalTexture, true);
	}

	const char* materialFilePath = "materials/LargeBlocks.cmaterial";
	if (fs.FileExists(materialFilePath))
	{
		_material = resourceLibrary.GetOrLoad<Material>(materialFilePath);
	}
	else
	{
		_material = resourceLibrary.Create<Material>("Material", _shader);
		_material->SetShader(_shader);
		_material->SetFloat4("AlbedoTintColor", Color::White);
		_material->SetTexture("AlbedoTexture", _texture);
		_material->SetTexture("NormalTexture", _normalTexture);

		resourceLibrary.Save(materialFilePath, _material, true);
	}
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

	renderView.AddRenderObject(*_mesh, 0, Matrix4x4::Identity, *_material);
	renderView.AddRenderObject(*_boxMesh, 0, _boxTransform.GlobalTransform, *_material);

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
