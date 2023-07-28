#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "d3dx12.h"

class   RootSignature;
class   RenderContext;
class   VertexBuffer;
class   PipelineStateObject;
class   DescriptorHeapCache;
class   Shader;

using Microsoft::WRL::ComPtr;

/// <summary>
/// オフスクリーンを1枚のテクスチャに合成するクラス
/// </summary>
class CompositeRenderTarget
{
    friend class CompositeRenderTargetFactory;
public:
    //初期化時に必要な情報
    struct CompositeRenderTargetConf {
        D3D12_RESOURCE_DESC resource_desc; 		            //バックバッファの設定
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;    //バックバッファで使用しているディスクリプタヒープの設定
    };

private:
    CompositeRenderTarget(CompositeRenderTargetConf c) :
        conf_(c),
        resource_(),
        srv_heap_(),
        rtv_heap_(),
        pso_(),
        vertex_buffer_(),
        root_signature_(),
        pixel_shader_(),
        vertex_shader_(),
        srv_desc_(),
        descriptor_heap_cache_()
    {};
public:
    ~CompositeRenderTarget() {};

    //描画初期処理
    void beginRender(RenderContext* rc, D3D12_CPU_DESCRIPTOR_HANDLE depthStencil_view_handle);
    //描画処理
    void render(RenderContext* rc, ID3D12Device* device);
    //描画終了処理
    void endRender(RenderContext* rc);

private:
    //初期化
    void init(ID3D12Device* device);
    //リソースを作成
    void createResource(ID3D12Device* device);
    //シェーダーリソースビューのディスクリプタヒープを作成
    void createSRVHeap(ID3D12Device* device);
    //シェーダーリソースビューを作成
    void createSRVDesc(ID3D12Device* device);
    //レンダーターゲットビューのディスクリプタヒープを作成
    void createRTVHeap(ID3D12Device* device);
    //レンダーターゲットビューを作成
    void createRTV(ID3D12Device* device);

    //ルートシグネチャの作成
    void initRootSignature(ID3D12Device* device);
    //シェーダーのロード
    void initShader(ID3D12Device* device);
    //パイプラインステートオブジェクトの作成
    void initPipelineStateObject(ID3D12Device* device);
    //頂点バッファの作成
    void initVertexBuffer(ID3D12Device* device);
    //ディスクリプタヒープキャッシュマネージャーの作成
    void initDescriptorHeapCache();

public://取得系
    //リソースの取得
    ID3D12Resource* getResource() const { return resource_.Get(); }

    //シェーダーリソースビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getSRVHeap() const { return srv_heap_.Get(); }

    //レンダーターゲットビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getRTVHeap() const { return rtv_heap_.Get(); }
private:
    //頂点データ
    struct Vertex
    {
        DirectX::XMFLOAT3 position;	//座標
        DirectX::XMFLOAT2 uv;		//テクスチャ座標
    };

    CompositeRenderTargetConf conf_;			                    //クラスの設定

    ComPtr<ID3D12Resource> resource_;		                        //リソース
    ComPtr<ID3D12DescriptorHeap> srv_heap_;                         //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> rtv_heap_;                         //レンダーターゲットビューディスクリプタヒープ

    std::shared_ptr<PipelineStateObject> pso_;                      //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vertex_buffer_;                   //頂点バッファ
    std::shared_ptr<RootSignature> root_signature_;                 //ルートシグニチャ
    std::shared_ptr<Shader> pixel_shader_;                          //ピクセルシェーダー
    std::shared_ptr<Shader> vertex_shader_;                         //頂点シェーダー
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_;                      //シェーダーリソースビューの設定

    std::shared_ptr<DescriptorHeapCache> descriptor_heap_cache_;    //ディスクリプタヒープキャッシュマネージャー
};
