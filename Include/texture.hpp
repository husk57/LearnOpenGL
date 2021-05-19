#ifndef texture_hpp
#define texture_hpp

#include <glad/glad.h>
#include <stb_image.h>

class Texture {
public:
    unsigned int texture;

    Texture(const char* imagePath) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_set_flip_vertically_on_load(true);
        int width, height, colorChannelCount;
        unsigned char *imgData = stbi_load(imagePath, &width, &height, &colorChannelCount, 0);
        if (imgData) {
            GLenum format = 0;
            if (colorChannelCount == 1) {
                format = GL_RED;
            }
            else if (colorChannelCount == 3) {
                format = GL_RGB;
            }
            else if (colorChannelCount == 4) {
                format = GL_RGBA;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(imgData);
    }
};

#endif
