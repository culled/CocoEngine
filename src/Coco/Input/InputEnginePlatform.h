//
// Created by cullen on 3/29/26.
//

#ifndef COCOENGINE_INPUTENGINEPLATFORM_H
#define COCOENGINE_INPUTENGINEPLATFORM_H

namespace Coco
{
    /// @brief A class that an EnginePlatform that supports input should implement
    class InputEnginePlatform
    {
    public:
        virtual ~InputEnginePlatform() = default;

        /// @brief Determines if the platform supports mouse input
        /// @return True if the platform supports mouse input
        virtual bool SupportsMouse() const = 0;

        /// @brief Determines if the platform supports keyboard input
        /// @return True if the platform supports keyboard input
        virtual bool SupportsKeyboard() const = 0;

        /// @brief Determines if the platform supports raw input
        /// @return True if the platform supports raw input
        virtual bool SupportsRawInput() const = 0;
    };
} // Coco

#endif //COCOENGINE_INPUTENGINEPLATFORM_H