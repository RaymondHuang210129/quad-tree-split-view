#version 330 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iTextureCoord;

out vec2 textureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
  textureCoord = iTextureCoord;
  gl_Position = proj * view * model * vec4(iPosition.xyz, 1.0);
}
