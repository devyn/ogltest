#ifndef CAMERA_HH
#define CAMERA_HH

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void computeMatricesFromInputs(GLFWwindow *window);

glm::mat4 getProjectionMatrix();
glm::mat4 getViewMatrix();

#endif
