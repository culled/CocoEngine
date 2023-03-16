#pragma once

#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Core/Types/Quaternion.h>

using namespace Coco;
using namespace Coco::Rendering;

class HelloTriangleRenderPass final : public IRenderPass 
{
private:
	Ref<Mesh> _mesh;
	Ref<Material> _material;
	Matrix4x4 _meshTransform;

public:
	HelloTriangleRenderPass(Ref<Mesh> mesh, const Matrix4x4& meshTransform, Ref<Material> material);

	string GetName() const noexcept final { return "main"; }
	List<AttachmentDescription> GetAttachmentDescriptions() noexcept final;
	void Execute(RenderContext* renderContext) final;

	void UpdateMeshTransform(const Matrix4x4& meshTransform) { _meshTransform = meshTransform; }
};