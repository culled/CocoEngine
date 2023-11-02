#include "SceneDataProvider2D.h"

#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Rendering/Resources/BuiltInShaders.h>
#include <Coco/Core/Engine.h>

SceneDataProvider2D::SceneDataProvider2D() :
	_transform(Vector2(50, 50), 0.0, Vector2::One)
{
	std::vector<VertexData> vertices;
	std::vector<uint32> indices;
	MeshUtilities::CreateXYGrid(Vector2::One * 50.0, Vector3::Zero, vertices, indices);

	ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

	_mesh = resourceLibrary.Create<Mesh>("Mesh");
	VertexDataFormat format(VertexAttrFlags::UV0);
	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);

	GraphicsPipelineState pipelineState{};

	_shader = resourceLibrary.Create<Shader>("Shader2D", "2d");
	ShaderVariant variant = BuiltInShaders::UnlitVariant;
	variant.AttachmentBlendStates = { BlendState::AlphaBlending };
	_shader->AddVariant(variant);

	ImageSamplerDescription sampler = ImageSamplerDescription::LinearClamp;
	_texture = resourceLibrary.Create<Texture>("TextureUI", "assets/textures/UI Test 2.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);

	_material = resourceLibrary.Create<Material>("Material2D", _shader);
	_material->SetShader(_shader);
	_material->SetFloat4("TintColor", Color::White);
	_material->SetTexture("ColorTexture", _texture);
}

SceneDataProvider2D::~SceneDataProvider2D()
{
	_mesh.reset();
	_material.reset();
	_texture.reset();
	_shader.reset();
}

void SceneDataProvider2D::GatherSceneData(RenderView& renderView)
{
	if (!_mesh->Apply())
		return;

	_transform.RotateGlobal(MainLoop::cGet()->GetCurrentTick().DeltaTime);
	_transform.Recalculate();

	renderView.AddRenderObject(0, *_mesh, 0, _transform.GlobalTransform, *_material);
}
