#pragma once
//#include "d3dx12.h"
#include <vector>
#include <memory>
#include <concepts>

//シーンのインターフェース
template<typename T>
concept Scene = requires(T & value)
{
    {value.init()}      -> std::convertible_to<void>;//初期化処理
    {value.finalize()}  -> std::convertible_to<void>;//終了化処理
    {value.update()}    -> std::convertible_to<void>;//更新処理
    {value.render()}    -> std::convertible_to<void>;//描画処理
};


template<Scene T>
class SceneManager
{
private:
    SceneManager() {};
    ~SceneManager() {};
public:

    //シングルトン
    static SceneManager& getInstance()
    {
        static SceneManager instance;
        return instance;
    }

    //初期化処理
    void clear()
    {
        if (this->currentScene) {
            this->currentScene->finalize();
        }
    }

    //初期化処理
    void update()
    {
        if (currentScene) {
            currentScene->update();
        }
    }

    //描画処理
    void render()
    {
        if (currentScene) {
            currentScene->render();
        }
    }

    //変更処理
    void changeScene(T* scene)
    {
        if (scene) {
            currentScene->finalize();
            currentScene = scene;
            currentScene->init();
        }
    }
private:
    //現在のシーン
    T* currentScene;
};

