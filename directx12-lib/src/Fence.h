#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// GPUと同期するオブジェクト生成クラス
/// </summary>
class Fence
{
public:
    Fence() :
        fence(),
        value(0),
        event()
    {};
    ~Fence() {};

    //初期化処理
    void init(ID3D12Device* device);

private:
    //フェンスの生成
    ComPtr<ID3D12Fence> createFence(ID3D12Device* device);

    //フェンスの値の生成
    UINT64 createValue();

    //フェンスのイベントの生成
    HANDLE createEvent();
public:
    ID3D12Fence* getFence() const { return fence.Get(); } //フェンスの取得
    UINT64 getValue() const { return value; }            //フェンスの値の取得
    HANDLE getEvent() const { return event; }            //フェンスのイベントの取得

    void incrementValue() { value++; }                   //フェンスの値のインクリメント
private:
    ComPtr<ID3D12Fence> fence;  //フェンス
    UINT64 value;               //フェンスの値
    HANDLE event;               //フェンスのイベント
};
