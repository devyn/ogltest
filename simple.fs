#version 330 core

// From vertex shader.
in  vec3 fragmentColor;

// Directly mapped from fragmentColor.
out vec3 color;

void main() {
  // Just copy it.
  color = fragmentColor;
}
