#include "shader.h"

static const std::string readShaderSourceFile(const std::string& filename);
static void checkShaderCompile(const auto shader);
static void checkShaderLink(const auto shaderProgram);

static const GLuint
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

const GLuint& BasicShaderProgramProvider::program() const {
  if (glIsProgram(_program) == GL_TRUE) return _program;

  _program = buildShaderProgram(
      {{"basic.vert", GL_VERTEX_SHADER}, {"basic.frag", GL_FRAGMENT_SHADER}});

  return _program;
}

const GLuint& LightingShaderProgramProvider::program() const {
  if (glIsProgram(_program) == GL_TRUE) return _program;

  _program = buildShaderProgram({{"lighting.vert", GL_VERTEX_SHADER},
                                 {"lighting.frag", GL_FRAGMENT_SHADER}});

  return _program;
}

const GLuint& TextureLightingShaderProgramProvider::program() const {
  if (glIsProgram(_program) == GL_TRUE) return _program;

  _program =
      buildShaderProgram({{"texture_lighting.vert", GL_VERTEX_SHADER},
                          {"texture_lighting.frag", GL_FRAGMENT_SHADER}});

  return _program;
}

static const std::string readShaderSourceFile(const std::string& filename) {
  const auto sourcePath = std::filesystem::path("src") / "shaders";
  std::ifstream shaderFile(sourcePath / filename);
  if (!shaderFile.is_open())
    throw std::runtime_error("Fail to open file: " +
                             (sourcePath / filename).string());

  std::stringstream buffer;
  buffer << shaderFile.rdbuf();
  return buffer.str();
}

static void checkShaderCompile(const auto shader) {
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

static void checkShaderLink(const auto shaderProgram) {
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
