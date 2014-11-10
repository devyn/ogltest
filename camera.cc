#include <ctime>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hh"

glm::mat4 camera_projectionMatrix;
glm::mat4 camera_viewMatrix;

glm::vec3 position = glm::vec3(0, 0, 5);

GLfloat horizontalAngle    = 3.14f; // toward -Z
GLfloat verticalAngle      = 0.0f;  // look at horizon.

GLfloat speed              = 3.0f;  // 3 u/s
GLfloat mouseSpeed         = 0.05f;

double  mouseWheel         = 0.0;

const GLfloat INITIAL_FIELD_OF_VIEW = 45.0f;

void handleScroll(GLFWwindow *window, double xoffset, double yoffset) {
  mouseWheel += yoffset;
}

void initializeCamera(GLFWwindow *window) {
  glfwSetScrollCallback(window, &handleScroll);
}

void computeMatricesFromInputs(GLFWwindow *window) {
  static bool initialized = false;

  if (!initialized) {
    initializeCamera(window);
    initialized = true;
  }

  static double lastTime = glfwGetTime();

  double currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);
  lastTime = currentTime;

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  // Reset mouse position (so it doesn't move)
  glfwSetCursorPos(window, 1024/2, 768/2);

  // Compute new orientation
  horizontalAngle += mouseSpeed * deltaTime * float(1024/2 - xpos);
  verticalAngle   += mouseSpeed * deltaTime * float( 768/2 - ypos);

  // Compute direction (front vector) as Cartesian coordinates
  glm::vec3 direction(
      cos(verticalAngle) * sin(horizontalAngle),
      sin(verticalAngle),
      cos(verticalAngle) * cos(horizontalAngle)
  );

  // Compute right vector
  glm::vec3 right(
      sin(horizontalAngle - M_PI/2.0f),
      0,
      cos(horizontalAngle - M_PI/2.0f)
  );

  // Up vector is perpendicular to front and right
  glm::vec3 up = glm::cross(right, direction);

  // Standard WASD position + space & C
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    position += direction * deltaTime * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    position -= direction * deltaTime * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    position -= right * deltaTime * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    position += right * deltaTime * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    position += up * deltaTime * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    position -= up * deltaTime * speed;
  }

  // Zoom (field of view) : mouse wheel
  GLfloat fieldOfView = INITIAL_FIELD_OF_VIEW - 5 * mouseWheel;

  // Finally, compute matrices
  // Project: 4:3 ratio, 0.1 <-> 100 units range
  camera_projectionMatrix =
    glm::perspective(fieldOfView, 4.0f / 3.0f, 0.1f, 100.0f);

  // View: position, direction, up
  camera_viewMatrix = glm::lookAt(
      position,
      position + direction,
      up
  );
}

glm::mat4 getProjectionMatrix() {
  return camera_projectionMatrix;
}

glm::mat4 getViewMatrix() {
  return camera_viewMatrix;
}
