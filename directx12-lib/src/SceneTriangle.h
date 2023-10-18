#pragma once
#include "Scene.h"
#include "Triangle.h"

/// <summary>
/// �O�p�`�`��p�V�[��
/// </summary>
class SceneTriangle final : public Scene
{
public:
	SceneTriangle() :
		triangle_(nullptr),
		vertex_(),
		is_change_vertex_(false)
	{};
	~SceneTriangle() {};

	//����������
	void init(ID3D12Device* device)override;
	//�I������
	void finalize()override;
	//�X�V����
	void update()override;
	//�`�揈��
	void render(RenderContext* rc)override;
	//imgui��menu��\��
	void updateImguiMenu(RenderContext* rc, ImGuiManager* igm)override;
private:
	std::shared_ptr<Triangle> triangle_;    //�O�p�`
	Triangle::Vertex vertex_[3];            //���_���W
	bool is_change_vertex_;                 //���_���W��ύX������
};
