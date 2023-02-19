#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>

#include "RenderView.h"
#include "Graphics/GraphicsPlatform.h"
#include "Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
    class COCOAPI RenderingService : public EngineService
    {
    private:
        Managed<GraphicsPlatform> _graphics;

    public:
        RenderingService(const GraphicsBackendCreationParameters& backendCreateParams);
        ~RenderingService();

        virtual Logging::Logger* GetLogger() const override;
        virtual void Start() override;

        void Render(const RenderView& view, const RenderPipeline& pipeline);
    };
}

