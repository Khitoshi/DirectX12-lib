#pragma once

#include "PipelineStateObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "d3dx12.h"
#include "Texture.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"
#include "CommonGraphicsConfig.h"
#include <dxgi1_4.h>

using namespace Microsoft::WRL;

/// <summary>
/// レンダーコンテキスト
/// コマンドリスト関係の処理をまとめたクラス
/// </summary>
class RenderContext
{
    friend class RenderContextFactory;
private:
    RenderContext(ID3D12GraphicsCommandList4* command_list, const D3D12_VIEWPORT& viewport) :
        command_list_(command_list),
        current_viewport_(viewport)
    {};
public:
    ~RenderContext() {};

    /// <summary>
    /// 新しいコマンドリストが生成されたときに呼び出す
    /// コマンドリストのリセット
    /// </summary>
    /// <param name="commandAllocator">デバイスがコマンドリストを作成する時の元になるアロケーター</param>
    /// <param name="pipelineState">nullptrでもokなパイプラインステート</param>
    void reset(ID3D12CommandAllocator* command_allocator, ID3D12PipelineState* pso)
    {
        if (FAILED(this->command_list_->Reset(command_allocator, pso))) {
            throw std::exception("コマンドリストのリセットに失敗しました。");
        }
    }

    /// <summary>
    /// コマンドリストを閉じる
    /// </summary>
    void close()
    {
        if (FAILED(this->command_list_->Close())) {
            throw std::exception("コマンドリストのクローズに失敗しました。");
        }
    }

