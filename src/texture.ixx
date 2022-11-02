module;

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>
#include <stdexcept>

export module texture;

export class TextureProvider {
public:
  TextureProvider(std::string fileName) : _fileName(fileName) {}
  const GLuint& texture() const {
    if (glIsTexture(_texture) == GL_TRUE) return _texture;

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST_MIPMAP_LINEAR);

    int imageWidth{}, imageHeight{}, imageChannels{};
    unsigned char* imageData =
        stbi_load(_fileName.c_str(), &imageWidth, &imageHeight, &imageChannels, 0);
    if (!imageData) {
      glDeleteTextures(1, &_texture);
      throw std::runtime_error("No such file: " + _fileName);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imageData);
    return _texture;
  }
private:
  mutable GLuint _texture{};
  const std::string _fileName{};
};
