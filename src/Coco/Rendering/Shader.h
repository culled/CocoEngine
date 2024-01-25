#pragma once
#include <Coco\Core\Resources\Resource.h>
#include "Graphics/GraphicsPipelineTypes.h"
#include "Graphics/ShaderUniformLayout.h"
#include "MeshTypes.h"

namespace Coco::Rendering
{
    class Shader :
        public Resource
    {
        friend class ShaderSerializer;

    public:
        Shader(const ResourceID& id, const string& name);
        Shader(
            const ResourceID& id, 
            const string& name, 
            const std::vector<ShaderStage>& stages,
            const GraphicsPipelineState& pipelineState,
            const std::vector<AttachmentBlendState>& attachmentBlendStates,
            const VertexDataFormat& vertexFormat,
            const ShaderUniformLayout& globalUniforms,
            const ShaderUniformLayout& instanceUniforms,
            const ShaderUniformLayout& drawUniforms);

        // Inherited via Resource
        const std::type_info& GetType() const override { return typeid(Shader); }
        const char* GetTypename() const override { return "Shader"; }

        const string& GetName() const { return _name; }
        std::span<const ShaderStage> GetStages() const { return _stages; }
        const GraphicsPipelineState& GetPipelineState() const { return _pipelineState; }
        std::span<const AttachmentBlendState> GetAttachmentBlendStates() const { return _attachmentBlendStates; }
        const VertexDataFormat& GetVertexDataFormat() const { return _vertexFormat; }
        const ShaderUniformLayout& GetGlobalUniformLayout() const { return _globalUniforms; }
        const ShaderUniformLayout& GetInstanceUniformLayout() const { return _instanceUniforms; }
        const ShaderUniformLayout& GetDrawUniformLayout() const { return _drawUniforms; }

    private:
        string _name;
        std::vector<ShaderStage> _stages;
        GraphicsPipelineState _pipelineState;
        std::vector<AttachmentBlendState> _attachmentBlendStates;
        VertexDataFormat _vertexFormat;
        ShaderUniformLayout _globalUniforms;
        ShaderUniformLayout _instanceUniforms;
        ShaderUniformLayout _drawUniforms;

    private:
        void EnsureLayoutDataCalculated();
    };
}