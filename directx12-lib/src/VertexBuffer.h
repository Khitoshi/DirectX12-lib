#pragma once
#include "d3dx12.h"
#include "Descriptor.h"

using namespace Microsoft::WRL;

/// <summary>
/// ���_�o�b�t�@�����p&copy����N���X
/// </summary>
class VertexBuffer :public Descriptor
{
	friend class VertexBufferFactory;
public:
	/// <summary>
	/// ���_�o�b�t�@�������Ɏg�p����ݒ�
	/// </summary>
	struct VertexBufferConf {
		UINT size;               //���_�o�b�t�@�̃T�C�Y
		UINT stride;             //1���_�̃T�C�Y
	};

private:
	VertexBuffer(const VertexBufferConf c) :
		Descriptor(Descriptor::DescriptorType::Vertex),
		conf_(c),
		vertex_buffer_view_()
	{};

public:
	~VertexBuffer() {};

private:
	void init(ID3D12Device* device);

public:
	const D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView() const { return this->vertex_buffer_view_; }
	VertexBufferConf getConf() const { return this->conf_; }
private:
	VertexBufferConf conf_;
	D3D12_VERTEX_BUFFER_VIEW    vertex_buffer_view_;
};
