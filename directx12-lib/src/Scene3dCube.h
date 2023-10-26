#pragma once
#pragma once

#include "Scene.h"
#include <memory>
#include <DirectXMath.h>

class CubeModel;
class Camera;
class CameraController;

/// <summary>
/// âΩÇÃèàóùÇ‡ÇµÇ»Ç¢ÉVÅ[Éì
/// </summary>
class Scene3dCube final : public Scene
{
public:
	Scene3dCube() :
		cube_model_(),
		camera_(),
		is_change_camera_(false),
		camera_angle_(DirectX::XMFLOAT3(0, 0, 0)),
		camera_controller_()
	{};
	~Scene3dCube() {};

	void init(ID3D12Device* device)override;
	void finalize()override;
	void update()override;
	void render(RenderContext* rc)override;
	void updateImguiMenu(RenderContext* rc, ImGuiManager* igm)override;

private:
	std::shared_ptr<CubeModel> cube_model_;
	std::shared_ptr<Camera> camera_;
	bool is_change_camera_;
	DirectX::XMFLOAT3 camera_angle_;
	std::shared_ptr<CameraController> camera_controller_;
};
