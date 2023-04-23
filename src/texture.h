#pragma once
#include <glad/glad.h>
#include <stb_image.h>

#include <stdexcept>
#include <string>

class TextureProvider {
public:
  TextureProvider(std::string fileName);
  const GLuint& texture() const;

private:
  mutable GLuint _texture{};
  const std::string _fileName{};
};
