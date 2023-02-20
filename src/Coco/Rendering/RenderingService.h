#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>

#include "RenderView.h"
#include "Graphics/GraphicsPlatform.h"
#include "Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
    /// <summary>
    /// A service that allows for rendering operations
    /// </summary>
    class COCOAPI RenderingService : public EngineService
    {
    private:
        Managed<GraphicsPlatform> _graphics;

    public:
        RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams);
        ~RenderingService();

        virtual Logging::Logger* GetLogger() const override;
        virtual void Start() override;

        // TODO
        void Render(const RenderView& view, const RenderPipeline& pipeline);
    };
}

