#pragma once

#include "RenderContext.h"
#include<memory>
class GraphicsCommandList;

class RenderContextFactory
{
public:
    /// <summary>
    /// �����_�[�R���e�L�X�g���쐬
    /// </summary>
    /// <param name="command_list">�R�Â��邷��R�}���h���X�g</param>
    /// <param name="viewport">�`��̈�</param>
    /// <returns>
    /// ����&���������ꂽ�����_�[�R���e�L�X�g
    /// </returns>
    static std::shared_ptr<RenderContext> create(ID3D12GraphicsCommandList4* command_list, const D3D12_VIEWPORT& viewport)
    {
        std::shared_ptr<RenderContext> render_context(new RenderContext(command_list, viewport));
        return render_context;
    }
};
