#include "Renderpch.h"
#include "BuiltInPipeline.h"
#include "../Pipeline/RenderPasses/BuiltInRenderPass.h"
#include "../Gizmos/GizmoRenderPass.h"

namespace Coco::Rendering
{
	SharedRef<RenderPipeline> BuiltInPipeline::Create(bool useFrustumCulling, bool clearAttachments)
	{
		SharedRef<RenderPipeline> pipeline = CreateSharedRef<RenderPipeline>(0, "DefaultPipeline", clearAttachments ? AttachmentOptionFlags::Clear : AttachmentOptionFlags::None);

		std::array<uint8, 2> bindings = { 0, 1 };
		pipeline->AddRenderPass(CreateSharedRef<BuiltInRenderPass>(useFrustumCulling), bindings);
		pipeline->AddRenderPass(CreateSharedRef<GizmoRenderPass>(), bindings);

		return pipeline;
	}
}