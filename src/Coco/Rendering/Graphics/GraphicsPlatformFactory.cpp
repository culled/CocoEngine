#include "Renderpch.h"
#include "GraphicsPlatformFactory.h"

namespace Coco::Rendering
{
	GraphicsPlatformFactory::GraphicsPlatformFactory(const GraphicsPlatformCreateParams& createParams) :
		_createParams(createParams)
	{}
}