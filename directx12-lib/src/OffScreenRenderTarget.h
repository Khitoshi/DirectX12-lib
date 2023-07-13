#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// マルチパスレンダリング用のオフスクリーンレンダーターゲット
/// </summary>
class OffScreenRenderTarget
{
public:
    struct OffScreenRenderTargetConf {
        D3D12_RESOURCE_DESC resourceDesc; 		    //バックバッファの設定   
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;  //バックバッファで使用しているディスクリプタヒープの設定
        float clearColor[4];                            //生成時のクリアカラー
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
public:
    ID3D12Resource* getResource() const { return resource.Get(); }      //リソースの取得
    ID3D12DescriptorHeap* getSRVHeap() const { return SRVHeap.Get(); }  //シェーダーリソースビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getRTVHeap() const { return RTVHeap.Get(); }  //レンダーターゲットビューディスクリプタヒープの取得
private:
    OffScreenRenderTargetConf conf;			//設定
    ComPtr<ID3D12Resource> resource;		//リソース
    ComPtr<ID3D12DescriptorHeap> SRVHeap;   //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> RTVHeap;   //レンダーターゲットビューディスクリプタヒープ
};
