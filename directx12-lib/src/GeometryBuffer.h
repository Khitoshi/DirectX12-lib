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
			POSITION,
			NORMAL,
			TEXCOORD,
			TANGENT,
			COLOR,
		};

		enum class Format {
			FLOAT4,
			FLOAT3,
			FLOAT2,
		};

		struct Vertex
		{
			DirectX::XMFLOAT4 position;
			//std::vector<float> data;
			DirectX::XMFLOAT4 data;
		};


		Conf(const Semantic s, const Format f, const std::vector<Vertex> v, const std::vector<unsigned short>i) :
			semantic(s),
			format(f),
			vertices(v),
			indices(i)
		{};

		/*
		UINT getStrideSize() const {

			switch (format) {
			case Conf::Format::FLOAT4:
				return sizeof(float) * 4;
			case Conf::Format::FLOAT3:
				return sizeof(float) * 3;
			case Conf::Format::FLOAT2:
				return sizeof(float) * 2;
			default:
				return 0;
			}
		}
		*/

		std::string getSemanticName() const {
			switch (semantic) {
			case Conf::Semantic::POSITION:
				return "Position";
			case Conf::Semantic::NORMAL:
				return "Normal";
			case Conf::Semantic::TEXCOORD:
				return "Texcoord";
			case Conf::Semantic::TANGENT:
				return "Tangent";
			case Conf::Semantic::COLOR:
				return "Color";


			default:
				return "SemanticERROR";
			}
		}

		std::string getFormatName() const {
			switch (format) {
			case Conf::Format::FLOAT4:
				return "F4";
			case Conf::Format::FLOAT3:
				return "F3";
			case Conf::Format::FLOAT2:
				return "F2";


			default:
				return "FormatERROR";
			}
		}


		const Semantic semantic;

		const Format format;

		const std::vector<Vertex> vertices;

		const std::vector<unsigned short> indices;

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
	void update();
	void draw(RenderContext* rc);
	void debugDraw(RenderContext* rc, ImGuiManager* igm, int i);
private:
	void initRootSignature(ID3D12Device* device);
	void loadShader();
	void initPSO(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initIndexBuffer(ID3D12Device* device);
	void initSRVHeap(ID3D12Device* device);
	void initDepthStencil(ID3D12Device* device);
	void initOffScreenRenderTarget(ID3D12Device* device);


	void initSRV();

private:
	const Conf conf_;

	std::shared_ptr<Shader> vs_;
	std::shared_ptr<Shader> ps_;

	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<PipelineStateObject> pso_;
	std::shared_ptr<VertexBuffer> vb_;
	std::shared_ptr<IndexBuffer> ib_;
	std::shared_ptr<DescriptorHeap> descriptor_heap_;
	std::shared_ptr<DepthStencil> depth_stencil_;

	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;

	UINT num_indices_;

	D3D12_CPU_DESCRIPTOR_HANDLE  srv_handle_;

	ID3D12Device* device_;
};
