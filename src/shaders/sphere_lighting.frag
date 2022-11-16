#version 330 core

in vec3 fragmentNormal;
in vec3 fragmentPosition;
out vec4 oColor;

uniform vec4 color;

uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform float luminousIntensity;

vec3 calcPointLight(vec3 normal, vec3 viewDirection);

void main() {
  vec3 normal = normalize(fragmentNormal);
  vec3 viewDirection = normalize(viewPosition - fragmentPosition);
  oColor = vec4(calcPointLight(normal, viewDirection), color.a);
}

vec3 calcPointLight(vec3 normal, vec3 viewDirection) {
  // diffuse
  vec3 lightDirection = normalize(lightPosition - fragmentPosition);
  float normalDifference = max(dot(normal, lightDirection), 0.0);
  vec3 diffuse = vec3(color) * normalDifference * lightDiffuse;

  // specular
  vec3 reflectDireciton = reflect(-lightDirection, normal);
  float shininess = pow(max(dot(viewDirection, reflectDireciton), 0.0), 20);
  vec3 specular = vec3(color) * shininess * lightSpecular;

  // ambient
  vec3 ambient = vec3(color) * lightAmbient;

  vec3 positionDifference = lightPosition - fragmentPosition;
  float distance = length(positionDifference);
  float illumination = luminousIntensity / (1 + 0.1 * distance + 0.05 * pow(distance, 2));

  return illumination * (specular + ambient + diffuse);
}
