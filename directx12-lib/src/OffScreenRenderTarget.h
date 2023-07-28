#pragma once
#include "d3dx12.h"
#include "RenderContext.h"
#include "DepthStencil.h"

using namespace Microsoft::WRL;

/// <summary>
/// マルチパスレンダリング用のオフスクリーンレンダーターゲット
/// </summary>
class OffScreenRenderTarget
{
public:
    struct OffScreenRenderTargetConf {
        D3D12_RESOURCE_DESC resourceDesc; 		        //バックバッファの設定
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;  //バックバッファで使用しているディスクリプタヒープの設定

        bool operator==(const OffScreenRenderTargetConf& conf) const {
            return
                resourceDesc.Width == conf.resourceDesc.Width &&
                resourceDesc.Height == conf.resourceDesc.Height &&
                resourceDesc.Format == conf.resourceDesc.Format &&
                resourceDesc.DepthOrArraySize == conf.resourceDesc.DepthOrArraySize &&
                resourceDesc.MipLevels == conf.resourceDesc.MipLevels &&
                resourceDesc.SampleDesc.Count == conf.resourceDesc.SampleDesc.Count &&
                resourceDesc.SampleDesc.Quality == conf.resourceDesc.SampleDesc.Quality &&
                resourceDesc.Dimension == conf.resourceDesc.Dimension &&
                resourceDesc.Layout == conf.resourceDesc.Layout &&
                resourceDesc.Flags == conf.resourceDesc.Flags &&

                descriptorHeapDesc.NumDescriptors == conf.descriptorHeapDesc.NumDescriptors &&
                descriptorHeapDesc.Type == conf.descriptorHeapDesc.Type;
        }
    };

public:
    OffScreenRenderTarget(OffScreenRenderTargetConf c) :
        conf(c),
        resource(),
        SRVHeap(),
        RTVHeap()
    {};
    ~OffScreenRenderTarget() {};

    //初期化
    void init(ID3D12Device* device);
    //レンダリング開始処理
    void beginRender(RenderContext* rc);
    //レンダリング終了処理
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
    void setDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE cdh) { this->depthStencilViewHandle = cdh; } //深度ステンシルの設定
    void setViewport(D3D12_VIEWPORT vp) { viewport = vp; }  //ビューポートの設定
public://取得系
    ID3D12Resource* getResource() const { return resource.Get(); }      //リソースの取得
    ID3D12DescriptorHeap* getSRVHeap() const { return SRVHeap.Get(); }  //シェーダーリソースビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getRTVHeap() const { return RTVHeap.Get(); }  //レンダーターゲットビューディスクリプタヒープの取得
private:
    OffScreenRenderTargetConf conf;			//設定
    ComPtr<ID3D12Resource> resource;		//リソース
    ComPtr<ID3D12DescriptorHeap> SRVHeap;   //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> RTVHeap;   //レンダーターゲットビューディスクリプタヒープ

    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle; //深度ステンシルビューハンドル
    D3D12_VIEWPORT viewport;                            //ビューポート

};
