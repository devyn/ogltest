// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include "shaders.hh"

int main() {
  // Initialize GLFW
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW.\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
 
  // Open a window and create its OpenGL context
  GLFWwindow* window; // (In the accompanying source code, this variable is global)
  window = glfwCreateWindow(1024, 768, "Tutorial 02", NULL, NULL);
  if (window == NULL){
      fprintf(stderr, "Failed to open GLFW window.\n");
      glfwTerminate();
      return -1;
  }
 
  glfwMakeContextCurrent(window); // Initialize GLEW
  glewExperimental = true; // Needed in core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW.\n");
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Set up a Vertex Array Object.
  GLuint vertexArrayID;

  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  // Vertices representing a triangle.
  static const GLfloat gVertexBufferData[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
  };

  // Insert the triangle data into a vertex buffer.
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer); // 1 buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // Use it:
  glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData,
      GL_STATIC_DRAW /* contents will be modified once only */);

  // Set clear color to blue.
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Load shaders.
  GLuint programID = loadShadersFromFiles("simple.vs", "simple.fs");

  do {
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader program.
    glUseProgram(programID);

    // Use vertex attribute array [0]. (arbitrary)
    glEnableVertexAttribArray(0);

    // Use our vertex buffer:
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // Tell OpenGL what kind of data is in the buffer:
    glVertexAttribPointer(
        0,        /* attribute 0 */
        3,        /* size (# vertices) */
        GL_FLOAT, /* data type */
        GL_FALSE, /* are they normalized? (TODO: what is this?) */
        0,        /* stride (TODO: look up) */
        (void*)0  /* offset */
    );

    // Draw it!
    glDrawArrays(
        GL_TRIANGLES, /* form triangle(s) */
        0,            /* first vertex index */
        3             /* # vertices */
    );

    // Stop using vertex attribute array.
    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);
}
