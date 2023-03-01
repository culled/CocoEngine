#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>

#include "Graphics/RenderView.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/GraphicsPresenter.h"
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
        Ref<RenderPipeline> _defaultPipeline;

    public:
        RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams);
        ~RenderingService();

        virtual Logging::Logger* GetLogger() const override;
        virtual void Start() override;

        /// <summary>
        /// Creates a graphics presenter
        /// </summary>
        /// <returns>A graphics presenter</returns>
        GraphicsPresenter* CreatePresenter() const { return _graphics->CreatePresenter(); }

        void SetDefaultPipeline(Ref<RenderPipeline> pipeline);

        // TODO
        void Render(GraphicsPresenter* presenter);
        void Render(GraphicsPresenter* presenter, const Ref<RenderPipeline>& pipeline);

    private:
        void DoRender(const Ref<RenderPipeline>& pipeline, RenderContext* context);
    };
}

