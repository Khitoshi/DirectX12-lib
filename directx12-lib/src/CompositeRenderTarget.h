#pragma once
#include "d3dx12.h"
#include "PipelineStateObject.h"
#include "VertexBuffer.h"
#include "RenderContext.h"
#include "RootSignature.h"
#include "Shader.h"
#include "DescriptorHeapCache.h"

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
        RTVHeap(),
        srvDesc(),
        descriptorHeapCache()
    {};
    ~CompositeRenderTarget() {};

    //初期化
    void init(ID3D12Device* device);
    void beginRender(RenderContext* rc, D3D12_VIEWPORT viewport, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle);
    void render(RenderContext* rc, ID3D12Device* device);
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

    //ルートシグネチャの作成
    void initRootSignature(ID3D12Device* device);
    //シェーダーのロード
    void initShader(ID3D12Device* device);
    //パイプラインステートオブジェクトの作成
    void initPipelineStateObject(ID3D12Device* device);

    //頂点バッファの作成
    void initVertexBuffer(ID3D12Device* device);

public://設定系
    //void setDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE cdh) { this->depthStencilViewHandle = cdh; }       //深度ステンシルの設定
    //void setViewport(D3D12_VIEWPORT vp) { viewport = vp; }             //ビューポートの設定
public://取得系
    ID3D12Resource* getResource() const { return resource.Get(); }      //リソースの取得
    ID3D12DescriptorHeap* getSRVHeap() const { return SRVHeap.Get(); }  //シェーダーリソースビューディスクリプタヒープの取得
    ID3D12DescriptorHeap* getRTVHeap() const { return RTVHeap.Get(); }  //レンダーターゲットビューディスクリプタヒープの取得
private:
    //頂点データ
    struct Vertex
    {
        DirectX::XMFLOAT3 position;	//座標
        DirectX::XMFLOAT2 uv;		//テクスチャ座標
    };

    CompositeRenderTargetConf conf;			            //設定
    ComPtr<ID3D12Resource> resource;		            //リソース
    ComPtr<ID3D12DescriptorHeap> SRVHeap;               //シェーダーリソースビューディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> RTVHeap;               //レンダーターゲットビューディスクリプタヒープ

    std::shared_ptr<PipelineStateObject> pso;           //パイプラインステートオブジェクト
    std::shared_ptr<VertexBuffer> vb;                   //頂点バッファ
    std::shared_ptr<RootSignature> rootSignature;       //ルートシグニチャ
    BasicShaderPair shaderPair;                         //シェーダーペア
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    std::shared_ptr<DescriptorHeapCache> descriptorHeapCache;       //ディスクリプタヒープキャッシュマネージャー
};
