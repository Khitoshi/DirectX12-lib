#pragma once

#include "Scene.h"

/// <summary>
/// ���̏��������Ȃ��V�[��
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
