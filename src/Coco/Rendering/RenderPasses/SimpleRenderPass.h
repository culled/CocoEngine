//
// Created by cullen on 4/22/26.
//

#ifndef COCOENGINE_SIMPLERENDERPASS_H
#define COCOENGINE_SIMPLERENDERPASS_H
#include "Coco/Rendering/RenderScene.h"
#include "Coco/Rendering/ShaderTypes.h"
#include "Coco/Rendering/Graphics/Resources/RenderContext.h"

namespace Coco
{
    template<class SceneDataType, typename ObjectDataType>
    class SimpleRenderPass
    {
    public:
        SimpleRenderPass(const RenderGraphResourceRef& colorAttachment, SharedPtr<Shader> drawShader, const GraphicsPipelineState& pipelineState, const char* cameraDataUniformName = "cameraData") :
            _drawShader(std::move(drawShader)),
            _pipelineState(pipelineState),
            _colorAttachment(colorAttachment),
            _cameraDataUniformName(cameraDataUniformName)
        {}

        void Setup(RenderGraphBuilder& builder)
        {
            _colorAttachment = builder.WriteRenderTarget(_colorAttachment);
        }

        void Render(const RenderScene& sceneData, RenderContext& ctx) const
        {
            ctx.SetShader(*_drawShader, _pipelineState);
            ShaderCursor globalCursor;
            if (ctx.CreateAndBindGlobalBuffer(_cameraDataUniformName.CStr(), globalCursor))
            {
                const SceneDataType* globalData = sceneData.GetData<SceneDataType>(0, false);
                globalData->WriteInto(globalCursor);
            }

            for (const auto& obj : sceneData.GetRenderObjectView())
            {
                if (sceneData.HasData<ObjectDataType>(obj.ID, true))
                {
                    const ObjectDataType* objData = sceneData.GetData<ObjectDataType>(obj.ID, true);
                    objData->SetDrawData(ctx);

                    ctx.DrawObject(obj);
                }
            }
        }

    private:
        SharedPtr<Shader> _drawShader;
        GraphicsPipelineState _pipelineState;
        String _cameraDataUniformName;
        RenderGraphResourceRef _colorAttachment;
    };
} // Coco

#endif //COCOENGINE_SIMPLERENDERPASS_H