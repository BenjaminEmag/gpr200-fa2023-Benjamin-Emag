#pragma once

#include "cubeMap.cpp"
#include <string>

namespace MyLib {
	unsigned int loadCubemap(std::string faces[]);
	unsigned int generateSkyboxVAO(float scale);
}