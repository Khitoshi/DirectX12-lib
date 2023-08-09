#include "Scene3dModel.h"
#include "Model.h"
#include "Camera.h"
#include "CameraController.h"
#include "CommonGraphicsConfig.h"
#include "InputManager.h"
#include "RenderContext.h"

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
