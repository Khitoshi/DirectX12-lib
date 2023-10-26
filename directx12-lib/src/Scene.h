#pragma once

#include "d3dx12.h"

class RenderContext;
class ImGuiManager;

/// <summary>
/// ‰¼‘z“I‚Èscene
/// </summary>
class Scene
{
public:
	Scene() {};
	virtual ~Scene() {};
	virtual void init(ID3D12Device* device) = 0;
	virtual void finalize() = 0;
	virtual void update() = 0;
	virtual void render(RenderContext* rc) = 0;
	virtual void updateImguiMenu(RenderContext* rc, ImGuiManager* igm) {};
};
