#include "GraphicsCommandList.h"
#include <stdexcept>

void GraphicsCommandList::init(ID3D12Device* device, ID3D12CommandAllocator* command_allocator)
{
	//TODO:現在のGPUが1つなのでnodemaskは0にしているから，複数GPUに対応する場合は変更する必要がある
	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, nullptr, IID_PPV_ARGS(&this->command_list_)))) {
		throw std::runtime_error("failed to create command list");
	}

	//コマンドリスト生成時にはまだコマンドリストはクローズされていないので，クローズする
	if (FAILED(this->command_list_->Close())) {
		throw std::runtime_error("failed to close command list");
	}
}