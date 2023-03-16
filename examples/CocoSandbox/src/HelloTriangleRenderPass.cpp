#include "HelloTriangleRenderPass.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include <Coco/Core/Engine.h>

using namespace Coco;
using namespace Coco::Rendering;


HelloTriangleRenderPass::HelloTriangleRenderPass(Ref<Mesh> mesh, const Matrix4x4& meshTransform, Ref<Material> material) :
	_mesh(mesh), _meshTransform(meshTransform), _material(material)
{}

List<AttachmentDescription> HelloTriangleRenderPass::GetAttachmentDescriptions() noexcept
{
    List<AttachmentDescription> attachments;
    attachments.Add(AttachmentDescription(PixelFormat::RGBA8, ColorSpace::sRGB, false));
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
}
