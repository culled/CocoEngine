//
// Created by cullen on 3/2/26.
//
#include "WindowTypes.h"

namespace Coco
{
    const char* ToString(WindowState state)
    {
        switch (state)
        {
            case WindowState::Minimized:
                return "Minimized";
            case WindowState::Maximized:
                return "Maximized";
            default:
                return "Default";
        }
    }
}