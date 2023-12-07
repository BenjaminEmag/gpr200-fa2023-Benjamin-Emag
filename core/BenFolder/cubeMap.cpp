#pragma once

#include "cubeMap.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

namespace MyLib {
	unsigned int loadCubemap(std::string faces[])
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < 6; i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

    unsigned int generateSkyboxVAO(float scale) {
        float skyboxVertices[] = {
            // Front face
            -scale,  scale, -scale,
            -scale, -scale, -scale,
             scale, -scale, -scale,
             scale, -scale, -scale,
             scale,  scale, -scale,
            -scale,  scale, -scale,

            // Back face
            -scale, -scale,  scale,
            -scale,  scale,  scale,
             scale,  scale,  scale,
             scale,  scale,  scale,
             scale, -scale,  scale,
            -scale, -scale,  scale,

            // Left face
            -scale, -scale, -scale,
            -scale,  scale, -scale,
            -scale,  scale,  scale,
            -scale,  scale,  scale,
            -scale, -scale,  scale,
            -scale, -scale, -scale,

            // Right face
             scale, -scale, -scale,
             scale,  scale, -scale,
             scale,  scale,  scale,
             scale,  scale,  scale,
             scale, -scale,  scale,
             scale, -scale, -scale,

             // Top face
            -scale,  scale, -scale,
            -scale,  scale,  scale,
             scale,  scale,  scale,
             scale,  scale,  scale,
             scale,  scale, -scale,
            -scale,  scale, -scale,

            // Bottom face
           -scale, -scale, -scale,
           -scale, -scale,  scale,
            scale, -scale, -scale,
            scale, -scale, -scale,
           -scale, -scale,  scale,
            scale, -scale,  scale,
        };

        unsigned int skyboxVAO, skyboxVBO;
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);

        glBindVertexArray(skyboxVAO);

        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        // Set the vertex attributes (only position for the skybox)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return skyboxVAO;
    }



}
