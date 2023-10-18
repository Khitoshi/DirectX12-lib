#pragma once
//#include "d3dx12.h"
#include <vector>
#include <memory>
#include <concepts>
#include "Scene.h"
#include <imgui/imgui.h>
//#include "SceneDefault.h"
#include "SceneTriangle.h"
#include "SceneSprite.h"
#include "Scene3dCube.h"
#include "SceneTexture3dCube.h"
#include "SceneModel.h"
#include <functional>
#include "InputManager.h"

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

	void init(ID3D12Device* device, std::shared_ptr<Scene> scene)
	{
		registerScene();
		this->currentScene = scene;
		if (this->currentScene) {
			this->currentScene->init(device);
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
	void render(RenderContext* rc)
	{
		if (currentScene) {
			currentScene->render(rc);
		}
	}

	/// <summary>
	/// �V�[���ύX����
	/// </summary>
	/// <param name="conf">�`��ɕK�v�Ȑݒ�</param>
	void changeScene(ID3D12Device* device)
	{
		if (!isSceneChange) return;

		//�����V�[���̏ꍇ�͕ύX���Ȃ�
		if (typeid(*currentScene) == typeid(*nextScene)) {
			isSceneChange = false;
			return;
		}

		//�V�[���ύX
		InputManager::Instance().clearMouseInputListener();
		clear();
		currentScene = std::move(this->nextScene);
		currentScene->init(device);
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
			//�V�[����ǉ�����ꍇ�͂����ɒǉ�
			//{"Default",     []() { return std::make_shared<SceneDefault>(); }},
			{"Triangle",    []() { return std::make_shared<SceneTriangle>(); }},
			{"Sprite",      []() { return std::make_shared<SceneSprite>(); }},
			{"Cube",      []() { return std::make_shared<Scene3dCube>(); }},
			{"TextureCube",      []() { return std::make_shared<SceneTexture3dCube>(); }},
			{"Model",      []() { return std::make_shared<SceneModel>(); }},
		};
	}

	void updateImguiMenu(RenderContext* rc, ImGuiManager* igm)
	{
		if (currentScene) {
			currentScene->updateImguiMenu(rc, igm);
		}
	}

private:
	std::shared_ptr<Scene> currentScene = nullptr;
	std::shared_ptr<Scene> nextScene = nullptr;
	bool isSceneChange = false;

	using SceneFactory = std::function<std::shared_ptr<Scene>()>;
	using SceneEntry = std::pair<std::string, SceneFactory>;
	std::vector<SceneEntry> sceneFactories;
};
