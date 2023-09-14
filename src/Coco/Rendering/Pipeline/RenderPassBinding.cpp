#include "Renderpch.h"
#include "RenderPassBinding.h"

namespace Coco::Rendering
{
	RenderPassBinding::RenderPassBinding() : 
		Pass{},
		PipelineToPassIndexMapping{}
	{}

	RenderPassBinding::RenderPassBinding(SharedRef<RenderPass> renderPass, const std::unordered_map<uint8, uint8>& indexMapping) :
		Pass(renderPass),
		PipelineToPassIndexMapping(indexMapping)
	{}
}