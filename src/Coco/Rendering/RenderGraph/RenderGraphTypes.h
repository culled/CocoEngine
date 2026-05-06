//
// Created by cullen on 3/8/26.
//

#ifndef COCOENGINE_RENDERGRAPHTYPES_H
#define COCOENGINE_RENDERGRAPHTYPES_H
#include <Coco/Core/Types/CoreTypes.h>
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Types/Optional.h"
#include "Coco/Core/Types/String.h"
#include "Coco/Rendering/Graphics/Resources/ImageTypes.h"
#include "Coco/Core/Types/StackArray.h"
#include <functional>

namespace Coco
{
    class Image;
    class RenderScene;
    class RenderContext;

    using RenderGraphExecuteFunction = std::function<void(const RenderScene&, RenderContext&)>;

    enum class ResourceType : uint8
    {
        None = 0,
        Texture,
        Buffer
    };

    enum class ResourceAccessType : uint8
    {
        None = 0,
        ShaderRead,
        ShaderWrite,
        RenderTarget
    };

    struct RenderGraphResourceRef
    {
        uint32 ID;
        ResourceType Type;
        ResourceAccessType AccessType;

        RenderGraphResourceRef();
        RenderGraphResourceRef(uint32 id, ResourceType type, ResourceAccessType accessType);
    };

    bool operator== (const RenderGraphResourceRef& a, const RenderGraphResourceRef& b);

    struct RenderGraphResource
    {
        uint32 ID;
        ResourceType Type;
        uint32 ReadCount;
        uint32 FirstPassIndex;
        uint32 LastPassIndex;
        bool IsExternal;

        RenderGraphResource(uint32 id, ResourceType type);
        virtual ~RenderGraphResource() = default;

        RenderGraphResourceRef AsResourceRef(ResourceAccessType accessType) const;
    };

    struct RenderGraphTextureResource : public RenderGraphResource
    {
        Ref<Image> TextureImage;
        ImageDescription Desc;
        bool IsAttachment;

        RenderGraphTextureResource(uint32 id, const ImageDescription& desc);
    };

    struct RenderGraphAttachment
    {
        uint64 TextureID;
        Optional<RenderTargetClearValue> ClearValue;

        RenderGraphAttachment(uint64 textureID);
    };

    struct RenderGraphNode
    {
        String PassName;
        RenderGraphExecuteFunction CallbackFunction;
        StackArray<RenderGraphResourceRef, 8> Inputs;
        StackArray<RenderGraphResourceRef, 8> Outputs;
        uint32 PassIndex;

        RenderGraphNode(const char* passName);
    };

    struct RenderPassAttachmentInfo
    {
        Ref<Image> AttachmentImage;
        ImageAttachmentType Type;
        Optional<RenderTargetClearValue> ClearValue;
        bool SaveResult;
    };
}

#endif //COCOENGINE_RENDERGRAPHTYPES_H