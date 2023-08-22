#include "CameraController.h"
#include "Camera.h"

void CameraController::onMouseMove(int x, int y)
{
    if (is_left_button_down_) {
        this->camera_->rotate(x, y);
    }
}

void CameraController::onMouseLeftButtonDown()
{
    this->is_left_button_down_ = true;
}

void CameraController::onMouseLeftButtonUp()
{
    this->is_left_button_down_ = false;
}

void CameraController::onMouseRightButtonDown()
{
    this->is_right_button_down_ = true;
}

void CameraController::onMouseRightButtonUp()
{
    this->is_right_button_down_ = false;
}

