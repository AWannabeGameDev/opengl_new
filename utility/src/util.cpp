#include "utility/util.h"
#include <stdio.h> 
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
					 GLsizei length, const GLchar* message, const void* userParam)
{
	printf("OpenGL debug message : %s\n", message);
}

GLFWwindow* initialize(int windowWidth, int windowHeight, std::string_view title, int majorVersion, int minorVersion)
{
	if(glfwInit() != GLFW_TRUE)
	{
		printf("Failed to initialize GLFW\n");
		return nullptr;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, title.data(), nullptr, nullptr);
	if(!window)
	{
		printf("Failed to create window\n");
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return nullptr;
	}
	printf("Using OpenGL version : %s\n", glGetString(GL_VERSION));

	return window;
}

unsigned int compileShader(unsigned int type, std::string_view path)
{
	std::ifstream file;
	file.open(path.data());
	std::stringstream sstream;
	std::string source;

	if(!file.is_open())
	{
		printf("Failed to open shader file\n");
		return 0;
	} else
	{
		sstream << file.rdbuf();
		source = sstream.str();
	}

	const char* sourceStr = source.c_str();

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &sourceStr, nullptr);
	glCompileShader(shader);

#ifdef _DEBUG

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if(success != GL_TRUE)
	{
		int logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		char* message = (char*)::operator new(logLength);
		glGetShaderInfoLog(shader, logLength, nullptr, message);

		printf("Failed to compile shader %s : %s", path.data(), message);

		::operator delete(message);

		return 0;
	}

#endif

	return shader;
}

unsigned int createShaderProgram(std::string_view vertexShaderPath, std::string_view fragmentShaderPath)
{
	unsigned int vshader = compileShader(GL_VERTEX_SHADER, vertexShaderPath);
	unsigned int fshader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);

	unsigned int program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

#ifdef _DEBUG

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if(success != GL_TRUE)
	{
		int logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		char* message = (char*)::operator new(logLength);
		glGetProgramInfoLog(program, logLength, nullptr, message);

		printf("Failed to link program : %s", message);

		::operator delete(message);

		return 0;
	}

#endif

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	return program;
}

unsigned int createShaderProgram(std::string_view vertexShaderPath, std::string_view geometryShaderPath, 
								 std::string_view fragmentShaderPath)
{
	unsigned int vshader = compileShader(GL_VERTEX_SHADER, vertexShaderPath);
	unsigned int gshader = compileShader(GL_GEOMETRY_SHADER, geometryShaderPath);
	unsigned int fshader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);

	unsigned int program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glAttachShader(program, gshader);
	glLinkProgram(program);

#ifdef _DEBUG

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if(success != GL_TRUE)
	{
		int logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		char* message = (char*)::operator new(logLength);
		glGetProgramInfoLog(program, logLength, nullptr, message);

		printf("Failed to link program : %s", message);

		::operator delete(message);

		return 0;
	}

#endif

	glDeleteShader(vshader);
	glDeleteShader(gshader);
	glDeleteShader(fshader);

	return program;
}

float randrange(float min, float max)
{
	return (((float)rand() * (max - min) / RAND_MAX) + min);
}