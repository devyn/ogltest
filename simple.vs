#version 330 core

// attribute #0 corresponds to vertexPosition_modelspace
// (name and number are arbitrary)
layout(location = 0) in vec3 vertexPosition_modelspace;

void main() {
  gl_Position.xyz = vertexPosition_modelspace;
  gl_Position.w   = 1.0; // normalized
}
