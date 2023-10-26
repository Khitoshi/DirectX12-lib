#pragma once
#include <Windows.h>
#include <vector>

class MouseInputListener;

class InputManager
{
private:
	InputManager() :
		mouse_input_listeners_()
	{};
	~InputManager() {};
public:
	static InputManager& Instance() {
		static InputManager instance;
		return instance;
	}

	void addMouseInputListener(MouseInputListener* listener);
	void OnMouseMove(int x, int y);

	void onMouseLeftButtonDown();
	void onMouseLeftButtonUp();
	void onMouseRightButtonDown();
	void onMouseRightButtonUp();

	void clearMouseInputListener() { mouse_input_listeners_.clear(); }

private:
	std::vector<MouseInputListener*> mouse_input_listeners_;
};
