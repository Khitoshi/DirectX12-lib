#pragma once
//#include "d3dx12.h"
#include <vector>
#include <memory>
#include <concepts>

//�V�[���̃C���^�[�t�F�[�X
template<typename T>
concept Scene = requires(T & value)
{
    {value.init()}      -> std::convertible_to<void>;//����������
    {value.finalize()}  -> std::convertible_to<void>;//�I��������
    {value.update()}    -> std::convertible_to<void>;//�X�V����
    {value.render()}    -> std::convertible_to<void>;//�`�揈��
};


template<Scene T>
class SceneManager
{
private:
    SceneManager() {};
    ~SceneManager() {};
public:

    //�V���O���g��
    static SceneManager& getInstance()
    {
        static SceneManager instance;
        return instance;
    }

    //����������
    void clear()
    {
        if (this->currentScene) {
            this->currentScene->finalize();
        }
    }

    //����������
    void update()
    {
        if (currentScene) {
            currentScene->update();
        }
    }

    //�`�揈��
    void render()
    {
        if (currentScene) {
            currentScene->render();
        }
    }

    //�ύX����
    void changeScene(T* scene)
    {
        if (scene) {
            currentScene->finalize();
            currentScene = scene;
            currentScene->init();
        }
    }
private:
    //���݂̃V�[��
    T* currentScene;
};

