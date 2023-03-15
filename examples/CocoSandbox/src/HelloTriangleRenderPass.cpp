#include "HelloTriangleRenderPass.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include <Coco/Core/Engine.h>

using namespace Coco;
using namespace Coco::Rendering;

ShaderUniformObject::ShaderUniformObject() : Padding{ 0 }
{
	BaseColor[0] = 0.0f;
	BaseColor[1] = 0.0f;
	BaseColor[2] = 0.0f;
	BaseColor[3] = 1.0f;
}

HelloTriangleRenderPass::HelloTriangleRenderPass()
{
	// Setup our basic shader
	GraphicsPipelineState pipelineState;
	//pipelineState.CullingMode = CullMode::None;
	_shader = CreateRef<Shader>("HelloTriangle");
	_shader->CreateSubshader("main",
		{
			{ShaderStageType::Vertex, "assets/shaders/built-in/ObjectShader.vert.spv"},
			{ShaderStageType::Fragment, "assets/shaders/built-in/ObjectShader.frag.spv"}
		},
		pipelineState,
		{
			ShaderVertexAttribute(BufferDataFormat::Vector3),
			ShaderVertexAttribute(BufferDataFormat::Vector2)
		},
		{
			ShaderDescriptor("_MaterialInfo", 0, ShaderDescriptorType::UniformStruct, sizeof(ShaderUniformObject)),
			ShaderDescriptor("_MainTex", 1, ShaderDescriptorType::UniformSampler)
		});

	_material = CreateRef<Material>(_shader);
	_material->SetStruct("_MaterialInfo", _shaderUO);

	// Setup our basic mesh
	double size = 10.0;
	_mesh = CreateRef<Mesh>();

	_mesh->SetPositions({ 
		Vector3(-0.5, 0.5, 0.0) * size, Vector3(-0.5, 1.5, 0.0) * size, Vector3(0.5, 1.5, 0.0) * size, Vector3(0.5, 0.5, 0.0) * size, // Forward (+Y)
		Vector3(0.5, 0.5, 0.0) * size, Vector3(1.5, 0.5, 0.0)* size, Vector3(1.5, -0.5, 0.0) * size, Vector3(0.5, -0.5, 0.0) * size, // Right (+X)
		Vector3(-0.5, 1.5, 0.0)* size, Vector3(-0.5, 1.5, 1.0)* size, Vector3(0.5, 1.5, 1.0)* size, Vector3(0.5, 1.5, 0.0)* size }); // Up (+Z)

	_mesh->SetUVs({
		Vector2(0.0, 0.0), Vector2(0.0, 1.0), Vector2(1.0, 1.0), Vector2(1.0, 0.0),
		Vector2(0.0, 0.0), Vector2(0.0, 1.0), Vector2(1.0, 1.0), Vector2(1.0, 0.0),
		Vector2(0.0, 0.0), Vector2(0.0, 1.0), Vector2(1.0, 1.0), Vector2(1.0, 0.0),
		});

	_mesh->SetIndices({ 
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8});

	_meshTransform = Matrix4x4::CreateWithTranslation(Vector3(0.0, 0.0, -10.0));
}

List<AttachmentDescription> HelloTriangleRenderPass::GetAttachmentDescriptions() noexcept
{
    List<AttachmentDescription> attachments;
    attachments.Add(AttachmentDescription(PixelFormat::BGRA8, ColorSpace::sRGB, false));
    return attachments;
}

void HelloTriangleRenderPass::Execute(RenderContext* renderContext)
{
	// Update mesh data on the GPU if it is dirty
	if(_mesh->GetIsDirty())
		_mesh->UploadData();

	// Draw the mesh
	renderContext->UseMaterial(_material);
	renderContext->Draw(_mesh.get(), _meshTransform);

	const double t = Coco::Engine::Get()->GetMainLoop()->GetRunningTime();
	const double a = Math::Sin(t) * 0.5 + 0.5;
	_shaderUO.BaseColor[0] = static_cast<float>(a);
	_shaderUO.BaseColor[1] = static_cast<float>(a);
	_shaderUO.BaseColor[2] = static_cast<float>(a);
	_material->SetStruct("_MaterialInfo", _shaderUO);

	//_meshRotation *= Quaternion(Vector3::Up, 0.01);
}
