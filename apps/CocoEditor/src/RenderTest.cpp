#include "RenderTest.h"

#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Core/Engine.h>

namespace Coco
{
    RenderTest::RenderTest()
    {
        VertexDataFormat format{};
		format.HasUV0 = true;

        std::vector<VertexData> vertices;
        std::vector<uint32> indices;
		MeshUtilities::CreateXYGrid(Vector2::One, Vector3(0.0, 0.0, -1.0), format, vertices, indices);

        ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

        _mesh = resourceLibrary.Create<Mesh>("Mesh");
        _mesh->SetVertices(format, vertices);
        _mesh->SetIndices(indices, 0);

		GraphicsPipelineState pipelineState{};

		_shader = resourceLibrary.Create<Shader>("Shader2D", "2d");
		_shader->AddRenderPassShader(
			RenderPassShader(
				_shader->GetID(),
				"test",
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
				GlobalShaderUniformLayout(),
				ShaderUniformLayout(
					{
						ShaderDataUniform("BaseColor", ShaderStageFlags::Fragment, BufferDataType::Float4)
					},
					{
						ShaderTextureUniform("BaseTexSampler", ShaderStageFlags::Fragment, ShaderTextureUniform::DefaultTextureType::White)
					}
				),
				ShaderUniformLayout(
					{
						ShaderDataUniform("ModelMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
					},
					{}
				)
			)
		);

		_material = resourceLibrary.Create<Material>("Material2D", _shader);
		_material->SetShader(_shader);
		_material->SetFloat4("BaseColor", Color::Red);
    }

    void RenderTest::GatherSceneData(RenderView& renderView)
    {
		if (!_mesh->Apply())
			return;

		renderView.AddRenderObject(*_mesh, 0, Matrix4x4::Identity, _material.Get());
	}
}