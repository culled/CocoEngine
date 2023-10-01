#include "Renderpch.h"
#include "DebugRender.h"

#include "../Shader.h"
#include "../Mesh.h"
#include "../Material.h"
#include "../MeshUtilities.h"
#include "DebugRenderPass.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	DebugDrawCall::DebugDrawCall(uint64 firstIndexOffset, uint64 indexCount, const Coco::Color& color, const Matrix4x4& transform) :
		FirstIndexOffset(firstIndexOffset),
		IndexCount(indexCount),
		Color(color),
		Transform(transform)
	{}

	const VertexDataFormat DebugRender::_sVertexFormat = VertexDataFormat();

	DebugRender::DebugRender() :
		_drawCalls()
	{
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

		_shader = resources.Create<Shader>("DebugShader", DebugRenderPass::sPassName);

		GraphicsPipelineState pipelineState;
		pipelineState.TopologyMode = TopologyMode::Lines;
		pipelineState.CullingMode = CullMode::None;
		pipelineState.EnableDepthWrite = false;

		_shader->AddRenderPassShader(
			RenderPassShader(
				_shader->GetID(),
				DebugRenderPass::sPassName,
				{
					ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Debug.vert.spv"),
					ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Debug.frag.spv")
				},
				pipelineState,
				{
					BlendState::Opaque
				},
				{
					ShaderVertexAttribute("Position", BufferDataType::Float3)
				},
				GlobalShaderUniformLayout(),
				ShaderUniformLayout(),
				ShaderUniformLayout(
					{
						ShaderDataUniform("Model", ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
						ShaderDataUniform("Color", ShaderStageFlags::Vertex, BufferDataType::Float4)
					},
					{}
				)
			)
		);

		_material = resources.Create<Material>("Debug", _shader);

		_mesh = resources.Create<Mesh>("DebugMesh", false);

		SetupMesh();
	}

	DebugRender::~DebugRender()
	{
		_mesh.Invalidate();
		_shader.Invalidate();
		_material.Invalidate();
	}

	void DebugRender::GatherSceneData(RenderView& renderView)
	{
		for (const DebugDrawCall& dc : _drawCalls)
		{
			renderView.AddRenderObject(
				*_mesh,
				dc.FirstIndexOffset,
				dc.IndexCount,
				dc.Transform,
				BoundingBox::Zero,
				_material.Get(),
				nullptr,
				dc.Color);
		}

		_drawCalls.clear();
	}

	void DebugRender::DrawLine3D(const Vector3& start, const Vector3& end, const Color& color)
	{
		double dist = (start - end).GetLength();
		Matrix4x4 t = Matrix4x4::CreateTransform(start, Quaternion::FromToRotation(Vector3::Forward, end), Vector3::One * dist);
		_drawCalls.emplace_back(_lineIndexInfo.first, _lineIndexInfo.second, color, t);
	}

	void DebugRender::DrawRay3D(const Vector3& origin, const Vector3& direction, const Color& color)
	{
		DrawLine3D(origin, origin + direction, color);
	}

	void DebugRender::DrawWireBox(const Vector3& origin, const Quaternion& rotation, const Vector3& scale, const Color& color)
	{
		DrawWireBox(Matrix4x4::CreateTransform(origin, rotation, scale), color);
	}

	void DebugRender::DrawWireBox(const Matrix4x4& transform, const Color& color)
	{
		_drawCalls.emplace_back(_boxIndexInfo.first, _boxIndexInfo.second, color, transform);
	}

	void DebugRender::DrawWireBounds(const BoundingBox& box, const Matrix4x4& transform, const Color& color)
	{
		BoundingBox transformedBox = box.Transformed(transform);
		DrawWireBox(transformedBox.GetCenter(), Quaternion::Identity, transformedBox.GetSize(), color);
	}

	void DebugRender::DrawWireSphere(double radius, const Vector3& position, const Color& color)
	{
		_drawCalls.emplace_back(_sphereIndexInfo.first, _sphereIndexInfo.second, color, Matrix4x4::CreateTransform(position, Quaternion::Identity, Vector3::One * radius));
	}

	void DebugRender::SetupMesh()
	{
		std::vector<VertexData> verts;
		std::vector<uint32> indices;

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

		_boxIndexInfo = std::make_pair(0, indices.size());

		uint32 vertexOffset = static_cast<uint32>(verts.size());
		uint32 indexOffset = static_cast<uint32>(indices.size());

		// Create line mesh (line points forward)
		verts.emplace_back(Vector3::Zero);
		verts.emplace_back(Vector3::Forward);

		indices.push_back(vertexOffset);
		indices.push_back(vertexOffset + 1);

		_lineIndexInfo = std::make_pair(indexOffset, 2);

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

		_sphereIndexInfo = std::make_pair(indexOffset, indices.size() - indexOffset);

		_mesh->SetVertices(_sVertexFormat, verts);
		_mesh->SetIndices(indices, 0);
		_mesh->Apply();
	}
}