//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_GRAPHICSPLATFORMTYPES_H
#define COCOENGINE_GRAPHICSPLATFORMTYPES_H
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Types/Version.h"

#include "Resources/ImageTypes.h"

namespace Coco
{
    enum class GraphicsDeviceType : uint8
    {
        Discrete,
        Integrated,
        Other
    };

    struct GraphicsDeviceCreateParams
    {
        GraphicsDeviceType PreferredDeviceType;
        bool SupportPresentation;
        bool RequireComputeCapability;

        GraphicsDeviceCreateParams();
    };

    struct GraphicsDeviceDescription
    {
        GraphicsDeviceType Type;
        String Name;
        Version DriverVersion;

        /// @brief The maximum amount of MSAA samples this device supports
        MSAASamples MaximumMSAASamples;

        /// @brief The maximum width of an image
        uint32 MaxImageWidth;

        /// @brief The maximum height of an image
        uint32 MaxImageHeight;

        /// @brief The maximum depth of an image
        uint32 MaxImageDepth;

        /// @brief The minimum alignment for a data buffer
        uint32 MinimumBufferAlignment;

        /// @brief The maximum anisotropic filtering level
        uint32 MaxAnisotropicLevel;

        /// @brief If true, this device supports drawing polygons in wireframe
        bool SupportsWireframe;

        uint32 MaxPushConstantSize;
    };
}

#endif //COCOENGINE_GRAPHICSPLATFORMTYPES_H