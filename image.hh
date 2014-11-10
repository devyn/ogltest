#ifndef IMAGE_HH
#define IMAGE_HH

#include <GL/gl.h>

// Note: texture dimensions must be powers of two.
GLuint loadBMPImage(const char *pathToBMPFile);

#endif
