#include "BasicSceneDataProvider.h"
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Core/MainLoop/MainLoop.h>

BasicSceneDataProvider::BasicSceneDataProvider() :
	_position(Vector3::Zero),
	_rotation(Quaternion::Identity),
	_scale(Vector3::One),
	_mesh(CreateUniqueRef<Mesh>()),
	_shader(CreateUniqueRef<Shader>(""))
{
	// TODO: create mesh data
	VertexDataFormat format{};
	std::vector<VertexData> vertices;
	std::vector<uint32> indices;
	MeshUtilities::CreateXYGrid(Vector2::One, Vector3::Zero, format, vertices, indices);

	_mesh->SetVertices(format, vertices);
	_mesh->SetIndices(indices, 0);
	
	GraphicsPipelineState pipelineState{};
	pipelineState.CullingMode = CullMode::None;

	_shader->AddRenderPassShader(
		RenderPassShader(
			0,
			"basic",
			{
				ShaderStage("main", ShaderStageType::Vertex, "assets/shaders/built-in/Unlit.vert.spv"),
				ShaderStage("main", ShaderStageType::Fragment, "assets/shaders/built-in/Unlit.frag.spv")
			},
			pipelineState,
			{
				BlendState()
			},
			{
				ShaderVertexAttribute("Position", BufferDataType::Float3)
			},
			{
				ShaderDataUniform("projectionMatrix", UniformScope::Global, ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
				ShaderDataUniform("viewMatrix", UniformScope::Global, ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
				ShaderDataUniform("baseColor", UniformScope::Instance, ShaderStageFlags::Fragment, BufferDataType::Float4),
				ShaderDataUniform("modelMatrix", UniformScope::Draw, ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
			},
			{}
		)
	);
}

void BasicSceneDataProvider::GatherSceneData(RenderView& renderView)
{
	if (!_mesh->Apply())
		return;

	//_position.X = 1.0;
	//_position.Y = 1.0;
	//_position.X = Math::Cos(MainLoop::cGet()->GetCurrentTick().Time);
	//_position.Y = Math::Sin(MainLoop::cGet()->GetCurrentTick().Time);
	//_position.Z = Math::Sin(MainLoop::cGet()->GetCurrentTick().Time) * 0.5 + 0.5;
	//_rotation = Quaternion(Vector3::Forward, MainLoop::cGet()->GetCurrentTick().Time * 0.5);
	_rotation = Quaternion(Vector3::Up, MainLoop::cGet()->GetCurrentTick().Time * 0.5);
	//_rotation = Quaternion(Vector3::Right, MainLoop::cGet()->GetCurrentTick().Time * 0.5);
	//_scale.X = Math::Cos(MainLoop::cGet()->GetCurrentTick().Time) * 0.5 + 0.5;
	//_scale.Y = Math::Sin(MainLoop::cGet()->GetCurrentTick().Time) * 0.5 + 0.5;

	renderView.AddRenderObject(*_mesh, 0, *_shader, Matrix4x4::CreateTransform(_position, _rotation, _scale));
}
