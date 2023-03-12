#include "HelloTriangleRenderPass.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>

using namespace Coco;
using namespace Coco::Rendering;

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
			ShaderVertexAttribute(BufferDataFormat::Vector3)
		});

	// Setup our basic mesh
	double size = 10.0;
	_mesh = CreateRef<Mesh>();

	_mesh->SetPositions({ 
		Vector3(-0.1, 0.0, 0.0) * size, Vector3(-0.1, 1.0, 0.0) * size, Vector3(0.1, 1.0, 0.0) * size, Vector3(0.1, 0.0, 0.0) * size, // Forward (+Y)
		Vector3(0.0, 0.1, 0.0) * size, Vector3(1.0, 0.1, 0.0)* size, Vector3(1.0, -0.1, 0.0) * size, Vector3(0.0, -0.1, 0.0) * size, // Right (+X)
		Vector3(-0.1, 1.0, 0.0)* size, Vector3(-0.1, 1.0, 1.0)* size, Vector3(0.1, 1.0, 1.0)* size, Vector3(0.1, 1.0, 0.0)* size }); // Up (+Z)

	_mesh->SetIndices({ 
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8});

	_meshTransform = Matrix4x4::CreateWithTranslation(Vector3(0.0, 0.0, -10.0));
}

List<AttachmentDescription> HelloTriangleRenderPass::GetAttachmentDescriptions()
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
	renderContext->UseShader(_shader);
	renderContext->Draw(_mesh, _meshTransform);

	//_meshRotation *= Quaternion(Vector3::Up, 0.01);
}
