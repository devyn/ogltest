#version 330 core

// attribute #0 corresponds to vertexPosition_modelspace
// (name and number are arbitrary)
layout(location = 0) in vec3 vertexPosition_modelspace;

// Our transformation matrix for model space to world space.
uniform mat4 mvp;

void main() {
  // Model space as a 4-vector. (w = 1)
  vec4 v = vec4(vertexPosition_modelspace, 1);

  // Transform model space -> world space.
  gl_Position = mvp * v;
}
