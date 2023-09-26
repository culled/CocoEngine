#include "SceneDataProvider3D.h"
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Core/Engine.h>

SceneDataProvider3D::SceneDataProvider3D() :
	_transform()
{
	VertexDataFormat format{};
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

	ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

	_mesh = resourceLibrary.Create<Mesh>("Mesh");
	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);
	
	GraphicsPipelineState pipelineState{};

	_shader = resourceLibrary.Create<Shader>("Shader", "");
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
				BlendState::Opaque
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

	ImageSamplerDescription sampler = ImageSamplerDescription::LinearRepeat;
	sampler.LODBias = -1.0;
	sampler.MaxAnisotropy = 16;
	_texture = resourceLibrary.Create<Texture>("Texture", "assets/textures/LargeBlocks.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);

	_material = resourceLibrary.Create<Material>("Material", _shader);
	_material->SetShader(_shader);
	_material->SetFloat4("baseColor", Color::White);
	_material->SetTexture("baseTexSampler", _texture);
}

void SceneDataProvider3D::GatherSceneData(RenderView& renderView)
{
	if (!_mesh->Apply())
		return;

	//_transform.RotateGlobal(Vector3::Up, MainLoop::cGet()->GetCurrentTick().DeltaTime);
	//_transform.Recalculate();

	renderView.AddRenderObject(*_mesh, 0, _transform.GlobalTransform, _material.Get());
}
