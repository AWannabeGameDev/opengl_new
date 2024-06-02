#include "utility/keyboard_input.h"

KeyboardInput::KeyboardInput(GLFWwindow* glfwWindow) : 
	keyNameMap{}, keyStatusMap{}, window{glfwWindow}
{}

void KeyboardInput::setKeybind(std::string_view keyName, int glfwKey)
{
	keyNameMap[std::string{keyName}] = glfwKey;
	keyStatusMap[glfwKey] = {false, false};
}

void KeyboardInput::removeKeybind(std::string_view keyName)
{
	auto it = keyNameMap.find(std::string{keyName});
	if(it != keyNameMap.end())
	{
		keyStatusMap.erase(it->second);
		keyNameMap.erase(it);
	}
}

void KeyboardInput::update()
{
	for(auto& [key, status] : keyStatusMap)
	{
		status.previous = status.current;
		if(glfwGetKey(window, key) == GLFW_PRESS)
			status.current = true;
		else
			status.current = false;
	}
}

bool KeyboardInput::keyJustPressed(std::string_view keyName) const
{
	auto it = keyNameMap.find(std::string{keyName});
	if(it != keyNameMap.end())
	{
		const KeyStatus& status = keyStatusMap.at(it->second);
		return status.current && !status.previous;
	}
	else
	{
		return false;
	}
}

bool KeyboardInput::keyPressed(std::string_view keyName) const
{
	auto it = keyNameMap.find(std::string{keyName});
	if(it != keyNameMap.end())
	{
		const KeyStatus& status = keyStatusMap.at(it->second);
		return status.current;
	}
	else
	{
		return false;
	}
}

bool KeyboardInput::keyJustReleased(std::string_view keyName) const
{
	auto it = keyNameMap.find(std::string{keyName});
	if(it != keyNameMap.end())
	{
		const KeyStatus& status = keyStatusMap.at(it->second);
		return !status.current && status.previous;
	}
	else
	{
		return false;
	}
}

bool KeyboardInput::keyReleased(std::string_view keyName) const
{
	auto it = keyNameMap.find(std::string{keyName});
	if(it != keyNameMap.end())
	{
		const KeyStatus& status = keyStatusMap.at(it->second);
		return !status.current;
	}
	else
	{
		return false;
	}
}

bool KeyboardInput::anyKeyJustPressed() const
{
	for(auto& [key, status] : keyStatusMap)
	{
		if(status.current && !status.previous) 
			return true;
	}
	return false;
}

bool KeyboardInput::anyKeyPressed() const
{
	for(auto& [key, status] : keyStatusMap)
	{
		if(status.current)
			return true;
	}
	return false;
}

bool KeyboardInput::anyKeyJustReleased() const
{
	for(auto& [key, status] : keyStatusMap)
	{
		if(!status.current && status.previous)
			return true;
	}
	return false;
}

bool KeyboardInput::anyKeyReleased() const
{
	for(auto& [key, status] : keyStatusMap)
	{
		if(!status.current)
			return true;
	}
	return false;
}