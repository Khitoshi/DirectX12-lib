#include "GraphicsCommandList.h"
#include <stdexcept>

void GraphicsCommandList::init(ID3D12Device* device, ID3D12CommandAllocator* command_allocator)
{
	//TODO:���݂�GPU��1�Ȃ̂�nodemask��0�ɂ��Ă��邩��C����GPU�ɑΉ�����ꍇ�͕ύX����K�v������
	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, nullptr, IID_PPV_ARGS(&this->command_list_)))) {
		throw std::runtime_error("failed to create command list");
	}

	//�R�}���h���X�g�������ɂ͂܂��R�}���h���X�g�̓N���[�Y����Ă��Ȃ��̂ŁC�N���[�Y����
	if (FAILED(this->command_list_->Close())) {
		throw std::runtime_error("failed to close command list");
	}
}