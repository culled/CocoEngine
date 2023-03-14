#pragma once

#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Core/Types/Quaternion.h>

using namespace Coco;
using namespace Coco::Rendering;

struct ShaderUniformObject
{
	float BaseColor[4];
	uint8_t Padding[128];

	ShaderUniformObject();
};

class HelloTriangleRenderPass : public IRenderPass 
{
private:
	Ref<Shader> _shader;
	Ref<Mesh> _mesh;
	Ref<Material> _material;
	ShaderUniformObject _shaderUO;
	Matrix4x4 _meshTransform;

public:
	HelloTriangleRenderPass();

	virtual string GetName() const override { return "main"; }
	virtual List<AttachmentDescription> GetAttachmentDescriptions() override;
	virtual void Execute(RenderContext* renderContext) override;
};