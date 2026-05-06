//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_X11INCLUDES_H
#define COCOENGINE_X11INCLUDES_H

namespace X11
{
    #include <X11/Xlib.h>
    #include <X11/extensions/Xrandr.h>
    #include <X11/Xatom.h>
    #include <X11/keysymdef.h>
    #ifdef COCO_SERVICE_INPUT
    #include <X11/extensions/XInput2.h>
    #endif
}

// Undef defines that pollute the namespace
#undef None
#undef Always
#undef Bool
#undef Status
#undef True
#undef False
#undef MouseButton
#undef Button3
#undef Button4

#endif //COCOENGINE_X11INCLUDES_H