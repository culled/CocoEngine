#include "HelloTriangleRenderPass.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>

using namespace Coco;
using namespace Coco::Rendering;

HelloTriangleRenderPass::HelloTriangleRenderPass()
{
	// Setup our basic shader
	GraphicsPipelineState pipelineState;
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
	_mesh = CreateRef<Mesh>();
	_mesh->SetPositions({ Vector3(0.0, -0.5, 0.0), Vector3(0.5, 0.5, 0.0), Vector3(0.0, 0.5, 0.0), Vector3(0.5, -0.5, 0.0) });
	_mesh->SetIndices({ 0, 1, 2, 0, 3, 1 });
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
	renderContext->Draw(_mesh);
}
