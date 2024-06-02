#include <utility/profiler.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

Profiler::Profiler(const char* message, float& accumulator) :
	message{message}, startTime{(float)glfwGetTime()}, accumulator{accumulator}
{}

Profiler::~Profiler()
{
	float interval = ((float)glfwGetTime() - startTime) * 1000.0f;
	accumulator += interval;
	printf("Profiler : %s | Took %f ms\n", message, interval);
}