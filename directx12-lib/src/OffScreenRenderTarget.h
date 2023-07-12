#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

class OffScreenRenderTarget
{
public:
    struct OffScreenRenderTargetConf {
        D3D12_RESOURCE_DESC backBufferDesc;
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
        float clearColor[4];
    };

public:
    OffScreenRenderTarget(OffScreenRenderTargetConf c) :conf(c), resource() {};
    ~OffScreenRenderTarget() {};

    void init(ID3D12Device* device);
private:
    //リソースを作成
    void createResource(ID3D12Device* device);
    //ディスクリプタヒープのサイズを作成
    void createDescriptorHeapSize(ID3D12Device* device);
    //シェーダーリソースビューを作成
    void createShaderResourceView(ID3D12Device* device);
    void createRenderTargetView(ID3D12Device* device);
public:
    ID3D12Resource* getResource() const { return resource.Get(); }  				    //リソースの取得
    //ID3D12DescriptorHeap* getDescriptorHeap() const { return descriptorHeap.Get(); }    //ディスクリプタヒープの取得
    int getDescriptorHeapSize() const { return descriptorHeapSize; }                    //ディスクリプタヒープのサイズの取得
    ID3D12DescriptorHeap* getSRVHeap() const { return SRVHeap.Get(); }                   //シェーダーリソースビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getRTVHeap() const { return RTVHeap.Get(); }                   //レンダーターゲットビューディスクリプタヒープの取得
private:
    OffScreenRenderTargetConf conf;				    //設定
    ComPtr<ID3D12Resource> resource;				//リソース
    //ComPtr<ID3D12DescriptorHeap> descriptorHeap;    //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> SRVHeap;           //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> RTVHeap;           //レンダーターゲットビューディスクリプタヒープ
    int descriptorHeapSize;                         //ディスクリプタヒープのサイズ
};
