#include "InputManager.h"
#include "MouseInputListener.h"

void InputManager::addMouseInputListener(MouseInputListener* listener)
{
	auto itr = std::find(mouse_input_listeners_.begin(), mouse_input_listeners_.end(), listener);
	if (itr != mouse_input_listeners_.end()) {
		return;
	}

	this->mouse_input_listeners_.push_back(listener);
}

void InputManager::OnMouseMove(int x, int y)
{
	for (auto& mouse_listener : this->mouse_input_listeners_) {
		mouse_listener->onMouseMove(x, y);
	}
}

void InputManager::onMouseLeftButtonDown()
{
	for (auto& mouse_listener : this->mouse_input_listeners_) {
		mouse_listener->onMouseLeftButtonDown();
	}
}

void InputManager::onMouseLeftButtonUp()
{
	for (auto& mouse_listener : this->mouse_input_listeners_) {
		mouse_listener->onMouseLeftButtonUp();
	}
}

void InputManager::onMouseRightButtonDown()
{
	for (auto& mouse_listener : this->mouse_input_listeners_) {
		mouse_listener->onMouseRightButtonDown();
	}
}

void InputManager::onMouseRightButtonUp()
{
	for (auto& mouse_listener : this->mouse_input_listeners_) {
		mouse_listener->onMouseRightButtonUp();
	}
}
