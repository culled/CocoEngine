#include "RenderingResource.h"

#include "RenderingService.h"

namespace Coco::Rendering
{
    RenderingResource::RenderingResource(const ResourceID& id, const string& name) : Resource(id, name)
    {}

    Logging::Logger* RenderingResource::GetRenderingLogger()
    { 
        return EnsureRenderingService()->GetLogger(); 
    }

    RenderingService* RenderingResource::EnsureRenderingService()
    {
        RenderingService* renderService = RenderingService::Get();

        if (renderService == nullptr)
            throw Exception("No active rendering service found");

        return renderService;
    }
}
