//
// Created by cullen on 3/2/26.
//

#ifndef COCOENGINE_X11TYPES_H
#define COCOENGINE_X11TYPES_H

namespace X11
{
    #include <X11/Xdefs.h>

    typedef XID Window;
    typedef struct _XDisplay Display;
    typedef union _XEvent XEvent;
}

#endif //COCOENGINE_X11TYPES_H