#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void stbi_clear_error()
{
	stbi__err(0, 0);
}