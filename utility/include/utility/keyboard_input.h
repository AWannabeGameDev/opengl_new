#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <unordered_map>
#include <GLFW/glfw3.h>
#include <string>
#include <string_view>

class KeyboardInput
{
private :
	struct KeyStatus
	{
		bool previous;
		bool current;
	};

	std::unordered_map<std::string, int> keyNameMap;
	std::unordered_map<int, KeyStatus> keyStatusMap;
	GLFWwindow* window;

public :
	KeyboardInput(GLFWwindow* glfwWindow);

	void setKeybind(std::string_view keyName, int glfwKey);
	void removeKeybind(std::string_view keyName);

	void update();

	bool keyJustPressed(std::string_view keyName) const;
	bool keyPressed(std::string_view keyName) const;
	bool keyJustReleased(std::string_view keyName) const;
	bool keyReleased(std::string_view keyName) const;

	bool anyKeyJustPressed() const;
	bool anyKeyPressed() const;
	bool anyKeyJustReleased() const;
	bool anyKeyReleased() const;
};

#endif