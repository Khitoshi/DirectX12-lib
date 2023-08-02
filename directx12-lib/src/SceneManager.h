#pragma once
//#include "d3dx12.h"
#include <vector>
#include <memory>
#include <concepts>
#include "Scene.h"
#include "imgui/imgui.h"
//#include "SceneDefault.h"
#include "SceneTriangle.h"
#include "SceneSprite.h"
#include "Scene3dCube.h"
#include <functional>

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager
{
private:
    SceneManager() :currentScene(std::make_shared<SceneTriangle>()) {};
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

    void init(ID3D12Device* device, std::shared_ptr<Scene> scene)
    {
        registerScene();
        this->currentScene = scene;
        if (this->currentScene) {
            this->currentScene->init(device);
        }
    }

    /// <summary>
    /// 更新処理
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
    void render(RenderContext* rc)
    {
        if (currentScene) {
            currentScene->render(rc);
        }
    }

    /// <summary>
    /// シーン変更処理
    /// </summary>
    /// <param name="conf">描画に必要な設定</param>
    void changeScene(ID3D12Device* device)
    {
        if (!isSceneChange) return;

        //同じシーンの場合は変更しない
        if (typeid(*currentScene) == typeid(*nextScene)) {
            isSceneChange = false;
            return;
        }

        //シーン変更
        clear();
        currentScene = std::move(this->nextScene);
        currentScene->init(device);
        isSceneChange = false;
    }

    /// <summary>
    /// シーン選択
    /// </summary>
    void sceneSelect()
    {
        ImGui::Begin("Scene Manager");
        //シーン選択loop
        ImGui::Text("SCENE LIST");
        for (auto& [name, factory] : sceneFactories)
        {
            if (ImGui::Button(name.c_str())) {
                this->nextScene = factory();
                this->isSceneChange = true;
            }
        }

        ImGui::End();
    }

    /// <summary>
    /// シーン登録
    /// </summary>
    /// <param name="scene"></param>
    void registerScene()
    {
        sceneFactories = {
            //シーンを追加する場合はここに追加
            //{"Default",     []() { return std::make_shared<SceneDefault>(); }},
            {"Triangle",    []() { return std::make_shared<SceneTriangle>(); }},
            {"Sprite",      []() { return std::make_shared<SceneSprite>(); }},
            {"3dCube",      []() { return std::make_shared<Scene3dCube>(); }},
        };
    }

    void updateImguiMenu()
    {
        if (currentScene) {
            currentScene->updateImguiMenu();
        }
    }

private:
    std::shared_ptr<Scene> currentScene = nullptr;  //現在のシーン
    std::shared_ptr<Scene> nextScene = nullptr;     //次のシーン
    bool isSceneChange = false;                     //シーン変更フラグ

    //シーンのファクトリー
    using SceneFactory = std::function<std::shared_ptr<Scene>()>;
    using SceneEntry = std::pair<std::string, SceneFactory>;
    //std::map<std::string, SceneFactory> sceneFactories;
    std::vector<SceneEntry> sceneFactories;
};
