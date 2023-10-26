#pragma once

#include "RenderContext.h"
#include<memory>
class GraphicsCommandList;

/// <summary>
/// レンダーコンテキストファクトリクラス
/// </summary>
class RenderContextFactory
{
public:
	static std::shared_ptr<RenderContext> create(ID3D12GraphicsCommandList4* command_list, const D3D12_VIEWPORT& viewport)
	{
		std::shared_ptr<RenderContext> render_context(new RenderContext(command_list, viewport));
		return render_context;
	}
};
