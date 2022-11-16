module;

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

export module shader;

const std::string readShaderSourceFile(const std::string& filename);
void checkShaderCompile(const auto shader);
void checkShaderLink(const auto shaderProgram);

export const GLuint
buildShaderProgram(const std::unordered_map<std::string, GLenum>& sourceFiles) {
  const auto shaderProgram{glCreateProgram()};
  for (const auto& [sourceFile, shaderType] : sourceFiles) {
    const auto shader{glCreateShader(shaderType)};
    const auto source = readShaderSourceFile(sourceFile);
    const auto sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);

    glCompileShader(shader);
    checkShaderCompile(shader);

    glAttachShader(shaderProgram, shader);
    glDeleteShader(shader);
  }

  glLinkProgram(shaderProgram);
  checkShaderLink(shaderProgram);
  return shaderProgram;
}

export class BasicShaderProgramProvider {
public:
  const GLuint& program() const {
    if (glIsProgram(_program) == GL_TRUE) return _program;

    _program = buildShaderProgram(
        {{"basic.vert", GL_VERTEX_SHADER}, {"basic.frag", GL_FRAGMENT_SHADER}});

    return _program;
  }

private:
  mutable GLuint _program{};
};

export class TextureShaderProgramProvider {
public:
  const GLuint& program() const {
    if (glIsProgram(_program) == GL_TRUE) return _program;

    _program = buildShaderProgram({{"texture.vert", GL_VERTEX_SHADER},
                                   {"texture.frag", GL_FRAGMENT_SHADER}});

    return _program;
  }

private:
  mutable GLuint _program{};
};

export class LightingShaderProgramProvider {
public:
  const GLuint& program() const {
    if (glIsProgram(_program) == GL_TRUE) return _program;

    _program = buildShaderProgram({{"lighting.vert", GL_VERTEX_SHADER},
                                   {"lighting.frag", GL_FRAGMENT_SHADER}});

    return _program;
  }

private:
  mutable GLuint _program{};
};

export class SphereLightingShaderProgramProvider {
public:
  const GLuint& program() const {
    if (glIsProgram(_program) == GL_TRUE) return _program;

    _program =
        buildShaderProgram({{"sphere_lighting.vert", GL_VERTEX_SHADER},
                            {"sphere_lighting.frag", GL_FRAGMENT_SHADER}});

    return _program;
  }

private:
  mutable GLuint _program{};
};

export class LightSourceShaderProgramProvider {
public:
  const GLuint& program() const {
    if (glIsProgram(_program) == GL_TRUE) return _program;

    _program = buildShaderProgram({{"light_source.vert", GL_VERTEX_SHADER},
                                   {"light_source.frag", GL_FRAGMENT_SHADER}});

    return _program;
  }

private:
  mutable GLuint _program{};
};

const std::string readShaderSourceFile(const std::string& filename) {
  const auto sourcePath = std::filesystem::path("src") / "shaders";
  std::ifstream shaderFile(sourcePath / filename);
  if (!shaderFile.is_open())
    throw std::runtime_error("Fail to open file: " +
                             (sourcePath / filename).string());

  std::stringstream buffer;
  buffer << shaderFile.rdbuf();
  return buffer.str();
}

void checkShaderCompile(const auto shader) {
  GLint success{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::array<GLchar, 1024> infoLog{};
    glGetShaderInfoLog(shader, 1024, nullptr, infoLog.data());
    std::cout << "Shader Compilation Failed" << std::endl
              << infoLog.data() << std::endl;
    throw std::runtime_error(infoLog.data());
  }
}

void checkShaderLink(const auto shaderProgram) {
  GLint success{};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    std::array<GLchar, 1024> infoLog{};
    glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog.data());
    std::cout << "Shader Program Link Failed" << std::endl
              << infoLog.data() << std::endl;
    throw std::runtime_error(infoLog.data());
  }
}

template <typename T>
concept UniformAcceptable =
    std::is_same_v<T, glm::mat4> || std::is_same_v<T, glm::vec4> ||
    std::is_same_v<T, glm::vec3> || std::is_same_v<T, GLfloat>;

export template <UniformAcceptable T>
void setUniformToProgram(const GLuint& shaderProgram, const std::string& name,
                         const T& data) {
  glUseProgram(shaderProgram);
  if constexpr (std::is_same_v<T, glm::mat4>) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                       GL_FALSE, glm::value_ptr(data));
  } else if constexpr (std::is_same_v<T, glm::vec4>) {
    glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                 glm::value_ptr(data));
  } else if constexpr (std::is_same_v<T, glm::vec3>) {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                 glm::value_ptr(data));
  } else if constexpr (std::is_same_v<T, GLfloat>) {
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), data);
  }
}
