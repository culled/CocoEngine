#include "BasicSceneDataProvider.h"
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Core/MainLoop/MainLoop.h>

BasicSceneDataProvider::BasicSceneDataProvider() :
	_transform(),
	_mesh(CreateManagedRef<Mesh>()),
	_shader(CreateManagedRef<Shader>("")),
	_texture(CreateManagedRef<Texture>("assets/textures/UV_Gradient.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, ImageSamplerDescription())),
	_material(CreateManagedRef<Material>())
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

	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);
	
	GraphicsPipelineState pipelineState{};
	//pipelineState.CullingMode = CullMode::None;

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
				BlendState()
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

	_material->SetShader(_shader);
	_material->SetFloat4("baseColor", Color::White);
	_material->SetTexture("baseTexSampler", _texture);
}

void BasicSceneDataProvider::GatherSceneData(RenderView& renderView)
{
	if (!_mesh->Apply())
		return;

	//_transform.RotateGlobal(Vector3::Up, MainLoop::cGet()->GetCurrentTick().DeltaTime);
	//_transform.Recalculate();

	renderView.AddRenderObject(*_mesh, 0, *_material, _transform.GlobalTransform);
}
