#pragma once
#include <string>
#include <memory>
#include <map>

/// <summary>
/// リソースマネージャー
/// 作成したリソースを登録して
/// エラーが発生した場合や
/// アプリケーションが終了場合
/// 全て解放する
/// </summary>
class ResourceManager
{
private:
    ResourceManager() {};
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
public:

    /// <summary>
    /// インスタンスの取得
    /// シングルトンで作成されているため
    /// グローバルで使用可能
    /// </summary>
    /// <returns></returns>
    static ResourceManager* getInstance() {
        static ResourceManager instance;
        return &instance;
    }

    /// <summary>
    /// リソースの登録
    /// エラーや終了時に解放するため
    /// </summary>
    /// <typeparam name="T">管理したいリソースの型</typeparam>
    /// <param name="name">管理用の名前</param>
    /// <param name="resource">リソース本体</param>
    template <typename T>
    void registerResource(const std::string& name, std::shared_ptr<T> resource) {
        //名前で検索
        auto it = resources.find(name);
        //既に同じ名前で登録されていないか確認
        if (it != resources.end()) {
            //登録済みの場合はエラー
            throw std::runtime_error("Resource already exists: " + name);
            return;
        }

        //マルチスレッド用のロック
        //std::lock_guard<std::mutex> lock(mtx);

        //登録
        resources[name] = std::move(resource);
    }

    /// <summary>
    /// リソースの取得
    /// </summary>
    /// <param name="name">リソースの管理用名前</param>
    /// <param name="resource">リソース本体</param>
    template<typename T>
    std::shared_ptr<T> getResource(const std::string& name) {
        //マルチスレッド用のロック
        //std::lock_guard<std::mutex> lock(mtx);

        //名前で検索
        auto it = resources.find(name);
        if (it != resources.end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        throw std::runtime_error("Resource not found: " + name);
    }

private:
    //リソースの管理用
    std::map<std::string, std::shared_ptr<void>> resources;
    //マルチスレッド用のロック
    //std::mutex mtx;
};
