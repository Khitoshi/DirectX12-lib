#include "SceneFBXModel.h"
#include "FBXModel.h"
#include "Camera.h"
#include "CameraController.h"
#include "CommonGraphicsConfig.h"
#include "InputManager.h"
#include "RenderContext.h"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <string>

//std::string SceneFBXModel::file_path_("asset/models/RKonstantinov.fbx");
//std::string SceneFBXModel::file_path_("asset/models/box.fbx");
//std::string SceneFBXModel::file_path_("asset/models/CornellBox-Emptysquashed.fbx");
//std::string SceneFBXModel::file_path_("asset/models/.test/cube.002.1.fbx");
//std::string SceneFBXModel::file_path_("asset/models/.test/cube.004.fbx");
std::string SceneFBXModel::file_path_("asset/models/.test/nico.fbx");
//std::string SceneFBXModel::file_path_("asset/models/.test/cube.000.fbx");

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="device">GPUデバイス</param>
void SceneFBXModel::init(ID3D12Device* device)
{
    this->camera_ = std::make_shared<Camera>();
    this->camera_->init(windowWidth, windowHeight);
    this->camera_controller_ = std::make_shared<CameraController>(this->camera_.get());
    InputManager::Instance().addMouseInputListener(this->camera_controller_.get());

    FBXModel::ModelConf conf = {};
    DirectX::XMStoreFloat4x4(&conf.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    DirectX::XMStoreFloat4x4(&conf.view, DirectX::XMMatrixTranspose(this->camera_->getViewMatrix()));
    DirectX::XMStoreFloat4x4(&conf.projection, DirectX::XMMatrixTranspose(this->camera_->getProjectionMatrix()));
    model = std::make_shared<FBXModel>(conf);
    model->init(device, file_path_.c_str());
}

/// <summary>
/// 終了化処理
/// </summary>
void SceneFBXModel::finalize()
{
}

/// <summary>
/// 更新処理
/// </summary>
void SceneFBXModel::update()
{
    //モデルの切り替え
    if (is_change_model_) {
        model->setModel(file_path_.c_str());
        is_change_model_ = false;
    }

    FBXModel::ModelConf conf = {};
    DirectX::XMStoreFloat4x4(&conf.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    DirectX::XMStoreFloat4x4(&conf.view, DirectX::XMMatrixTranspose(this->camera_->getViewMatrix()));
    DirectX::XMStoreFloat4x4(&conf.projection, DirectX::XMMatrixTranspose(this->camera_->getProjectionMatrix()));
    this->model->setConf(conf);
    this->model->update();
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rc"></param>
void SceneFBXModel::render(RenderContext* rc)
{
    model->draw(rc);
}

/// <summary>
/// デバッグ用のメニューを表示する
/// </summary>
void SceneFBXModel::updateImguiMenu()
{
    ImGui::Begin("3dModel");

    //テクスチャの動的選択
    bool static isFileBrowser = true;
    static ImGui::FileBrowser fileDialog;
    if (isFileBrowser) {
        fileDialog.SetTitle("file brower");
        fileDialog.SetTypeFilters({ ".fbx" });
        if (!fileDialog.SetPwd("./asset/models")) {
            throw std::runtime_error("fileDialog.SetPwd");
        }

    }
    {
        ImGui::Text("NOW Models File Path:\n %s", this->file_path_.c_str());
        if (ImGui::Button("open file dialog")) {
            fileDialog.Open();
        }
        fileDialog.Display();
        if (fileDialog.HasSelected()) {
            auto u8str = fileDialog.GetSelected().generic_u8string();
            std::string str(u8str.begin(), u8str.end());
            file_path_ = str;
            fileDialog.ClearSelected();
            this->is_change_model_ = true;
        }
    }

    {//カメラの位置を変更する
        DirectX::XMFLOAT3 camera_eye = this->camera_->getEye();
        float eye[3] = { camera_eye.x, camera_eye.y, camera_eye.z };
        if (ImGui::DragFloat3("eye", eye)) {
            this->camera_->setEye(DirectX::XMFLOAT3(eye[0], eye[1], eye[2]));
        }
    }

    {//カメラの注視点を変更する
        DirectX::XMFLOAT3 camera_target = this->camera_->getTarget();
        float target[3] = { camera_target.x, camera_target.y, camera_target.z };
        if (ImGui::DragFloat3("target", target)) {
            this->camera_->setTarget(DirectX::XMFLOAT3(target[0], target[1], target[2]));
        }
    }

    {//カメラの描画の最大範囲
        float camera_far = this->camera_->getFar();
        if (ImGui::DragFloat("far", &camera_far)) {
            this->camera_->setFar(camera_far);
        }
    }

    {//カメラの描画の最小範囲
        float camera_near = this->camera_->getNear();
        if (ImGui::DragFloat("near", &camera_near)) {
            this->camera_->setNear(camera_near);
        }
    }

    {//ターゲットとの距離
        float camera_target_len = this->camera_->getTargetPositionLength();
        if (ImGui::DragFloat("target length", &camera_target_len)) {
            this->camera_->setTargetPositionLength(camera_target_len);
        }
    }

    ImGui::End();
}
