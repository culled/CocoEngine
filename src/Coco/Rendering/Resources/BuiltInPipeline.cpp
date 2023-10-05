#include "Renderpch.h"
#include "BuiltInPipeline.h"
#include "../Pipeline/RenderPasses/BuiltInRenderPass.h"

namespace Coco::Rendering
{
	SharedRef<RenderPipeline> BuiltInPipeline::Create(bool useFrustumCulling)
	{
		SharedRef<RenderPipeline> pipeline = CreateSharedRef<RenderPipeline>();

		std::array<uint8, 2> bindings = { 0, 1 };
		pipeline->AddRenderPass(CreateSharedRef<BuiltInRenderPass>(useFrustumCulling), bindings);

		return pipeline;
	}
}