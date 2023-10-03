#pragma once

#include "DescriptorHeap.h"
#include <vector>
#include <memory>


//TODO:�g�p���Ă��Ȃ��̂ō폜����
class CBVManager
{
private:
	CBVManager() {}
	~CBVManager() {}

public:
	static CBVManager* GetInstance()
	{
		static CBVManager instance;
		return &instance;
	}

	//�o�^���邽�����A���łɓ������e���o�^����Ă���ꍇ�͉������Ȃ�
	void registerHeap(std::shared_ptr<DescriptorHeap> heap)
	{
		auto itr = std::find(cbv_heaps_.begin(), cbv_heaps_.end(), heap);
		if (itr != cbv_heaps_.end()) return;
		cbv_heaps_.push_back(heap);
	}

	//��ʃT�C�Y��ύX����ۂȂǂɈ�x�N���A����K�v������
	void clear()
	{
		for (auto& heap : cbv_heaps_) {
			heap->clear();
		}
	}

	//�t���X�N���[���ɐ؂�ւ���ꍇ�ȂǂɈ�x�N���A���Ă���f�B�X�N���v�^�̍č쐬���s���K�v������
	void reInit(ID3D12Device* device)
	{
		for (auto& heap : cbv_heaps_) {
			heap->reInit(device);
		}
	}

private:
	std::vector<std::shared_ptr<DescriptorHeap>> cbv_heaps_;

};
