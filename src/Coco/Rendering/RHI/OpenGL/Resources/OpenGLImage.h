//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_OPENGLIMAGE_H
#define COCOENGINE_OPENGLIMAGE_H
#include "Coco/Rendering/Graphics/Resources/Image.h"

namespace Coco
{
    class OpenGLGraphicsPlatform;

    class OpenGLImage : public Image
    {
    public:
        OpenGLImage(OpenGLGraphicsPlatform* platform, uint64 id, const ImageDescription& desc);
        ~OpenGLImage();

        uint32 GetHandle() const { return _image; }

    private:
        OpenGLGraphicsPlatform* _platform;
        uint32 _image;
    };
} // Coco

#endif //COCOENGINE_OPENGLIMAGE_H