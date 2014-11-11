#version 330 core

// Interpolated UV coords. V coordinate is backwards.
in vec2 UV;

// Output color from texture at [U,V].
out vec3 color;

// Sampler for the texture we want to use.
uniform sampler2D textureSampler;

void main() {
  // Flip V coord.
  vec2 newUV = vec2(UV.x, 1.0 - UV.y);

  // RGB @ textureSampler[U,V].
  color = texture(textureSampler, newUV).rgb;
}
