//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_GRAPHICSSURFACE_H
#define COCOENGINE_GRAPHICSSURFACE_H
#include "Coco/Core/Math/Size.h"
#include "Coco/Rendering/Graphics/GraphicsResource.h"

namespace Coco
{
    enum class VSyncMode
    {
        Immediate = 0,
        EveryVBlank
    };

    class GraphicsSurface : public GraphicsResource
    {
    public:
        virtual  ~GraphicsSurface() = default;

        virtual void SetFramebufferSize(const Sizei& framebufferSize) = 0;
        virtual Sizei GetFramebufferSize() const = 0;
        virtual void SetVSyncMode(VSyncMode mode) = 0;
        virtual VSyncMode GetVSyncMode() const = 0;
        virtual void RebuildIfNeeded() {}

    protected:
        GraphicsSurface(uint64 id);
    };
} // Coco

#endif //COCOENGINE_GRAPHICSSURFACE_H