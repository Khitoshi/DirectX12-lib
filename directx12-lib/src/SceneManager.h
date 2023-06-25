#pragma once
//#include "d3dx12.h"
#include <vector>
#include <memory>
#include <concepts>
#include "Scene.h"


/// <summary>
/// �V�[���Ǘ��N���X
/// </summary>
class SceneManager
{
private:
    SceneManager() :currentScene() {};
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
    /// ����������
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
    std::shared_ptr<Scene> currentScene = nullptr;//���݂̃V�[��
};

