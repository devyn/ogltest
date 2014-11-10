#include <cstdlib>
#include <cstdio>

#include <GL/glew.h>

#include "image.hh"

GLuint loadBMPImage(const char* pathToBMPFile) {
  // Header data.
  unsigned char header[54];   // Holds entire header.
  unsigned int dataPos;       // Where bitmap image data begins.
  unsigned int width, height;
  unsigned int imageSize;     // size (in bytes) : 24-bit = 3(W x H)

  FILE *file = fopen(pathToBMPFile, "rb");
  if (!file) {
    fprintf(stderr, "Image file could not be opened: %s\n", pathToBMPFile);
    return 0;
  }

  if (fread(header, 1, 54, file) != 54) {
    fprintf(stderr, "BMP file too small.\n");
    return 0;
  }

  if (header[0] != 'B' || header[1] != 'M') {
    fprintf(stderr, "BMP: Magic number mismatch. Invalid or corrupt file.\n");
    return 0;
  }

  // Read the header data.
  dataPos   = *(int *) &header[0x0A];
  imageSize = *(int *) &header[0x22];
  width     = *(int *) &header[0x12];
  height    = *(int *) &header[0x16];

  // Sometimes dataPos or imageSize are missing, so guess.
  if (imageSize == 0) imageSize = width * height * 3; // 24-bit = 3(W x H)
  if (dataPos   == 0) dataPos   = 54;                 // end of header

  fseek(file, dataPos, SEEK_SET);

  // Allocate buffer, read, close.
  unsigned char *data = new unsigned char[imageSize];
  fread(data, 1, imageSize, file);
  fclose(file);

  // Now turn that into an OpenGL texture.
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give it to OpenGL: map BGR (BMP format) -> RGB
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
      GL_UNSIGNED_BYTE, data);

  // Texture filter: linear interpolation.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Generate mipmaps.
  glGenerateMipmap(GL_TEXTURE_2D);

  return textureID;
}
