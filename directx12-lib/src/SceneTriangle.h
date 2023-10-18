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

	//初期化処理
	void init(ID3D12Device* device)override;
	//終了処理
	void finalize()override;
	//更新処理
	void update()override;
	//描画処理
	void render(RenderContext* rc)override;
	//imguiのmenuを表示
	void updateImguiMenu(RenderContext* rc, ImGuiManager* igm)override;
private:
	std::shared_ptr<Triangle> triangle_;    //三角形
	Triangle::Vertex vertex_[3];            //頂点座標
	bool is_change_vertex_;                 //頂点座標を変更したか
};
