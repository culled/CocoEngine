#include "Renderpch.h"
#include "RenderPassBinding.h"

namespace Coco::Rendering
{
	RenderPassBinding::RenderPassBinding() : 
		PassEnabled(false),
		Pass{},
		PipelineToPassIndexMapping{}
	{}

	RenderPassBinding::RenderPassBinding(
		SharedRef<RenderPass> renderPass, 
		const std::unordered_map<uint8, uint8>& indexMapping) :
		PassEnabled(true),
		Pass(renderPass),
		PipelineToPassIndexMapping(indexMapping)
	{}
}