#pragma once

#include "PipelineStateObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "d3dx12.h"
#include <dxgi1_4.h>
#include "RootSignature.h"
using namespace Microsoft::WRL;

/// <summary>
/// レンダーコンテキスト
/// コマンドリスト関係の処理をまとめたクラス
/// </summary>
class RenderContext
{
public:
    RenderContext() :
        commandList(),
        viewport(),
        currentViewport()
    {};
    ~RenderContext() {};

    /// <summary>
    /// DX12Resoucesのコマンドリストを参照する
    /// これをすることでrenderContextのコマンドの実行ができる
    /// </summary>
    /// <param name="commandList">生成済みのコマンドリスト</param>
    void init(ID3D12GraphicsCommandList4* commandList)
    {
        this->commandList = commandList;
    }

    /// <summary>
    /// 新しいコマンドリストが生成されたときに呼び出す
    /// コマンドリストのリセット
    /// </summary>
    /// <param name="commandAllocator">デバイスがコマンドリストを作成する時の元になるアロケーター</param>
    /// <param name="pipelineState">nullptrでもokなパイプラインステート</param>
    void reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState)
    {
        if (FAILED(this->commandList->Reset(commandAllocator, pipelineState))) {
            throw std::exception("コマンドリストのリセットに失敗しました。");
        }
    }

    /// <summary>
    /// コマンドリストを閉じる
    /// </summary>
    void close()
    {
        if (FAILED(this->commandList->Close())) {
            throw std::exception("コマンドリストのクローズに失敗しました。");
        }
    }

    /// <summary>
    /// レンダーターゲットのクリア
    /// </summary>
    /// <param name="renderTargetHandle">レンダーターゲットのハンドル</param>
    /// <param name="clearColor">RGBA</param>
    void clearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle, const float clearColor[4])
    {
        this->commandList->ClearRenderTargetView(renderTargetHandle, clearColor, 0, nullptr);
    }

    /// <summary>
    /// 深度ステンシルビューのクリア
    /// </summary>
    /// <param name="dsvHandle">深度ステンシルのハンドル</param>
    /// <param name="clearValue">クリア値</param>
    void clearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue)
    {
        this->commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearValue, 0, 0, nullptr);
    }

    /// <summary>
    /// ビューポートの設定
    /// 3Dレンダリングされたシーンがウィンドウ上のどの部分に表示されるべきかを定義
    /// つまり、3Dワールドのどの部分が最終的に2D画面に投影されるかを決定している
    /// </summary>
    /// <param name="viewport">設定するビューポート</param>
    void setViewport(const D3D12_VIEWPORT& viewport)
    {
        this->commandList->RSSetViewports(1, &viewport);
        currentViewport = viewport;
    }

    /// <summary>
    /// シザリング矩形の設定
    /// 実際にピクセルがレンダリングされる2Dレンダーターゲットの一部を定義
    /// </summary>
    /// <param name="viewport">ビューポートと同じサイズのシザリングを設置</param>
    void setScissorRect(const D3D12_VIEWPORT& viewport)
    {
        //シザリング矩形を設定
        D3D12_RECT scissorRect = {};
        scissorRect.bottom = static_cast<LONG>(viewport.Height);
        scissorRect.top = 0;
        scissorRect.left = 0;
        scissorRect.right = static_cast<LONG>(viewport.Width);
        //SetScissorRect(scissorRect);
        this->commandList->RSSetScissorRects(1, &scissorRect);
    }

    /// <summary>
    /// レンダーターゲットの設定
    /// </summary>
    /// <param name="rtvHandle">レンダーターゲットのハンドル</param>
    /// <param name="dsvHandle">深度ステンシルビューのハンドル</param>
    //void setRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
    void setRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
    {
        //レンダーターゲットの設定
        this->commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    }

    /// <summary>
    /// レンダーターゲットの状態遷移を待つ
    /// TARGET -> PRESENT
    /// </summary>
    /// <param name="renderTarget">状態遷移させるリソース</param>
    void TransitionRenderTargetToRender(ID3D12Resource* renderTarget)
    {
        //リソースバリアで、グラフィックスパイプラインでリソースの使用方法が変更されるタイミングを制御
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTarget,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        //バリアをコマンドリストに追加
        this->commandList->ResourceBarrier(1, &barrier);
    }

    /// <summary>
    /// レンダーターゲットへの描き込み待ち
    /// PRESENT -> TARGET
    /// に遷移
    /// </summary>
    /// <param name="renderTarget">状態遷移させるリソース</param>
    void TransitionRenderTargetToPresent(ID3D12Resource* renderTarget) {
        //リソースバリアで、グラフィックスパイプラインでリソースの使用方法が変更されるタイミングを制御
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTarget,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        //バリアをコマンドリストに追加
        this->commandList->ResourceBarrier(1, &barrier);
    }

    /// <summary>
    /// ディスクリプタヒープの登録
    /// </summary>
    /// <param name="descriptorHeap"></param>
    void setDescriptorHeap(ID3D12DescriptorHeap* descriptorHeap)
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap };
        this->commandList->SetDescriptorHeaps(1, descriptorHeaps);
    }

    /// <summary>
    /// パイプラインステート登録
    /// </summary>
    /// <param name="pso"></param>
    void setPipelineState(PipelineStateObject* pso)
    {
        this->commandList->SetPipelineState(pso->getPipelineStateObject());
    }


    /// <summary>
    /// プリミティブトポロジーの設定
    /// </summary>
    /// <param name="topology"></param>
    void setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
    {
        this->commandList->IASetPrimitiveTopology(topology);
    }


    /// <summary>
    /// 頂点バッファを設定
    /// </summary>
    /// <param name="vb">頂点バッファ</param>
    void setVertexBuffer(VertexBuffer* vb)
    {
        D3D12_VERTEX_BUFFER_VIEW vbView = vb->getVertexBufferView();
        this->commandList->IASetVertexBuffers(0, 1, &vbView);
    }

    /// <summary>
    /// インデックスバッファを設定
    /// </summary>
    /// <param name="ib">インデックスバッファ</param>
    void setIndexBuffer(IndexBuffer* ib)
    {
        D3D12_INDEX_BUFFER_VIEW ibView = ib->getIndexBufferView();
        this->commandList->IASetIndexBuffer(&ibView);
    }

    /// <summary>
    /// インデックス付きの描画コールを実行
    /// </summary>
    /// <param name="indexCount">インデックスの要素数</param>
    void drawIndexed(UINT indexCount) {
        this->commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
        //this->commandList->DrawInstanced(indexCount, 1, 0, 0);
    }

    /// <summary>
    /// ルートシグシグニチャを設定
    /// </summary>
    /// <param name="rootSignature">ルートシグニチャ</param>
    void setRootSignature(RootSignature* rootSignature) {
        this->commandList->SetGraphicsRootSignature(rootSignature->getRootSignature());
    }

public:
    ID3D12GraphicsCommandList4* getCommandList() { return this->commandList; }//コマンドリストの取得

private:
    enum { MAX_DESCRIPTOR_HEAP = 4 };	//ディスクリプタヒープの最大数
    enum { MAX_CONSTANT_BUFFER = 8 };	//定数バッファの最大数
    enum { MAX_SHADER_RESOURCE = 16 };	//シェーダーリソースの最大数

    ID3D12GraphicsCommandList4* commandList;                        //コマンドリスト
    D3D12_VIEWPORT viewport;                                        //現在のビューポート
    //std::vector< ComPtr<ID3D12Resource> > m_scratchResourceList;	//スクラッチリソースのリスト。
    D3D12_VIEWPORT currentViewport;				                    //現在のビューポート
};