    /// <summary>
    /// レンダーターゲットのクリア
    /// </summary>
    /// <param name="render_target_handle">レンダーターゲットのハンドル</param>
    /// <param name="clear_color">RGBA</param>
    void clearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE render_target_handle, const float clear_color[4])
    {
        this->command_list_->ClearRenderTargetView(render_target_handle, clear_color, 0, nullptr);
    }

    /// <summary>
    /// 深度ステンシルビューのクリア
    /// </summary>
    /// <param name="dsv_handle">深度ステンシルのハンドル</param>
    /// <param name="clear_value">クリア値</param>
    void clearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle, float clear_value)
    {
        this->command_list_->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clear_value, 0, 0, nullptr);
    }

    /// <summary>
    /// ビューポートの設定
    /// 3Dレンダリングされたシーンがウィンドウ上のどの部分に表示されるべきかを定義
    /// つまり、3Dワールドのどの部分が最終的に2D画面に投影されるかを決定している
    /// </summary>
    /// <param name="viewport">設定するビューポート</param>
    void setViewport(const D3D12_VIEWPORT& viewport)
    {
        this->command_list_->RSSetViewports(1, &viewport);
        this->current_viewport_ = viewport;
    }

    /// <summary>
    /// シザリング矩形の設定
    /// 実際にピクセルがレンダリングされる2Dレンダーターゲットの一部を定義
    /// </summary>
    /// <param name="viewport">ビューポートと同じサイズのシザリングを設置</param>
    void setScissorRect(const D3D12_VIEWPORT& viewport)
    {
        D3D12_RECT scissorRect = {};
        scissorRect.bottom = static_cast<LONG>(viewport.Height);
        scissorRect.top = 0;
        scissorRect.left = 0;
        scissorRect.right = static_cast<LONG>(viewport.Width);
        this->command_list_->RSSetScissorRects(1, &scissorRect);
    }

    /// <summary>
    /// レンダーターゲットの設定
    /// </summary>
    /// <param name="rtvHandle">レンダーターゲットのハンドル</param>
    /// <param name="dsvHandle">深度ステンシルビューのハンドル</param>
    void setRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle)
    {
        //レンダーターゲットの設定
        this->command_list_->OMSetRenderTargets(1, &rtv_handle, FALSE, &dsv_handle);
    }

    /// <summary>
    /// ルートパラメータに対応するディスクリプタテーブルを設定
    /// </summary>
    /// <param name="rootParamIndex"></param>
    /// <param name="handle"></param>
    void setGraphicsRootDescriptorTable(int root_param_index, D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        this->command_list_->SetGraphicsRootDescriptorTable(root_param_index, handle);
    }

    /// <summary>
    /// レンダーターゲット(フロントバッファ)の状態遷移を待つ
    /// TARGET -> PRESENT
    /// </summary>
    /// <param name="resouce">状態遷移させるレンダーターゲットのリソース</param>
    void transitionMainRenderTargetEnd(ID3D12Resource* resouce)
    {
        //リソースバリアで、グラフィックスパイプラインでリソースの使用方法が変更されるタイミングを制御
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        //バリアをコマンドリストに追加
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// レンダーターゲット(フロントバッファ)への描き込み待ち
    /// PRESENT -> TARGET
    /// に遷移
    /// </summary>
    /// <param name="resouce">状態遷移させるレンダーターゲットのリソース</param>
    void transitionMainRenderTargetBegin(ID3D12Resource* resouce) {
        //リソースバリアで、グラフィックスパイプラインでリソースの使用方法が変更されるタイミングを制御
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        //バリアをコマンドリストに追加
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// OffScreenRenderTargetへの描き込み開始
    /// PIXEL_SHADER_RESOURCE -> RENDER_TARGET
    /// </summary>
    /// <param name="resouce">状態遷移させるレンダーターゲットのリソース</param>
    void transitionOffScreenRenderTargetBegin(ID3D12Resource* resouce)
    {
        //リソースバリアで、グラフィックスパイプラインでリソースの使用方法が変更されるタイミングを制御
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        //バリアをコマンドリストに追加
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// OffScreenRenderTargetへの描き込み待ち
    /// RENDER_TARGET -> PIXEL_SHADER_RESOURCE
    /// </summary>
    /// <param name="resouce">状態遷移させるレンダーターゲットのリソース</param>
    void transitionOffScreenRenderTargetEnd(ID3D12Resource* resouce)
    {
        //リソースバリアで、グラフィックスパイプラインでリソースの使用方法が変更されるタイミングを制御
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resouce,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        //バリアをコマンドリストに追加
        this->barriers_.push_back(barrier);
    }

    /// <summary>
    /// リソースバリアの実行
    /// </summary>
    void executeResourceBarriers()
    {
        if (!this->barriers_.empty()) {
            this->command_list_->ResourceBarrier(static_cast<UINT>(this->barriers_.size()), this->barriers_.data());
            this->barriers_.clear();
        }
    }

    /// <summary>
    /// ディスクリプタヒープの登録
    /// </summary>
    /// <param name="descriptorHeap"></param>
    void setDescriptorHeap(ID3D12DescriptorHeap* descriptor_heap)
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = { descriptor_heap };
        this->command_list_->SetDescriptorHeaps(1, descriptorHeaps);
    }

    /// <summary>
    /// パイプラインステート登録
    /// </summary>
    /// <param name="pso"></param>
    void setPipelineState(PipelineStateObject* pso)
    {
        this->command_list_->SetPipelineState(pso->getPipelineStateObject());
    }

    /// <summary>
    /// プリミティブトポロジーの設定
    /// </summary>
    /// <param name="topology"></param>
    void setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
    {
        this->command_list_->IASetPrimitiveTopology(topology);
    }

    /// <summary>
    /// 頂点バッファを設定
    /// </summary>
    /// <param name="vb">頂点バッファ</param>
    void setVertexBuffer(VertexBuffer* vb)
    {
        this->command_list_->IASetVertexBuffers(0, 1, &vb->getVertexBufferView());
    }

    /// <summary>
    /// インデックスバッファを設定
    /// </summary>
    /// <param name="ib">インデックスバッファ</param>
    void setIndexBuffer(IndexBuffer* ib)
    {
        this->command_list_->IASetIndexBuffer(&ib->getIndexBufferView());
    }

    /// <summary>
    /// 定数バッファビューを設定
    /// </summary>
    /// <param name="cbv">定数バッファ</param>
    void setConstantBufferView(ConstantBuffer* cbv)
    {
        auto ds = cbv->getDescriptorHeap();
        this->command_list_->SetDescriptorHeaps(1, &ds);
        this->setGraphicsRootDescriptorTable(0, cbv->getGPUDescriptorHandleForHeapStart());
    }

    /// <summary>
    /// インデックス付きの描画コールを実行
    /// </summary>
    /// <param name="indexCount">インデックスの数</param>
    void drawIndexed(UINT indexCount) {
        this->command_list_->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
    }

    /// <summary>
    /// インデックスなしの描画コールを実行
    /// </summary>
    /// <param name="indexCount">インデックスの数</param>
    void drawInstanced(UINT index_count)
    {
        this->command_list_->DrawInstanced(index_count, 1, 0, 0);
    }

    /// <summary>
    /// ルートシグシグニチャを設定
    /// </summary>
    /// <param name="rootSignature">ルートシグニチャ</param>
    void setRootSignature(RootSignature* rootSignature) {
        this->command_list_->SetGraphicsRootSignature(rootSignature->getRootSignature());
    }

    /// <summary>
    /// テスクチャを設定
    /// </summary>
    /// <param name="texture"></param>
    void setTexture(Texture* texture) {
        auto ds = texture->GetDescriptorHeap();
        this->command_list_->SetDescriptorHeaps(1, &ds);
        this->setGraphicsRootDescriptorTable(0, ds->GetGPUDescriptorHandleForHeapStart());
    }
    void setTexture(ID3D12DescriptorHeap* ds, int ds_num) {
        this->command_list_->SetDescriptorHeaps(ds_num, &ds);
        this->setGraphicsRootDescriptorTable(0, ds->GetGPUDescriptorHandleForHeapStart());
    }

    /// <summary>
    /// 単純な描画開始処理
    /// </summary>
    /// <param name="rtvHandle">レンダーターゲットビューハンドル</param>
    /// <param name="dsvHandle">深度ステンシルビューハンドル</param>
    void simpleStart(D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle)
    {
        //リソースバリアの実行
        this->executeResourceBarriers();

        //レンダーターゲットを設定
        this->setRenderTarget(rtv_handle, dsv_handle);

        //ビューポートとシザー矩形を設定
        this->setViewport(this->current_viewport_);
        this->setScissorRect(this->current_viewport_);

        //レンダーターゲットのクリア
        this->clearRenderTarget(rtv_handle, backGroundColor);

        //深度ステンシルのクリア
        this->clearDepthStencil(dsv_handle, 1.0f);
    }

public://取得系
    ID3D12GraphicsCommandList4* getCommandList() { return this->command_list_; }//コマンドリストの取得

private:

    ID3D12GraphicsCommandList4* command_list_;        //コマンドリスト
    D3D12_VIEWPORT current_viewport_;				    //現在のビューポート

    std::vector<CD3DX12_RESOURCE_BARRIER> barriers_; //RESOURCE_BARRIERのリスト
};
