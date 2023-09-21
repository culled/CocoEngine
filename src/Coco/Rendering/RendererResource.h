#pragma once
#include <Coco\Core\Resources\Resource.h>

namespace Coco::Rendering
{
    class RenderService;

    /// @brief A Resource that needs access to the RenderService
    class RendererResource :
        public Resource
    {
    public:
        RendererResource(const ResourceID& id, const string& name);
        virtual ~RendererResource() = default;

    protected:
        /// @brief Gets the active RenderService. Throws if no RenderService is active
        /// @return The active render service
        RenderService* EnsureRenderService();
    };
}