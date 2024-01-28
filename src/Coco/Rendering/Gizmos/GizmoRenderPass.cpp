#include "Renderpch.h"
#include "GizmoRenderPass.h"
#include "Gizmos.h"
#include "../Mesh.h"
#include "../Shader.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const string GizmoRenderPass::_name = "Gizmo";
	std::array<RenderPassAttachment, 2> GizmoRenderPass::_attachments = {
		RenderPassAttachment(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentUsageFlags::Color),
		RenderPassAttachment(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentUsageFlags::Depth)
	};

	SharedRef<Mesh> GizmoRenderPass::_gizmoMesh;
	SharedRef<Shader> GizmoRenderPass::_gizmoShader;

	GizmoRenderPass::GizmoRenderPass(bool clearGizmoDrawCalls) :
		_clearDrawCalls(clearGizmoDrawCalls)
	{}

	void GizmoRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<GizmoDrawCall> drawCalls = Gizmos::_drawCalls;

		if (drawCalls.size() == 0)
			return;

		EnsureGizmoResources();
		context.SetShader(_gizmoShader);

		for (const auto& drawCall : drawCalls)
		{
			const Submesh* submesh = nullptr;

			if (!_gizmoMesh->TryGetSubmesh(drawCall.SubmeshID, submesh))
				continue;

			std::array<ShaderUniformValue, 2> drawUniforms = {
				ShaderUniformValue("Color", drawCall.Color),
				ShaderUniformValue("Model", drawCall.Transform)
			};

			context.SetDrawUniforms(drawUniforms);

			context.Draw(_gizmoMesh, *submesh);
		}

		if(_clearDrawCalls)
			Gizmos::_drawCalls.clear();
	}

	void GizmoRenderPass::EnsureGizmoResources()
	{
		if (!_gizmoMesh)
			CreateGizmoMesh();

		if (!_gizmoShader)
			CreateGizmoShader();
	}

	void GizmoRenderPass::CreateGizmoMesh()
	{
		std::vector<VertexData> verts;
		std::vector<uint32> indices;
		std::unordered_map<uint32, Submesh> submeshes;

		// Create the box mesh

		//   2-------3
		//  /|      /|
		// 6-------7 |
		// | |     | |
		// | 0-----|-1
		// |/      |/
		// 4-------5
		verts.emplace_back(Vector3(-0.5, -0.5, -0.5));
		verts.emplace_back(Vector3(0.5, -0.5, -0.5));
		verts.emplace_back(Vector3(-0.5, 0.5, -0.5));
		verts.emplace_back(Vector3(0.5, 0.5, -0.5));

		verts.emplace_back(Vector3(-0.5, -0.5, 0.5));
		verts.emplace_back(Vector3(0.5, -0.5, 0.5));
		verts.emplace_back(Vector3(-0.5, 0.5, 0.5));
		verts.emplace_back(Vector3(0.5, 0.5, 0.5));

		indices.push_back(0);
		indices.push_back(1);

		indices.push_back(0);
		indices.push_back(2);

		indices.push_back(0);
		indices.push_back(4);

		indices.push_back(3);
		indices.push_back(1);

		indices.push_back(3);
		indices.push_back(2);

		indices.push_back(3);
		indices.push_back(7);

		indices.push_back(5);
		indices.push_back(4);

		indices.push_back(5);
		indices.push_back(7);

		indices.push_back(5);
		indices.push_back(1);

		indices.push_back(6);
		indices.push_back(4);

		indices.push_back(6);
		indices.push_back(7);

		indices.push_back(6);
		indices.push_back(2);

		submeshes.try_emplace(Gizmos::_boxSubmeshID, Submesh(0, indices.size()));

		uint32 vertexOffset = static_cast<uint32>(verts.size());
		uint32 indexOffset = static_cast<uint32>(indices.size());

		// Create line mesh (line points forward)
		verts.emplace_back(Vector3::Zero);
		verts.emplace_back(Vector3::Forward);

		indices.push_back(vertexOffset);
		indices.push_back(vertexOffset + 1);

		submeshes.try_emplace(Gizmos::_lineSubmeshID, Submesh(indexOffset, 2));

		vertexOffset = static_cast<uint32>(verts.size());
		indexOffset = static_cast<uint32>(indices.size());

		// Create sphere mesh
		const double radius = 1.0;
		const uint32 resolution = 32;

		// ZY circle
		for (uint32 x = 0; x < resolution; x++)
		{
			double r = (static_cast<double>(x) / resolution) * Math::PI * 2.0;
			verts.emplace_back(Vector3(0.0, Math::Sin(r), Math::Cos(r)) * radius);
		}

		// XY circle
		for (uint32 z = 0; z < resolution; z++)
		{
			double r = (static_cast<double>(z) / resolution) * Math::PI * 2.0;
			verts.emplace_back(Vector3(Math::Cos(r), Math::Sin(r), 0.0) * radius);
		}

		// XZ circle
		for (uint32 y = 0; y < resolution; y++)
		{
			double r = (static_cast<double>(y) / resolution) * Math::PI * 2.0;
			verts.emplace_back(Vector3(Math::Cos(r), 0.0, Math::Sin(r)) * radius);
		}

		for (uint32 i = 0; i < resolution; i++)
		{
			uint32 i0 = vertexOffset + i;
			uint32 i1 = vertexOffset + ((i + 1) % resolution);

			indices.push_back(i0);
			indices.push_back(i1);

			indices.push_back(i0 + resolution);
			indices.push_back(i1 + resolution);

			indices.push_back(i0 + resolution * 2);
			indices.push_back(i1 + resolution * 2);
		}

		submeshes.try_emplace(Gizmos::_sphereSubmeshID, Submesh(indexOffset, indices.size() - indexOffset));

		_gizmoMesh = Engine::Get()->GetResourceLibrary().Create<Mesh>(ResourceID("Gizmos::Mesh"), MeshUsageFlags::None);
		_gizmoMesh->SetVertices(VertexDataFormat(), verts);
		_gizmoMesh->SetIndices(indices, submeshes);
	}

	void GizmoRenderPass::CreateGizmoShader()
	{
		GraphicsPipelineState pipelineState;
		pipelineState.TopologyMode = TopologyMode::Lines;
		pipelineState.CullingMode = CullMode::None;
		pipelineState.EnableDepthWrite = false;

		_gizmoShader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			ResourceID("Gizmos::Shader"),
			"gizmo",
			std::initializer_list<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Gizmo.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Gizmo.frag.glsl")
				}),
			pipelineState,
			std::vector<AttachmentBlendState>({
				AttachmentBlendState::Opaque
				}),
			VertexDataFormat(),
			ShaderUniformLayout::Empty,
			ShaderUniformLayout::Empty,
			ShaderUniformLayout(
				{
					ShaderUniform("Model", ShaderStageFlags::Vertex, Matrix4x4::Identity),
					ShaderUniform("Color", ShaderStageFlags::Vertex, Color::White)
				}
			)
		);
	}
}