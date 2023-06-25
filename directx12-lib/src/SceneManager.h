#pragma once
//#include "d3dx12.h"
#include <vector>
#include <memory>
#include <concepts>
#include "Scene.h"


/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager
{
private:
    SceneManager() :currentScene() {};
    ~SceneManager() {};
public:
    /// <summary>
    /// シングルトンなインスタンスを取得
    /// </summary>
    /// <returns></returns>
    static SceneManager& getInstance()
    {
        static SceneManager instance;
        return instance;
    }

    /// <summary>
    /// 初期化処理
    /// </summary>
    void clear()
    {
        if (this->currentScene) {
            this->currentScene->finalize();
        }
    }

    /// <summary>
    /// 初期化処理
    /// </summary>
    void update()
    {
        if (currentScene) {
            currentScene->update();
        }
    }

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="conf"></param>
    void render(SceneConf conf)
    {
        if (currentScene) {
            currentScene->render(conf);
        }
    }

    /// <summary>
    /// シーン変更処理
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="conf"></param>
    template<class T>
    void changeScene(SceneConf conf)
    {

        clear();
        currentScene = std::make_shared<T>();
        currentScene->init(conf);

    }

private:
    std::shared_ptr<Scene> currentScene = nullptr;//現在のシーン
};

