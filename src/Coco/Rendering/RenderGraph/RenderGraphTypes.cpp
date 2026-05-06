//
// Created by cullen on 3/8/26.
//
#include "RenderGraphTypes.h"

#include <limits>

namespace Coco
{
    RenderGraphResourceRef::RenderGraphResourceRef() :
        RenderGraphResourceRef(0, ResourceType::None, ResourceAccessType::None)
    {}

    RenderGraphResourceRef::RenderGraphResourceRef(uint32 id, ResourceType type, ResourceAccessType accessType) :
        ID(id),
        Type(type),
        AccessType(accessType)
    {}

    RenderGraphResource::RenderGraphResource(uint32 id, ResourceType type) :
        ID(id),
        Type(type),
        ReadCount(0),
        FirstPassIndex(std::numeric_limits<uint32>::max()),
        LastPassIndex(0),
        IsExternal(false)
    {}

    RenderGraphResourceRef RenderGraphResource::AsResourceRef(ResourceAccessType accessType) const
    {
        return RenderGraphResourceRef(ID, Type, accessType);
    }

    RenderGraphTextureResource::RenderGraphTextureResource(uint32 id, const ImageDescription& desc) :
        RenderGraphResource(id, ResourceType::Texture),
        Desc(desc)
    {}

    RenderGraphAttachment::RenderGraphAttachment(uint64 textureID) :
        TextureID(textureID),
        ClearValue()
    {}

    RenderGraphNode::RenderGraphNode(const char* passName) :
        PassName(passName),
        CallbackFunction(nullptr),
        Inputs(),
        Outputs(),
        PassIndex(0)
    {}
}
