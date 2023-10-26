#pragma once
#include "Scene.h"
#include "Triangle.h"

/// <summary>
/// 三角形描画用シーン
/// </summary>
class SceneTriangle final : public Scene
{
public:
	SceneTriangle() :
		triangle_(nullptr),
		vertex_(),
		is_change_vertex_(false)
	{};
	~SceneTriangle() {};

	void init(ID3D12Device* device)override;
	void finalize()override;
	void update()override;
	void render(RenderContext* rc)override;
	void updateImguiMenu(RenderContext* rc, ImGuiManager* igm)override;

private:
	std::shared_ptr<Triangle> triangle_;
	Triangle::Vertex vertex_[3];
	bool is_change_vertex_;
};
