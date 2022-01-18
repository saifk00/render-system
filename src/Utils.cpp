#include <rendersystem/Utils.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>



namespace Utils {
    unsigned int TextureFromFile(const std::string& path,
            std::function<void(void)> textureSettingsCallback) {
        unsigned int texId;

        glGenTextures(1, &texId);

        int width, height, c;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(
            path.c_str(),
            &width,
            &height,
            &c,
            0);

        if (data == nullptr) {
            std::cout << "failed to load texture from " << path << std::endl;
            return -1;
        }
        else {
            GLenum format;
            if (c == 1) format = GL_RED;
            else if (c == 3) format = GL_RGB;
            else if (c == 4) format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, texId);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            textureSettingsCallback();

            stbi_image_free(data);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        return texId;
    }
}