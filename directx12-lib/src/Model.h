#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <map>
#include "d3dx12.h"
#include "Mesh.h"

class RootSignature;
class DescriptorHeap;
class Shader;
class PipelineStateObject;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class DepthStencil;
class Texture;
class OffScreenRenderTarget;
class RenderContext;
class GeometryBuffer;
class PhongShading;
class ImGuiManager;

class Model
{
public:
	//定数バッファへのデータ転送用構造体
	struct ModelConf
	{
		DirectX::XMFLOAT4X4  model;
		DirectX::XMFLOAT4X4  view;
		DirectX::XMFLOAT4X4  projection;
	};
public:
	Model(const ModelConf& c) :
		conf_(c),
		meshes_(),
		root_signature_(),
		descriptor_heap_(),
		vertex_shader_(),
		pixel_shader_(),
		pso_(),
		vertex_buffers_(),
		index_buffers_(),
		constant_buffer_(),
		num_indices_(0),
		depth_stencil_(),
		off_screen_render_target_(),
		device_(0),
		num_cb_descriptors_(0),
		num_srv_descriptors_(0),
		geometry_buffer_(),
		phong_shading_(),
		camera_position_()
	{};
	~Model() {};

	void init(ID3D12Device* device, const char* model_file_path);
	void update();
	void draw(RenderContext* rc);
	void debugDraw(RenderContext* rc, ImGuiManager* igm);
private:
	void loadModel(ID3D12Device* device, const char* model_file_path);
	void loadShader();
	void initRootSignature(ID3D12Device* device);
	void initDescriptorHeap(ID3D12Device* device);
	void initPipelineStateObject(ID3D12Device* device);
	void initVertexBuffer(ID3D12Device* device);
	void initIndexBuffer(ID3D12Device* device);
	void initConstantBuffer(ID3D12Device* device);
	void initTexture(ID3D12Device* device);
	void initDepthStencil(ID3D12Device* device);
	void initOffScreenRenderTarget(ID3D12Device* device);

	void initGBuffers(ID3D12Device* device);

public:
	void setConf(const ModelConf& c) { conf_ = c; }
	void setModel(const char* model_file_path);
	void setCameraPosition(DirectX::XMFLOAT3 pos) { camera_position_ = pos; }
public:
	std::vector<std::shared_ptr<GeometryBuffer>> getGeometryBuffer() { return geometry_buffer_; }

private:
	ModelConf conf_;
	std::vector<Mesh> meshes_;
	std::shared_ptr<RootSignature> root_signature_;
	std::shared_ptr<DescriptorHeap> descriptor_heap_;

	std::shared_ptr<Shader> vertex_shader_;
	std::shared_ptr<Shader> pixel_shader_;

	std::shared_ptr<PipelineStateObject> pso_;

	std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers_;
	std::vector<std::shared_ptr<IndexBuffer>> index_buffers_;

	std::vector<UINT> num_indices_;
	std::shared_ptr<ConstantBuffer> constant_buffer_;
	std::shared_ptr<DepthStencil> depth_stencil_;
	std::shared_ptr<OffScreenRenderTarget> off_screen_render_target_;

	std::vector<std::shared_ptr<ConstantBuffer>> mesh_color_constant_buffers_;

	ID3D12Device* device_;

	UINT num_cb_descriptors_;
	UINT num_srv_descriptors_;

	std::vector<std::shared_ptr<GeometryBuffer>> geometry_buffer_;

	std::shared_ptr<PhongShading> phong_shading_;

	DirectX::XMFLOAT3 camera_position_;
};
