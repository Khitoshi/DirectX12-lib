#pragma once
#include "d3dx12.h"
#include "RenderContext.h"
//#include "OffScreenRenderTarget.h"

class CompositeRenderTarget
{
public:
    struct CompositeRenderTargetConf {
        D3D12_RESOURCE_DESC resourceDesc; 		    //バックバッファの設定
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;  //バックバッファで使用しているディスクリプタヒープの設定
        float clearColor[4];                            //生成時のクリアカラー
    };

public:
    CompositeRenderTarget(CompositeRenderTargetConf c) :
        conf(c),
        resource(),
        SRVHeap(),
        RTVHeap()
    {};
    ~CompositeRenderTarget() {};

    //初期化
    void init(ID3D12Device* device);
    void beginRender(RenderContext* rc, D3D12_VIEWPORT viewport, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle);
    void endRender(RenderContext* rc);

private:
    //リソースを作成
    void createResource(ID3D12Device* device);
    //シェーダーリソースビューのディスクリプタヒープを作成
    void createSRVHeap(ID3D12Device* device);
    //シェーダーリソースビューを作成
    void createShaderResourceView(ID3D12Device* device);
    //レンダーターゲットビューのディスクリプタヒープを作成
    void createRTVHeap(ID3D12Device* device);
    //レンダーターゲットビューを作成
    void createRenderTargetView(ID3D12Device* device);
public://設定系
    //void setDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE cdh) { this->depthStencilViewHandle = cdh; }       //深度ステンシルの設定
    //void setViewport(D3D12_VIEWPORT vp) { viewport = vp; }             //ビューポートの設定
public://取得系
    ID3D12Resource* getResource() const { return resource.Get(); }      //リソースの取得
    ID3D12DescriptorHeap* getSRVHeap() const { return SRVHeap.Get(); }  //シェーダーリソースビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getRTVHeap() const { return RTVHeap.Get(); }  //レンダーターゲットビューディスクリプタヒープの取得
private:
    CompositeRenderTargetConf conf;			            //設定
    ComPtr<ID3D12Resource> resource;		            //リソース
    ComPtr<ID3D12DescriptorHeap> SRVHeap;               //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> RTVHeap;               //レンダーターゲットビューディスクリプタヒープ

    //D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle; //深度ステンシルビューハンドル
    //D3D12_VIEWPORT viewport;                            //ビューポート

};
