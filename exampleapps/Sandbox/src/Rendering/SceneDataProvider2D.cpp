#include "SceneDataProvider2D.h"

#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Core/Engine.h>

SceneDataProvider2D::SceneDataProvider2D() :
	_transform(Vector2(50, 50), 0.0, Vector2::One)
{
	VertexDataFormat format{};
	format.HasUV0 = true;

	std::vector<VertexData> vertices;
	std::vector<uint32> indices;
	MeshUtilities::CreateXYGrid(Vector2::One * 50.0, Vector3::Zero, format, vertices, indices);

	ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

	_mesh = resourceLibrary.Create<Mesh>("Mesh");
	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);

	GraphicsPipelineState pipelineState{};

	_shader = resourceLibrary.Create<Shader>("Shader2D", "2d");
	_shader->AddRenderPassShader(
		RenderPassShader(
			0,
			"basic",
			{
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Unlit.vert.spv"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Unlit.frag.spv")
			},
			pipelineState,
			{
				BlendState::AlphaBlending
			},
			{
				ShaderVertexAttribute("Position", BufferDataType::Float3),
				ShaderVertexAttribute("UV", BufferDataType::Float2)
			},
			{
				ShaderDataUniform("projectionMatrix", UniformScope::Global, ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
				ShaderDataUniform("viewMatrix", UniformScope::Global, ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
				ShaderDataUniform("baseColor", UniformScope::Instance, ShaderStageFlags::Fragment, BufferDataType::Float4),
				ShaderDataUniform("modelMatrix", UniformScope::Draw, ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
			},
			{
				ShaderTextureUniform("baseTexSampler", UniformScope::Instance, ShaderStageFlags::Fragment)
			}
		)
	);

	ImageSamplerDescription sampler{};
	_texture = resourceLibrary.Create<Texture>("TextureUI", "assets/textures/UI Test 2.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);

	_material = resourceLibrary.Create<Material>("Material2D", _shader);
	_material->SetShader(_shader);
	_material->SetFloat4("baseColor", Color::White);
	_material->SetTexture("baseTexSampler", _texture);
}

SceneDataProvider2D::~SceneDataProvider2D()
{
	_mesh.Invalidate();
	_material.Invalidate();
	_texture.Invalidate();
	_shader.Invalidate();
}

void SceneDataProvider2D::GatherSceneData(RenderView& renderView)
{
	if (!_mesh->Apply())
		return;

	_transform.RotateGlobal(MainLoop::cGet()->GetCurrentTick().DeltaTime);
	_transform.Recalculate();

	renderView.AddRenderObject(*_mesh, 0, _transform.GlobalTransform, _material.Get());
}
