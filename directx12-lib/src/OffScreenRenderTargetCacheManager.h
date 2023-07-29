#pragma once
#include "OffScreenRenderTargetFactory.h"

#include <vector>
#include <stdexcept>

/// <summary>
/// オフスクリーンレンダーターゲットのキャッシュ管理システム
/// </summary>
class OffScreenRenderTargetCacheManager
{
private:
    OffScreenRenderTargetCacheManager() :
        off_screen_Render_target_list_()
    {};
    ~OffScreenRenderTargetCacheManager() {};

public:
    /// <summary>
    /// シングルトンなインスタンスを取得
    /// </summary>
    /// <returns>シングルトンなインスタンス</returns>
    static OffScreenRenderTargetCacheManager& getInstance()
    {
        static OffScreenRenderTargetCacheManager instance;
        return instance;
    }

    /// <summary>
    /// 描画予定のレンダーターゲットリストを取得
    /// </summary>
    /// <returns>レンダーターゲットリスト</returns>
    std::vector<OffScreenRenderTarget*> getRenderTargetList() const
    {
        return this->off_screen_Render_target_list_;
    }

    /// <summary>
    /// レンダーターゲットリストの追加
    /// 後ろから追加される
    /// </summary>
    /// <param name="ort">オフスクリーンレンダーターゲット</param>
    void addRenderTargetList(OffScreenRenderTarget* ort)
    {
        this->off_screen_Render_target_list_.push_back(ort);
    }

    /// <summary>
    /// レンダーターゲットリストをクリア
    /// </summary>
    void clearRenderTargetList()
    {
        this->off_screen_Render_target_list_.clear();
    }

private:
    std::vector<OffScreenRenderTarget*> off_screen_Render_target_list_;  //レンダーターゲットリスト
};
