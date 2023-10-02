#include "SceneDataProvider3D.h"
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/Rendering/RenderDebug/DebugRender.h>
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
		Ref<Resource> tempShader;
		Assert(resourceLibrary.GetOrLoad(shaderFilePath, tempShader))

		_shader = static_cast<Ref<Shader>>(tempShader);
	}
	else
	{
		_shader = resourceLibrary.Create<Shader>("Shader", "");
		_shader->AddRenderPassShader(
			RenderPassShader(
				_shader->GetID(),
				"basic",
				{
					ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Lit.vert.spv"),
					ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Lit.frag.spv")
				},
				pipelineState,
				{
					BlendState::Opaque
				},
			{
				ShaderVertexAttribute("Position", BufferDataType::Float3),
				ShaderVertexAttribute("Normal", BufferDataType::Float3),
				ShaderVertexAttribute("Tangent", BufferDataType::Float4),
				ShaderVertexAttribute("UV", BufferDataType::Float2)
			},
				GlobalShaderUniformLayout(),
				ShaderUniformLayout(
					{
						ShaderDataUniform("BaseColor", ShaderStageFlags::Fragment, BufferDataType::Float4)
					},
				{
					ShaderTextureUniform("BaseTexSampler", ShaderStageFlags::Fragment, ShaderTextureUniform::DefaultTextureType::White),
					ShaderTextureUniform("NormalTexSampler", ShaderStageFlags::Fragment, ShaderTextureUniform::DefaultTextureType::Normal)
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

		resourceLibrary.Save(shaderFilePath, _shader, true);
	}

	ImageSamplerDescription sampler = ImageSamplerDescription::LinearRepeat;
	sampler.LODBias = -1.0;
	sampler.MaxAnisotropy = 16;

	const char* colorTexFilePath = "textures/LargeBlocks.ctexture";
	if (fs.FileExists(colorTexFilePath))
	{
		Ref<Resource> tempTexture;
		Assert(resourceLibrary.GetOrLoad(colorTexFilePath, tempTexture))

		_texture = static_cast<Ref<Texture>>(tempTexture);
	}
	else
	{
		_texture = resourceLibrary.Create<Texture>("LargeBlocks", "assets/textures/LargeBlocks.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, sampler);
		resourceLibrary.Save(colorTexFilePath, _texture, true);
	}

	const char* normalTexFilePath = "textures/LargeBlocks_N.ctexture";
	if (fs.FileExists(normalTexFilePath))
	{
		Ref<Resource> tempTexture;
		Assert(resourceLibrary.GetOrLoad(normalTexFilePath, tempTexture))

		_normalTexture = static_cast<Ref<Texture>>(tempTexture);
	}
	else
	{
		_normalTexture = resourceLibrary.Create<Texture>("LargeBlocks_N", "assets/textures/LargeBlocks_N.png", ImageColorSpace::Linear, ImageUsageFlags::Sampled, sampler);
		resourceLibrary.Save(normalTexFilePath, _normalTexture, true);
	}

	_material = resourceLibrary.Create<Material>("Material", _shader);
	_material->SetShader(_shader);
	_material->SetFloat4("BaseColor", Color::White);
	_material->SetTexture("BaseTexSampler", _texture);
	_material->SetTexture("NormalTexSampler", _normalTexture);
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

	renderView.AddRenderObject(*_mesh, 0, Matrix4x4::Identity, _material.Get());
	renderView.AddRenderObject(*_boxMesh, 0, _boxTransform.GlobalTransform, _material.Get());

	DebugRender* debug = DebugRender::Get();

	if (debug && _drawBounds)
	{
		debug->DrawWireBounds(_boxMesh->GetBounds(), _boxTransform.GlobalTransform, Color::Yellow);
		debug->DrawWireBounds(_mesh->GetBounds(), Matrix4x4::Identity, Color::Yellow);

		debug->DrawRay3D(Vector3::Zero, Vector3::Backward, Color::Blue);
		debug->DrawRay3D(Vector3::Zero, Vector3::Up, Color::Green);
		debug->DrawRay3D(Vector3::Zero, Vector3::Right, Color::Red);

		debug->DrawWireSphere(0.5, Vector3(2.0, 0.0, 0.0), Color::Cyan);
	}

	renderView.AddDirectionalLight(Vector3(-0.2, -0.5, -0.3).Normalized(), Color::White, 1.0);
	//renderView.AddDirectionalLight(Vector3(0.2, -0.5, 0.3).Normalized(), Color::Green, 1.0);

	renderView.AddPointLight(Vector3(1.0, 3.0, 0.0), Color::Red, 3.0);
	renderView.AddPointLight(Vector3(0.0, 2.0, 4.0), Color::Green, 3.0);
}
