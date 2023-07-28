#pragma once

#include "RenderContext.h"
#include<memory>
class GraphicsCommandList;

class RenderContextFactory
{
public:
    /// <summary>
    /// レンダーコンテキストを作成
    /// </summary>
    /// <param name="command_list">紐づけるするコマンドリスト</param>
    /// <param name="viewport">描画領域</param>
    /// <returns>
    /// 生成&初期化されたレンダーコンテキスト
    /// </returns>
    static std::shared_ptr<RenderContext> create(ID3D12GraphicsCommandList4* command_list, const D3D12_VIEWPORT& viewport)
    {
        std::shared_ptr<RenderContext> render_context(new RenderContext(command_list, viewport));
        return render_context;
    }
};
