//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_RENDERINGENGINEPLATFORM_H
#define COCOENGINE_RENDERINGENGINEPLATFORM_H

namespace Coco
{
    class GraphicsPlatform;

    /// @brief Holds rendering extensions for a platform
    struct PlatformRenderingExtensions
    {
        virtual ~PlatformRenderingExtensions() = default;
    };

    /// @brief A class that an EnginePlatform that supports rendering should implement
    class RenderingEnginePlatform
    {
    public:
        virtual ~RenderingEnginePlatform() = default;

        /// @brief Gets platform-specific extensions for rendering
        /// @param graphicsPlatform The graphics platform
        /// @param extensions The rendering extensions
        virtual void GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions) = 0;
    };
} // Coco

#endif //COCOENGINE_RENDERINGENGINEPLATFORM_H