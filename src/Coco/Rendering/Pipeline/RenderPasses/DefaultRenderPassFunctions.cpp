#include "Renderpch.h"
#include "DefaultRenderPassFunctions.h"

#include "../../Graphics/RenderContext.h"
#include "../../Graphics/RenderView.h"

namespace Coco::Rendering
{
	void DefaultRenderPassFunctions::ApplyDefaultPreparations(RenderContext& context, const RenderView& renderView)
	{
		context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
		context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ViewMatrix"), renderView.GetViewMatrix());
	}
}