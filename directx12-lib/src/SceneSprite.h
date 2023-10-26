#pragma once

#include "Scene.h"
#include "Sprite.h"
#include <memory>

class Camera;

/// <summary>
/// スプライト描画用シーン
/// </summary>
class SceneSprite final : public Scene
{
public:
	SceneSprite() :
		sprite_(),
		vertex_(),
		is_change_vertex_(false),
		device_(nullptr),
		is_change_texture_(false)
	{};
	~SceneSprite() {};

	void init(ID3D12Device* device)override;
	void finalize()override;
	void update()override;
	void render(RenderContext* rc)override;
	void updateImguiMenu(RenderContext* rc, ImGuiManager* igm)override;

private:
	std::shared_ptr<Sprite> sprite_;
	Sprite::Vertex vertex_[4];
	bool is_change_vertex_;

	ID3D12Device* device_;

	static std::string file_path_;
	bool is_change_texture_;
};
