#pragma once
#include <vector>
#include <memory>
#include <concepts>
#include "Scene.h"
#include <imgui/imgui.h>
#include "SceneTriangle.h"
#include "SceneSprite.h"
#include "Scene3dCube.h"
#include "SceneTexture3dCube.h"
#include "SceneModel.h"
#include <functional>
#include "InputManager.h"

class SceneManager
{
private:
	SceneManager() :currentScene() {};
	~SceneManager() {};
public:
	static SceneManager& getInstance()
	{
		static SceneManager instance;
		return instance;
	}

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

	void update()
	{
		if (currentScene) {
			currentScene->update();
		}
	}

	void render(RenderContext* rc)
	{
		if (currentScene) {
			currentScene->render(rc);
		}
	}

	void changeScene(ID3D12Device* device)
	{
		if (!isSceneChange) return;

		if (typeid(*currentScene) == typeid(*nextScene)) {
			isSceneChange = false;
			return;
		}

		//シーン変更
		InputManager::Instance().clearMouseInputListener();
		clear();
		currentScene = std::move(this->nextScene);
		currentScene->init(device);
		isSceneChange = false;
	}

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

	void registerScene()
	{
		sceneFactories = {
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
