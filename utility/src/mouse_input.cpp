#include "utility/mouse_input.h"
#include <stdexcept>

MouseInput::MouseInput(GLFWwindow* window) :
	window{window}
{}

void MouseInput::setButtonBind(std::string_view buttonName, int glfwButton)
{
	buttonNameMap[std::string{buttonName}] = glfwButton;
	buttonStatusMap[glfwButton] = {false, false};
}

void MouseInput::removeButtonBind(std::string_view buttonName)
{
	auto it = buttonNameMap.find(std::string{buttonName});
	if(it != buttonNameMap.end())
	{
		buttonStatusMap.erase(it->second);
		buttonNameMap.erase(it);
	}
	else
	{
		throw std::invalid_argument{"The given buttonbind does not exist."};
	}
}

void MouseInput::update()
{
	for(auto& [button, status] : buttonStatusMap)
	{
		status.previous = status.current;
		if(glfwGetMouseButton(window, button) == GLFW_PRESS)
			status.current = true;
		else
			status.current = false;
	}
	prevMousePosX = curMousePosX;
	prevMousePosY = curMousePosY;
	glfwGetCursorPos(window, &curMousePosX, &curMousePosY);
	curMousePosY *= -1;
}

bool MouseInput::buttonPressed(std::string_view buttonName) const
{
	auto it = buttonNameMap.find(std::string{buttonName});
	if(it != buttonNameMap.end())
	{
		const ButtonStatus& status = buttonStatusMap.at(it->second);
		return status.current;
	}
	else
	{
		throw std::invalid_argument{"The given buttonbind does not exist."};
	}
}

bool MouseInput::buttonJustPressed(std::string_view buttonName) const
{
	auto it = buttonNameMap.find(std::string{buttonName});
	if(it != buttonNameMap.end())
	{
		const ButtonStatus& status = buttonStatusMap.at(it->second);
		return status.current && !status.previous;
	}
	else
	{
		throw std::invalid_argument{"The given buttonbind does not exist."};
	}
}

bool MouseInput::buttonReleased(std::string_view buttonName) const
{
	auto it = buttonNameMap.find(std::string{buttonName});
	if(it != buttonNameMap.end())
	{
		const ButtonStatus& status = buttonStatusMap.at(it->second);
		return !status.current;
	}
	else
	{
		throw std::invalid_argument{"The given buttonbind does not exist."};
	}
}

bool MouseInput::buttonJustReleased(std::string_view buttonName) const
{
	auto it = buttonNameMap.find(std::string{buttonName});
	if(it != buttonNameMap.end())
	{
		const ButtonStatus& status = buttonStatusMap.at(it->second);
		return !status.current && status.previous;
	}
	else
	{
		throw std::invalid_argument{"The given buttonbind does not exist."};
	}
}

bool MouseInput::mouseMoved() const
{
	return ((prevMousePosX != curMousePosX) || (prevMousePosY != curMousePosY));
}

float MouseInput::getMouseMovementX() const
{
	return (float)(curMousePosX - prevMousePosX);
}

float MouseInput::getMouseMovementY() const
{
	return (float)(curMousePosY - prevMousePosY);
}

float MouseInput::getMousePositionX() const
{
	return (float)curMousePosX;
}

float MouseInput::getMousePositionY() const
{
	return (float)curMousePosY;
}