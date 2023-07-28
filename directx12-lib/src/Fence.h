#pragma once
#include "d3dx12.h"

using namespace Microsoft::WRL;

/// <summary>
/// GPUと同期するオブジェクト生成クラス
/// </summary>
class Fence
{
    friend class FenceFactory;
private:
    Fence() :
        fence_(),
        value_(0),
        event_()
    {};

public:
    ~Fence() {};

private:
    //初期化処理
    void init(ID3D12Device* device);
    //フェンスの生成
    void createFence(ID3D12Device* device);
    //フェンスの値の生成
    void createValue();
    //フェンスのイベントの生成
    void createEvent();

public://取得系
    //フェンスの取得
    ID3D12Fence* getFence() const { return this->fence_.Get(); }
    //フェンスの値の取得
    UINT64 getValue() const { return this->value_; }
    //フェンスのイベントの取得
    HANDLE getEvent() const { return this->event_; }

    //フェンスの値のインクリメント
    void incrementValue() { this->value_++; }
private:
    ComPtr<ID3D12Fence> fence_;  //フェンス
    UINT64 value_;               //フェンスの値
    HANDLE event_;               //フェンスのイベント
};
