#include "ImGuiRenderViewProvider.h"

#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>

#include <imgui.h>

using namespace Coco::Rendering;

namespace Coco::ImGuiCoco
{
	const ShaderUniformLayout ImGuiRenderViewProvider::GlobalUniformLayout = ShaderUniformLayout(
		{
			ShaderUniform("ProjectionMatrix", ShaderUniformType::Matrix4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity)
		}
	);

	ImGuiRenderViewProvider::ImGuiRenderViewProvider(ImGuiViewport* viewport) :
		_viewport(viewport)
	{
		CocoAssert(_viewport, "Viewport was null")
	}
	
	void ImGuiRenderViewProvider::SetupRenderView(
		RenderView & renderView, 
		const CompiledRenderPipeline& pipeline, 
		uint64 rendererID, 
		const SizeInt& backbufferSize, 
		std::span<Ref<Image>> backbuffers)
	{
		std::vector<RenderTarget> rts;

		for (size_t i = 0; i < backbuffers.size(); i++)
			rts.emplace_back(backbuffers[i], Vector4::Zero);

		ImDrawData* drawData = _viewport->DrawData;

		RectInt viewport(Vector2Int::Zero, SizeInt(static_cast<uint32>(drawData->DisplaySize.x), static_cast<uint32>(drawData->DisplaySize.y)));

		RenderService* rendering = RenderService::Get();
		CocoAssert(rendering, "RenderService singleton was null")

		Matrix4x4 projection = rendering->GetPlatform().CreateOrthographicProjection(
			viewport.GetLeft(),
			viewport.GetRight(),
			viewport.GetTop(),
			viewport.GetBottom(),
			-1.0, 1.0);

		renderView.Setup(rts, viewport, Matrix4x4::Identity, projection, ViewFrustum(), MSAASamples::One, GlobalUniformLayout);

		std::array< ShaderUniformValue, 1> uniforms = { ShaderUniformValue("ProjectionMatrix", projection) };
		renderView.SetGlobalUniformValues(uniforms);
	}
}