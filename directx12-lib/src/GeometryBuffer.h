#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

class Shader;
class RootSignature;
class DescriptorHeap;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class DepthStencil;
class OffScreenRenderTarget;
class RenderContext;
class ImGuiManager;

class GeometryBuffer
{
public:
	struct Conf
	{
		enum class Semantic {
			NORMAL,
			COLOR,

			ALBEDO,
			POSITION,
		};

		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 data;
		};

		struct Matrix
		{
			DirectX::XMFLOAT4X4  model;
			DirectX::XMFLOAT4X4  view;
			DirectX::XMFLOAT4X4  projection;
		};

		Conf(const Semantic s, const std::vector<Vertex> v, const std::vector<unsigned short>i, Matrix m) :
			semantic(s),
			vertices(v),
			indices(i),
			matrix(m)
		{};

		std::string getSemanticName() const {
			switch (semantic) {
			case Conf::Semantic::NORMAL:
				return "Normal";

			case Conf::Semantic::COLOR:
				return "Color";

			case Conf::Semantic::ALBEDO:
				return "Albedo";

			case Conf::Semantic::POSITION:
				return "Position";

			default:
				return "SemanticERROR";
			}
		}

		const Semantic semantic;

		const std::vector<Vertex> vertices;

		const std::vector<unsigned short> indices;

		Matrix matrix;
	};

public:
	GeometryBuffer(const Conf c) :
		conf_(c),
		vs_(nullptr),
		ps_(nullptr),
		root_signature_(nullptr),

		pso_(nullptr),
		vb_(),
		ib_(),
		descriptor_heap_(nullptr),
		depth_stencil_(nullptr),
		off_screen_render_target_(nullptr),
		num_indices_(0),
		srv_handle_(),
		device_()
	{};
	~GeometryBuffer() {};

	void init(ID3D12Device* device);
	void update(const Conf::Matrix& m);
	void draw(RenderContext* rc);
	void debugDraw(RenderContext* rc, ImGuiManager* igm, int i);
private:
	void initRootSignature(ID3D12Device* device);
	void loadShader();
	void initPSO(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initIndexBuffer(ID3D12Device* device);
	void initConstantBuffer(ID3D12Device* device);
	void initDescriptorHeap(ID3D12Device* device);
	void initDepthStencil(ID3D12Device* device);
	void initOffScreenRenderTarget(ID3D12Device* device);

public://Žæ“¾Œn
	std::shared_ptr<OffScreenRenderTarget> getOffScreenRenderTarget() const { return off_screen_render_target_; }

private:
	const Conf conf_;

	std::shared_ptr<Shader> vs_;
	std::shared_ptr<Shader> ps_;

	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<VertexBuffer> vb_;
	std::shared_ptr<IndexBuffer> ib_;
	std::shared_ptr<ConstantBuffer> cb_;

	std::shared_ptr<DescriptorHeap> descriptor_heap_;
	std::shared_ptr<DepthStencil> depth_stencil_;

	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;

	UINT num_indices_;

	D3D12_CPU_DESCRIPTOR_HANDLE  srv_handle_;

	ID3D12Device* device_;
};
