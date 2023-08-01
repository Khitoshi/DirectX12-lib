#pragma once
#include "d3dx12.h"

class RenderContext;
class DepthStencil;

using namespace Microsoft::WRL;

/// <summary>
/// マルチパスレンダリング用のオフスクリーンレンダーターゲット
/// </summary>
class OffScreenRenderTarget
{
    friend class OffScreenRenderTargetFactory;
public:
    /// <summary>
    /// 生成に使用する設定
    /// </summary>
    struct OffScreenRenderTargetConf {
        D3D12_RESOURCE_DESC resource_desc; 		            //バックバッファの設定
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;    //バックバッファで使用しているディスクリプタヒープの設定

        bool operator==(const OffScreenRenderTargetConf& conf) const {
            return
                resource_desc.Width == conf.resource_desc.Width &&
                resource_desc.Height == conf.resource_desc.Height &&
                resource_desc.Format == conf.resource_desc.Format &&
                resource_desc.DepthOrArraySize == conf.resource_desc.DepthOrArraySize &&
                resource_desc.MipLevels == conf.resource_desc.MipLevels &&
                resource_desc.SampleDesc.Count == conf.resource_desc.SampleDesc.Count &&
                resource_desc.SampleDesc.Quality == conf.resource_desc.SampleDesc.Quality &&
                resource_desc.Dimension == conf.resource_desc.Dimension &&
                resource_desc.Layout == conf.resource_desc.Layout &&
                resource_desc.Flags == conf.resource_desc.Flags &&

                descriptor_heap_desc.NumDescriptors == conf.descriptor_heap_desc.NumDescriptors &&
                descriptor_heap_desc.Type == conf.descriptor_heap_desc.Type;
        }
    };

private:
    OffScreenRenderTarget(OffScreenRenderTargetConf c) :
        conf_(c),
        resource_(),
        srv_heap_(),
        rtv_heap_(),
        depth_stencil_view_handle_(),
        viewport_()
    {};

public:
    ~OffScreenRenderTarget() {};

    //レンダリング開始処理
    void beginRender(RenderContext* rc);
    //レンダリング終了処理
    void endRender(RenderContext* rc);

private:
    //初期化
    void init(ID3D12Device* device);
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
    //深度ステンシルの設定
    void setDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE cdh) { this->depth_stencil_view_handle_ = cdh; }
    //ビューポートの設定
    void setViewport(const D3D12_VIEWPORT vp) { this->viewport_ = vp; }

public://取得系
    //リソースの取得
    ID3D12Resource* getResource()       const { return this->resource_.Get(); }
    //シェーダーリソースビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getSRVHeap()  const { return this->srv_heap_.Get(); }
    //レンダーターゲットビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getRTVHeap()  const { return this->rtv_heap_.Get(); }

private:
    OffScreenRenderTargetConf conf_;			            //設定
    ComPtr<ID3D12Resource> resource_;		                //リソース
    ComPtr<ID3D12DescriptorHeap> srv_heap_;                 //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> rtv_heap_;                 //レンダーターゲットビューディスクリプタヒープ

    D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_view_handle_; //深度ステンシルビューハンドル
    D3D12_VIEWPORT viewport_;                               //ビューポート
};
