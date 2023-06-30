#pragma once
//#include "d3dx12.h"
#include <vector>
#include <memory>
#include <concepts>
#include "Scene.h"
#include "imgui/imgui.h"

#include "SceneTriangle.h"
#include "SceneSprite.h"
#include <functional>

/// <summary>
/// �V�[���Ǘ��N���X
/// </summary>
class SceneManager
{
private:
    SceneManager() :currentScene(std::make_shared<SceneDefault>()) {};
    ~SceneManager() {};
public:
    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns></returns>
    static SceneManager& getInstance()
    {
        static SceneManager instance;
        return instance;
    }

    /// <summary>
    /// ����������
    /// </summary>
    void clear()
    {
        if (this->currentScene) {
            this->currentScene->finalize();
        }
    }

    /// <summary>
    /// �X�V����
    /// </summary>
    void update()
    {
        if (currentScene) {
            currentScene->update();
        }
    }

    /// <summary>
    /// �`�揈��
    /// </summary>
    /// <param name="conf"></param>
    void render(SceneConf conf)
    {
        if (currentScene) {
            currentScene->render(conf);
        }
    }

    /// <summary>
    /// �V�[���ύX����
    /// </summary>
    /// <param name="conf">�`��ɕK�v�Ȑݒ�</param>
    void changeScene(SceneConf conf)
    {
        if (!isSceneChange) return;

        //�����V�[���̏ꍇ�͕ύX���Ȃ�
        if (typeid(*currentScene) == typeid(*nextScene)) {
            isSceneChange = false;
            return;
        }

        //�V�[���ύX
        clear();
        currentScene = std::move(this->nextScene);
        currentScene->init(conf);
        isSceneChange = false;
    }

    /// <summary>
    /// �V�[���I��
    /// </summary>
    void sceneSelect()
    {
        ImGui::Begin("Scene Manager");
        //�V�[���I��loop
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
    /// �V�[���o�^
    /// </summary>
    /// <param name="scene"></param>
    void registerScene()
    {
        sceneFactories = {
             {"Default",     []() { return std::make_shared<SceneDefault>(); }},
             {"Triangle",    []() { return std::make_shared<SceneTriangle>(); }},
             {"Sprite",      []() { return std::make_shared<SceneSprite>(); }},
             //�V�[����ǉ�����ꍇ�͂����ɒǉ�
        };
    }

    void updateImguiMenu()
    {
        if (currentScene) {
            currentScene->updateImguiMenu();
        }
    }

private:
    std::shared_ptr<Scene> currentScene = nullptr;  //���݂̃V�[��
    std::shared_ptr<Scene> nextScene = nullptr;     //���̃V�[��
    bool isSceneChange = false;                     //�V�[���ύX�t���O

    //�V�[���̃t�@�N�g���[
    using SceneFactory = std::function<std::shared_ptr<Scene>()>;
    using SceneEntry = std::pair<std::string, SceneFactory>;
    //std::map<std::string, SceneFactory> sceneFactories;
    std::vector<SceneEntry> sceneFactories;
};
