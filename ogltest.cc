// Include standard headers
#include <cstdio>
#include <cstdlib>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GL.
#include <GL/gl.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaders.hh"
#include "image.hh"
#include "camera.hh"
#include "obj.hh"

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

  // Read model.
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals; // TODO
  if (!loadOBJ("model.obj", vertices, uvs, normals)) {
    return -1;
  }

  // Insert the triangle data into a vertex buffer.
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer); // 1 buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // Use it:
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
      &vertices[0], GL_STATIC_DRAW /* contents will be modified once only */);

  // UV coordinates, vertex -> texture.
  GLuint uvBuffer;
  glGenBuffers(1, &uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2),
      &uvs[0], GL_STATIC_DRAW);

  // Set clear color to blue.
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Use the depth test (Z-buffer).
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Cull faces (use normals).
  glEnable(GL_CULL_FACE);

  // Load shaders.
  GLuint programID = loadShadersFromFiles("simple.vs", "simple.fs");

  // Model transformation matrix (identity).
  glm::mat4 model(1.0f);

  // Load texture.
  GLuint textureID = loadDDSImage("texture.dds");

  // Get handle for "mvp" in GLSL program.
  GLuint mvpLoc = glGetUniformLocation(programID, "mvp");

  // Get handle for "textureSampler" in GLSL program.
  GLuint textureSamplerLoc = glGetUniformLocation(programID, "textureSampler");

  do {
    // Update position.
    computeMatricesFromInputs(window);

    // Clear the screen: colors and Z-buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader program.
    glUseProgram(programID);

    // Total transformation for model space -> world space
    glm::mat4 mvp = getProjectionMatrix() * getViewMatrix() * model;

    // Input model space -> world space into program.
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    // Input texture sampler into program.
    glActiveTexture(GL_TEXTURE0);            // Texture unit 0.
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerLoc, 0);       // Also texture unit 0.

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

    // Use vertex UV coordinate attribute array [1].
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(
        1,
        2,        /* two coords (U,V) */
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    // Draw it!
    glDrawArrays(
        GL_TRIANGLES, /* form triangle(s) */
        0,            /* first vertex index */
        vertices.size()
    );

    // Stop using vertex attribute arrays.
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);
}
