#include "RenderingResource.h"

#include "RenderingService.h"

namespace Coco::Rendering
{
    RenderingResource::RenderingResource(const string& name, ResourceType type) : Resource(name, type)
    {}

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
