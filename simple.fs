#version 330 core

out vec3 color;

void main() {
  color = vec3(1, 0.5 + gl_FragCoord.x / 1000 * 0.5, 1);
}
