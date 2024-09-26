#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H

#include <string>
#include <string_view>
#include <GLFW/glfw3.h>
#include <unordered_map>

class MouseInput
{
private :
	struct ButtonStatus
	{
		bool previous;
		bool current;
	};

	GLFWwindow* window;
	double prevMousePosX, prevMousePosY;
	double curMousePosX, curMousePosY;

	std::unordered_map<std::string, int> buttonNameMap;
	std::unordered_map<int, ButtonStatus> buttonStatusMap;

public :
	MouseInput(GLFWwindow* window);

	void setButtonBind(std::string_view buttonName, int glfwButton);
	void removeButtonBind(std::string_view buttonName);

	void update();

	bool buttonPressed(std::string_view buttonName) const;
	bool buttonJustPressed(std::string_view buttonName) const;
	bool buttonReleased(std::string_view buttonName) const;
	bool buttonJustReleased(std::string_view buttonName) const;

	bool mouseMoved() const;
	float getMouseMovementX() const;
	float getMouseMovementY() const;
	float getMousePositionX() const;
	float getMousePositionY() const;
};

#endif