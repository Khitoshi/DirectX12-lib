#include "Scene3dModel.h"
#include "Model.h"
#include "Camera.h"
#include "CameraController.h"
#include "CommonGraphicsConfig.h"
#include "InputManager.h"
#include "RenderContext.h"
#include "imgui\imgui.h"

void Scene3dModel::init(ID3D12Device* device)
{
    this->camera_ = std::make_shared<Camera>();
    this->camera_->init(windowWidth, windowHeight);
    this->camera_controller_ = std::make_shared<CameraController>(this->camera_.get());
    InputManager::Instance().addMouseInputListener(this->camera_controller_.get());

    Model::ModelConf conf = {};
    DirectX::XMStoreFloat4x4(&conf.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    DirectX::XMStoreFloat4x4(&conf.view, DirectX::XMMatrixTranspose(this->camera_->getViewMatrix()));
    DirectX::XMStoreFloat4x4(&conf.projection, DirectX::XMMatrixTranspose(this->camera_->getProjectionMatrix()));
    model = std::make_shared<Model>(conf);
    //model->init(device, "asset/models/box.fbx");
    model->init(device, "asset/models/RKonstantinov.fbx");
    //model->init(device, "asset/models/suzanne.obj");

}

void Scene3dModel::finalize()
{
}

void Scene3dModel::update()
{
    Model::ModelConf conf = {};
    DirectX::XMStoreFloat4x4(&conf.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    DirectX::XMStoreFloat4x4(&conf.view, DirectX::XMMatrixTranspose(this->camera_->getViewMatrix()));
    DirectX::XMStoreFloat4x4(&conf.projection, DirectX::XMMatrixTranspose(this->camera_->getProjectionMatrix()));
    this->model->setConf(conf);
    this->model->update();
}

void Scene3dModel::render(RenderContext* rc)
{
    model->draw(rc);
}

void Scene3dModel::updateImguiMenu()
{
    ImGui::Begin("3dModel");
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
