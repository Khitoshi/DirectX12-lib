#include "SceneSprite.h"
#include "Camera.h"
#include <imgui/imgui.h>
#include <imgui/imfilebrowser.h>

std::string SceneSprite::file_path_("asset/img/Lena.png");

/// <summary>
/// ����������
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
void SceneSprite::init(ID3D12Device* device)
{
    this->device_ = device;

    //3�p�`�̃��\�[�X���쐬&������
    this->sprite_ = std::make_shared<Sprite>();
    this->sprite_->init(device, file_path_.c_str());

    //���_���W��ݒ�
    this->vertex_[0] = this->sprite_->getVertices(0);
    this->vertex_[1] = this->sprite_->getVertices(1);
    this->vertex_[2] = this->sprite_->getVertices(2);
    this->vertex_[3] = this->sprite_->getVertices(3);
}

/// <summary>
/// �I��������
/// </summary>
void SceneSprite::finalize()
{
}

/// <summary>
/// �X�V����
/// </summary>
void SceneSprite::update()
{
    //���_���W���X�V
    if (this->is_change_vertex_) {
        this->sprite_->setVertices(this->vertex_);
        this->is_change_vertex_ = false;
    }

    //�e�N�X�`�����X�V
    if (this->is_change_texture_) {
        this->is_change_texture_ = false;
        this->sprite_->setTexture(this->device_, this->file_path_.c_str());
    }


}

/// <summary>
/// �`�揈��
/// </summary>
/// <param name="rc"></param>
void SceneSprite::render(RenderContext* rc)
{
    this->sprite_->draw(rc);
}

/// <summary>
/// scene�Ŏg�p����imgui�̃��j���[�X�V����
/// </summary>
void SceneSprite::updateImguiMenu()
{
    ImGui::Begin("Sprite");

    //�e�N�X�`���̓��I�I��
    bool static isFileBrowser = true;
    static ImGui::FileBrowser fileDialog;
    if (isFileBrowser) {
        fileDialog.SetTitle("file brower");
        fileDialog.SetTypeFilters({ ".png",".jpg",".bmp" });
        if (!fileDialog.SetPwd("./asset/img")) {
            throw std::runtime_error("fileDialog.SetPwd");
        }

    }
    {
        ImGui::Text("NOW Texture File Path: %s", this->file_path_.c_str());
        if (ImGui::Button("open file dialog")) {
            fileDialog.Open();
        }
        fileDialog.Display();
        if (fileDialog.HasSelected()) {
            auto u8str = fileDialog.GetSelected().generic_u8string();
            std::string str(u8str.begin(), u8str.end());
            file_path_ = str;
            fileDialog.ClearSelected();
            this->is_change_texture_ = true;
        }
    }


    //���_�̈ʒu
    for (int i = 0; i < 4; ++i) {
        float position[3] = { this->vertex_[i].position.x, this->vertex_[i].position.y, this->vertex_[i].position.z };
        std::string label = "Vertex pos" + std::to_string(i + 1);

        if (ImGui::SliderFloat3(label.c_str(), position, -1.0f, 1.0f)) {
            this->vertex_[i].position.x = position[0];
            this->vertex_[i].position.y = position[1];
            this->vertex_[i].position.z = position[2];
            this->is_change_vertex_ = true;
        }
    }

    //���_�̈ʒu
    for (int i = 0; i < 4; ++i) {
        float uv[2] = { this->vertex_[i].uv.x, this->vertex_[i].uv.y, };
        std::string label = "Vertex uv" + std::to_string(i + 1);

        if (ImGui::SliderFloat2(label.c_str(), uv, -1.0f, 1.0f)) {
            this->vertex_[i].uv.x = uv[0];
            this->vertex_[i].uv.y = uv[1];
            this->is_change_vertex_ = true;
        }
    }

    //static bool isRotation = false;
    //if (ImGui::Checkbox("Rotation", &isRotation))imageFeatures |= FEATURE_ROTATE;
    //else imageFeatures &= ~FEATURE_ROTATE;

    ImGui::End();
}