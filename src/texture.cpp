#include <glad/glad.h>

#include "../include/texture.h"
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"


Texture2D::Texture2D(const std::string &filepath) {
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);



    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}

GLuint Texture2D::getTexture() const {
    return texture_id_;
}

Texture3D::Texture3D(std::vector<std::string> filepaths) {
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < filepaths.size(); i++)
    {
        unsigned char *data = stbi_load(filepaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = 0;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << filepaths[i] << std::endl;
            stbi_image_free(data);
        }
    }


}



GLuint Texture3D::getTexture() const {
    return texture_id_;
}