//
// Created by cullen on 3/28/26.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stbimage.h"

void stbi_clear_error()
{
    stbi__err(0, 0);
}