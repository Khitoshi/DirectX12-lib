#pragma once

#include "Scene.h"
#include <DirectXMath.h>
class Camera;
class CameraController;
class TextureCubeModel;

class SceneTexture3dCube final : public Scene
{
public:
	SceneTexture3dCube() :
		cube_model_(),
		camera_(),
		camera_angle_(DirectX::XMFLOAT3(0, 0, 0)),
		camera_controller_()
	{};
	~SceneTexture3dCube() {};

	void init(ID3D12Device* device)override;
	void finalize()override;
	void update()override;
	void render(RenderContext* rc)override;
	void updateImguiMenu(RenderContext* rc, ImGuiManager* igm)override;

private:
	std::shared_ptr<TextureCubeModel> cube_model_;
	std::shared_ptr<Camera> camera_;
	DirectX::XMFLOAT3 camera_angle_;
	std::shared_ptr<CameraController> camera_controller_;
};
