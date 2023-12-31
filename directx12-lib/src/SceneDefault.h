#pragma once

#include "Scene.h"

/// <summary>
/// 何の処理もしないシーン
/// </summary>
class SceneDefault final : public Scene
{
public:
	SceneDefault() {};
	~SceneDefault() {};

	void init(ID3D12Device* device)override {};
	void finalize()override {};
	void update()override {};
	void render(RenderContext* rc)override {};
};
