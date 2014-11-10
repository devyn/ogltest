// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

  // Vertices representing a cube.
  static const GLfloat gVertexBufferData[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
  };

  // Insert the triangle data into a vertex buffer.
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer); // 1 buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // Use it:
  glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData,
      GL_STATIC_DRAW /* contents will be modified once only */);

  // One color for each vertex.
  GLfloat gColorBufferData[12 * 3 * 3];
  for (int v = 0; v < 12 * 3; v++) {
    gColorBufferData[3*v+0] = (gVertexBufferData[3*v+0]+1)/2.0f; // Red
    gColorBufferData[3*v+1] = (gVertexBufferData[3*v+1]+1)/2.0f; // Green
    gColorBufferData[3*v+2] = (gVertexBufferData[3*v+2]+1)/2.0f; // Blue
  }

  GLuint colorBuffer;
  glGenBuffers(1, &colorBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(gColorBufferData), gColorBufferData,
      GL_STATIC_DRAW);

  // Set clear color to blue.
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Use the depth test (Z-buffer).
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Load shaders.
  GLuint programID = loadShadersFromFiles("simple.vs", "simple.fs");

  // Generate transformation matrices.
  // 45 degree FoV, 4:3 aspect ratio, 0.1 <-> 100 units range
  glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

  // View transformation.
  glm::mat4 view = glm::lookAt(
      glm::vec3(4, 3, 3), // Camera position (world)
      glm::vec3(0, 0, 0), // Point to aim at (world)
      glm::vec3(0, 1, 0)  // "Up" direction
  );

  GLfloat c = 0; // Used to move the model.

  // Get handle for "mvp" in GLSL program.
  GLuint matrixID = glGetUniformLocation(programID, "mvp");

  do {
    // Clear the screen: colors and Z-buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader program.
    glUseProgram(programID);

    // Rotation matrix for model.
    c += 0.5f;
    glm::mat4 model = glm::translate(glm::vec3(0, cos(0.05f * c), sin(0.05f * c))) *
                      glm::rotate(c, glm::vec3(0,0.5,1));

    // Total transformation for model space -> world space
    glm::mat4 mvp = projection * view * model;

    // Input model space -> world space into program.
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

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

    // Use vertex color attribute array [1].
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    // Draw it!
    glDrawArrays(
        GL_TRIANGLES, /* form triangle(s) */
        0,            /* first vertex index */
        12*3          /* # vertices */
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
