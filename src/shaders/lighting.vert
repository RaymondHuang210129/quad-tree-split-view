#version 330 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;

out vec3 fragmentNormal;
out vec3 fragmentPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
  gl_Position = proj * view * model * vec4(iPosition.xyz, 1.0);
  fragmentNormal = vec3(model * vec4(iNormal, 0.0));
  fragmentPosition = vec3(model * vec4(iPosition, 1.0));
}
