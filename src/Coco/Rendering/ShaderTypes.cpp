//
// Created by cullen on 3/12/26.
//
#include "ShaderTypes.h"

#include "Coco/Core/Math/Math.h"

namespace Coco
{
    AttachmentBlendState::AttachmentBlendState(BlendFactorMode colorSrcFactor, BlendFactorMode colorDstFactor,
        BlendOperation colorOp, BlendFactorMode alphaSrcFactor, BlendFactorMode alphaDstFactor,
        BlendOperation alphaOp) noexcept :
        ColorSourceFactor(colorSrcFactor),
        ColorDestinationFactor(colorDstFactor),
        ColorBlendOperation(colorOp),
        AlphaSourceFactor(alphaSrcFactor),
        AlphaDestinationFactor(alphaDstFactor),
        AlphaBlendOperation(alphaOp)
    {}

    const AttachmentBlendState AttachmentBlendState::None = AttachmentBlendState(
        BlendFactorMode::One, BlendFactorMode::Zero, BlendOperation::None,
        BlendFactorMode::One, BlendFactorMode::Zero, BlendOperation::None
    );

    const AttachmentBlendState AttachmentBlendState::Opaque = AttachmentBlendState(
        BlendFactorMode::One, BlendFactorMode::Zero, BlendOperation::None,
        BlendFactorMode::One, BlendFactorMode::Zero, BlendOperation::None
    );

    const AttachmentBlendState AttachmentBlendState::AlphaBlending = AttachmentBlendState(
        BlendFactorMode::SourceAlpha, BlendFactorMode::OneMinusSourceAlpha, BlendOperation::Add,
        BlendFactorMode::SourceAlpha, BlendFactorMode::OneMinusSourceAlpha, BlendOperation::Add
    );

    bool AttachmentBlendState::operator==(const AttachmentBlendState& rhs) const noexcept
    {
        return ColorSourceFactor == rhs.ColorSourceFactor &&
            ColorDestinationFactor == rhs.ColorDestinationFactor &&
            ColorBlendOperation == rhs.ColorBlendOperation &&
            AlphaSourceFactor == rhs.AlphaSourceFactor &&
            AlphaDestinationFactor == rhs.AlphaDestinationFactor &&
            AlphaBlendOperation == rhs.AlphaBlendOperation;
    }

    uint64 ToHash(const AttachmentBlendState& blendState)
    {
        return Math::CombineHashes(
            static_cast<uint64>(blendState.ColorSourceFactor),
            static_cast<uint64>(blendState.ColorDestinationFactor),
            static_cast<uint64>(blendState.ColorBlendOperation),
            static_cast<uint64>(blendState.AlphaSourceFactor),
            static_cast<uint64>(blendState.AlphaDestinationFactor),
            static_cast<uint64>(blendState.AlphaBlendOperation)
        );
    }

    GraphicsPipelineState::GraphicsPipelineState() noexcept :
        TopologyMode(MeshTopologyMode::Triangles),
        CullingMode(CullMode::Back),
        WindingMode(TriangleWindingMode::Clockwise),
        FillMode(PolygonFillMode::Fill),
        EnableDepthClamping(true),
        DepthTestMode(DepthTestingMode::LessThan),
        EnableDepthWrite(true),
        BlendState(AttachmentBlendState::Opaque)
    {}

    bool GraphicsPipelineState::operator==(const GraphicsPipelineState& rhs) const noexcept
    {
        return TopologyMode == rhs.TopologyMode &&
            CullingMode == rhs.CullingMode &&
            WindingMode == rhs.WindingMode &&
            FillMode == rhs.FillMode &&
            EnableDepthClamping == rhs.EnableDepthClamping &&
            DepthTestMode == rhs.DepthTestMode &&
            EnableDepthWrite == rhs.EnableDepthWrite &&
            BlendState == rhs.BlendState;
    }

    uint64 ToHash(const GraphicsPipelineState& state)
    {
        return Math::CombineHashes(
            static_cast<uint64>(state.TopologyMode),
            static_cast<uint64>(state.CullingMode),
            static_cast<uint64>(state.WindingMode),
            static_cast<uint64>(state.FillMode),
            static_cast<uint64>(state.EnableDepthClamping),
            static_cast<uint64>(state.DepthTestMode),
            static_cast<uint64>(state.EnableDepthWrite),
            ToHash(state.BlendState)
        );
    }
}
