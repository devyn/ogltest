#version 330 core

// Interpolated UV coords.
in vec2 UV;

// Output color from texture at [U,V].
out vec3 color;

// Sampler for the texture we want to use.
uniform sampler2D textureSampler;

void main() {
  // RGB @ textureSampler[U,V].
  color = texture(textureSampler, UV).rgb;
}
