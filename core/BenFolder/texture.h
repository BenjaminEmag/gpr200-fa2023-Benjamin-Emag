#pragma once

#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode);
GLenum getFormat(int numComponents);
GLenum getTextWrapS(int wrapMode);
GLenum getTextWrapT(int wrapMode);
GLenum getMinFilter(int filterMode);
GLenum getMagFilter(int filterMode);