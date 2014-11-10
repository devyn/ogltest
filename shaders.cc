#include <fstream>
#include <vector>
#include <cstdio>

#include <GL/glew.h>
#include "shaders.hh"

GLuint loadShadersFromFiles(const char *vertexShaderPath,
    const char *fragmentShaderPath) {

  // Create the shaders
  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string vertexShaderCode;
  std::ifstream vertexShaderStream(vertexShaderPath, std::ios::in);
  if(vertexShaderStream.is_open())
  {
      std::string Line = "";
      while(getline(vertexShaderStream, Line))
          vertexShaderCode += "\n" + Line;
      vertexShaderStream.close();
  }

  // Read the Fragment Shader code from the file
  std::string fragmentShaderCode;
  std::ifstream fragmentShaderStream(fragmentShaderPath, std::ios::in);
  if(fragmentShaderStream.is_open()){
      std::string Line = "";
      while(getline(fragmentShaderStream, Line))
          fragmentShaderCode += "\n" + Line;
      fragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int infoLogLength;

  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertexShaderPath);
  char const * vertexSourcePointer = vertexShaderCode.c_str();
  glShaderSource(vertexShaderID, 1, &vertexSourcePointer , NULL);
  glCompileShader(vertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
  std::vector<char> vertexShaderErrorMessage(infoLogLength);
  glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
  fprintf(stdout, "%s\n", &vertexShaderErrorMessage[0]);

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragmentShaderPath);
  char const * fragmentSourcePointer = fragmentShaderCode.c_str();
  glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer , NULL);
  glCompileShader(fragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
  std::vector<char> fragmentShaderErrorMessage(infoLogLength);
  glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
  fprintf(stdout, "%s\n", &fragmentShaderErrorMessage[0]);

  // Link the program
  fprintf(stdout, "Linking program\n");
  GLuint programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);

  // Check the program
  glGetProgramiv(programID, GL_LINK_STATUS, &Result);
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
  std::vector<char> programErrorMessage( std::max(infoLogLength, int(1)) );
  glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
  fprintf(stdout, "%s\n", &programErrorMessage[0]);

  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);

  return programID;
}
