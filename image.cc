#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <GL/glew.h>

#include "image.hh"

GLuint loadBMPImage(const char *pathToBMPFile) {
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

const unsigned int FOURCC_DXT1 = 0x31545844; // "DXT1"
const unsigned int FOURCC_DXT3 = 0x33545844; // "DXT3"
const unsigned int FOURCC_DXT5 = 0x35545844; // "DXT5"

GLuint loadDDSImage(const char *pathToDDSFile) {
  // Header data.
  unsigned char header[124];
  unsigned int height, width;
  unsigned int linearSize;
  unsigned int mipMapCount;
  unsigned int fourCC;        // Compression format

  FILE *file = fopen(pathToDDSFile, "rb");
  if (!file) {
    fprintf(stderr, "Image file could not be opened: %s\n", pathToDDSFile);
    return 0;
  }

  char magic[4];
  fread(magic, 1, 4, file);
  if (strncmp(magic, "DDS ", 4) != 0) {
    fprintf(stderr, "DDS: Magic number mismatch. Invalid or corrupt file.");
    return 0;
  }

  fread(header, 1, 124, file);

  height      = *(unsigned int *) &header[ 8];
  width       = *(unsigned int *) &header[12];
  //linearSize  = *(unsigned int *) &header[16];
  mipMapCount = *(unsigned int *) &header[24];
  fourCC      = *(unsigned int *) &header[80];

  // Detect format.
  unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
  unsigned int format;
  switch (fourCC) {
    case FOURCC_DXT1:
      format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      break;
    case FOURCC_DXT3:
      format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      break;
    case FOURCC_DXT5:
      format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      break;
    default:
      fprintf(stderr, "DDS: Unknown fourCC: 0x%08x\n", fourCC);
      return 0;
  }

  // Create OpenGL texture.
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // DXT1 has 64-bit blocks, and the rest have 128-bit blocks.
  unsigned int blockSize =
    (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

  // Calculate linearSize because it's broken.
  linearSize = ((width+3)/4) * ((height+3)/4) * blockSize;

  // Calculate buffer size based on mipmap count.
  // (In any case, must be < linearSize * 2)
  unsigned int bufSize  = mipMapCount > 1 ? linearSize * 2 : linearSize;
  unsigned char *buffer = new unsigned char[bufSize];

  // Read and close.
  fread(buffer, 1, bufSize, file);
  fclose(file);

  // Load mipmaps. `level` is the mipmap level. Each successive level is
  // one-quarter the size of the previous one.
  unsigned int level, offset = 0;
  for (level = 0; level < mipMapCount && (width || height); level++) {
    printf("Level %d (%d/%d)\n", level, offset, bufSize);

    unsigned int size = ((width+3)/4) * ((height+3)/4) * blockSize;

    // Insert mipmap.
    glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
        0, size, buffer + offset);

    offset += size;
    width  /= 2;
    height /= 2; // width/2 & height/2 => quarter
  }
  delete buffer;

  return textureID;
}
