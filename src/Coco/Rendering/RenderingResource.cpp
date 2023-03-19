#include "RenderingResource.h"

namespace Coco::Rendering
{
    Logging::Logger* RenderingResource::GetRenderingLogger() const 
    { 
        return EnsureRenderingService()->GetLogger(); 
    }

    RenderingService* RenderingResource::EnsureRenderingService() const
    {
        RenderingService* renderService = RenderingService::Get();

        if (renderService == nullptr)
            throw Exception("No active rendering service found");

        return renderService;
    }
}
