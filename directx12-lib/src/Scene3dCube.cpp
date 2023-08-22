#include "Scene3dCube.h"
#include "Camera.h"
#include "CameraController.h"
#include "CubeModel.h"
#include "CommonGraphicsConfig.h"
#include <imgui/imgui.h>
#include "InputManager.h"
/// <summary>
/// ����������
/// </summary>
/// <param name="device"></param>
void Scene3dCube::init(ID3D12Device* device)
{
    this->camera_ = std::make_shared<Camera>();
    this->camera_->init(windowWidth, windowHeight);
    this->camera_controller_ = std::make_shared<CameraController>(this->camera_.get());
    InputManager::Instance().addMouseInputListener(this->camera_controller_.get());

    //�V���v����3d�L���[�u�̏�����
    CubeModel::CubeModelConf conf = {};
    DirectX::XMStoreFloat4x4(&conf.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    DirectX::XMStoreFloat4x4(&conf.view, DirectX::XMMatrixTranspose(this->camera_->getViewMatrix()));
    DirectX::XMStoreFloat4x4(&conf.projection, DirectX::XMMatrixTranspose(this->camera_->getProjectionMatrix()));
    this->cube_model_ = std::make_shared<CubeModel>(conf);
    this->cube_model_->init(device);
}

/// <summary>
/// �I��������
/// </summary>
void Scene3dCube::finalize()
{
}

/// <summary>
/// �X�V����
/// </summary>
void Scene3dCube::update()
{
    this->camera_->update(windowWidth, windowHeight);

    //if (is_change_camera_) {
    CubeModel::CubeModelConf conf = {};
    DirectX::XMStoreFloat4x4(&conf.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    DirectX::XMStoreFloat4x4(&conf.view, DirectX::XMMatrixTranspose(this->camera_->getViewMatrix()));
    DirectX::XMStoreFloat4x4(&conf.projection, DirectX::XMMatrixTranspose(this->camera_->getProjectionMatrix()));

    this->cube_model_->setConf(conf);
    this->cube_model_->update();
    //is_change_camera_ = false;
    //}
}

/// <summary>
/// �`�揈��
/// </summary>
/// <param name="rc"></param>
void Scene3dCube::render(RenderContext* rc)
{
    this->cube_model_->draw(rc);
}

/// <summary>
/// �f�o�b�O�p��imgui��menu��\��
/// </summary>
void Scene3dCube::updateImguiMenu()
{
    ImGui::Begin("3dCube");
    {//�J�����̈ʒu��ύX����
        DirectX::XMFLOAT3 camera_eye = this->camera_->getEye();
        float eye[3] = { camera_eye.x, camera_eye.y, camera_eye.z };
        if (ImGui::DragFloat3("eye", eye)) {
            is_change_camera_ = true;
            this->camera_->setEye(DirectX::XMFLOAT3(eye[0], eye[1], eye[2]));
        }
    }

    {//�J�����̒����_��ύX����
        DirectX::XMFLOAT3 camera_target = this->camera_->getTarget();
        float target[3] = { camera_target.x, camera_target.y, camera_target.z };
        if (ImGui::DragFloat3("target", target)) {
            is_change_camera_ = true;
            this->camera_->setTarget(DirectX::XMFLOAT3(target[0], target[1], target[2]));
        }
    }

    {//�J������
        float camera_far = this->camera_->getFar();
        if (ImGui::DragFloat("far", &camera_far)) {
            is_change_camera_ = true;
            this->camera_->setFar(camera_far);
        }
    }

    {//�J������
        float camera_near = this->camera_->getNear();
        if (ImGui::DragFloat("near", &camera_near)) {
            is_change_camera_ = true;
            this->camera_->setNear(camera_near);
        }
    }

    ImGui::End();
}
