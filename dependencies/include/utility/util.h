#ifndef UTIL_H
#define UTIL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string_view>
#include <glm/glm.hpp>

void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
					 GLsizei length, const GLchar* message, const void* userParam);

GLFWwindow* initialize(int windowWidth, int windowHeight, std::string_view title, int majorVersion, int minorVersion);

unsigned int compileShader(unsigned int type, std::string_view path);
unsigned int createShaderProgram(std::string_view vertexShaderPath, std::string_view fragmentShaderPath);
unsigned int createShaderProgram(std::string_view vertexShaderPath, std::string_view geometryShaderPath,
								 std::string_view fragmentShaderPath);

float randrange(float min, float max);

#endif