#version 330 core

in vec2 textureCoord;
out vec4 oColor;

uniform vec4 color;
uniform sampler2D tex;

void main() {
  oColor = texture(tex, textureCoord);
}
