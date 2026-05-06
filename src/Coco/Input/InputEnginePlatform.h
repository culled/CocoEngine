//
// Created by cullen on 3/29/26.
//

#ifndef COCOENGINE_INPUTENGINEPLATFORM_H
#define COCOENGINE_INPUTENGINEPLATFORM_H

namespace Coco
{
    class InputEnginePlatform
    {
    public:
        virtual ~InputEnginePlatform() = default;

        virtual bool SupportsMouse() const = 0;
        virtual bool SupportsKeyboard() const = 0;
        virtual bool SupportsRawInput() const = 0;
    };
} // Coco

#endif //COCOENGINE_INPUTENGINEPLATFORM_H