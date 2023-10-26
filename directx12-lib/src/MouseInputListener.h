#pragma once

class MouseInputListener
{
public:
	MouseInputListener() {};
	~MouseInputListener() {};

	virtual void onMouseMove(int x, int y) = 0;
	virtual void onMouseLeftButtonDown() = 0;
	virtual void onMouseLeftButtonUp() = 0;
	virtual void onMouseRightButtonDown() = 0;
	virtual void onMouseRightButtonUp() = 0;
};
