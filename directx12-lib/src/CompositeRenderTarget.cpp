#include "CompositeRenderTarget.h"
#include <stdexcept>
#include "OffScreenRenderTargetCacheManager.h"
#include "RootSignatureCacheManager.h"
#include "PSOCacheManager.h"
#include "ShaderCacheManager.h"

/// <summary>
/// ������
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void CompositeRenderTarget::init(ID3D12Device* device)
{
    createResource(device);
    createSRVHeap(device);
    createShaderResourceView(device);
    createRTVHeap(device);
    createRenderTargetView(device);

    initRootSignature(device);
    initShader(device);
    initVertexBuffer(device);
    initPipelineStateObject(device);

    descriptorHeapCache = std::make_unique<DescriptorHeapCache>();
}

/// <summary>
/// offscreen�ɕ`�������ׂ̏�������
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
void CompositeRenderTarget::beginRender(RenderContext* rc, D3D12_VIEWPORT viewport, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle)
{
    rc->simpleStart(this->RTVHeap->GetCPUDescriptorHandleForHeapStart(), depthStencilViewHandle, this->resource.Get());
}

void CompositeRenderTarget::render(RenderContext* rc, ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle)
{
    //���[�g�V�O�l�`����ݒ�B
    rc->setRootSignature(this->rootSignature.get());
    //�p�C�v���C���X�e�[�g��ݒ�B
    rc->setPipelineState(this->pso.get());
    //�v���~�e�B�u�̃g�|���W�[��ݒ�B
    rc->setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //���_�o�b�t�@��ݒ�B
    rc->setVertexBuffer(this->vb.get());

    // SRVHeap�̎n�_���擾
    UINT slot = 0;
    for (auto& rt : OffScreenRenderTargetCacheManager::getInstance().getRenderTargetList())
    {
        //�f�B�X�N���v�^�q�[�v��ݒ�
        DescriptorHeapCache::DescriptorHeapCacheConf dhcConf = {};
        dhcConf.resource = rt->getResource();
        dhcConf.slot = slot;
        descriptorHeapCache->getOrCreate(device, dhcConf, this->SRVHeap.Get(), srvDesc);
        slot++;
    }
    rc->setDescriptorHeap(this->SRVHeap.Get());
    //GPU�n���h����commandlist�ɐݒ�
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = this->SRVHeap->GetGPUDescriptorHandleForHeapStart();
    rc->setGraphicsRootDescriptorTable(0, gpuHandle);

    //�h���[�R�[��
    rc->drawInstanced(4);
    OffScreenRenderTargetCacheManager::getInstance().clearRenderTargetList();
}

/// <summary>
/// offscreen�ɕ`�������ׂ̏I������
/// </summary>
/// <param name="rc">�����_�[�R���e�L�X�g</param>
void CompositeRenderTarget::endRender(RenderContext* rc)
{
    //�����_�[�^�[�Q�b�g��RESOURCE_BARRIER�ݒ�
    rc->TransitionTemporaryRenderTargetAwait(this->resource.Get());

}

/// <summary>
/// ���\�[�X���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void CompositeRenderTarget::createResource(ID3D12Device* device)
{
    D3D12_RESOURCE_DESC desc = {};
    desc = this->conf.resourceDesc;
    D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, conf.clearColor);

    if (FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(this->resource.ReleaseAndGetAddressOf())
    ))) {
        throw std::runtime_error("failed to create offscreen render target");
    }
}

/// <summary>
/// �V�F�[�_�[���\�[�X�r���[�p�̃f�B�X�N���v�^�q�[�v���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void CompositeRenderTarget::createSRVHeap(ID3D12Device* device)
{
    //SRV�f�B�X�N���v�^�q�[�v�쐬
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf.descriptorHeapDesc;
    desc.NumDescriptors = 2;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->SRVHeap.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// �V�F�[�_�[���\�[�X�r���[���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void CompositeRenderTarget::createShaderResourceView(ID3D12Device* device)
{
    ////�V�F�[�_�[���\�[�X�r���[���쐬
    srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

/// <summary>
/// �����_�[�^�[�Q�b�g�r���[�p�̃f�B�X�N���v�^�q�[�v���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void CompositeRenderTarget::createRTVHeap(ID3D12Device* device)
{
    //RTV�f�B�X�N���v�^�q�[�v�쐬
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc = this->conf.descriptorHeapDesc;
    desc.NumDescriptors = 1;
    if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(this->RTVHeap.ReleaseAndGetAddressOf())))) {
        throw std::runtime_error("failed to create offscreen render target descriptor heap");
    }
}

/// <summary>
/// �����_�[�^�[�Q�b�g�r���[���쐬����
/// </summary>
/// <param name="device">������&�����ς݂�GPU�f�o�C�X</param>
void CompositeRenderTarget::createRenderTargetView(ID3D12Device* device)
{
    //�����_�[�^�[�Q�b�g�r���[���쐬
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateRenderTargetView(this->resource.Get(), &rtvDesc, this->RTVHeap->GetCPUDescriptorHandleForHeapStart());
}

void CompositeRenderTarget::initRootSignature(ID3D12Device* device)
{
    RootSignatureConf rootSignatureConf = {};

    rootSignatureConf.samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.textureAddressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.numSampler = 1;
    rootSignatureConf.maxSrvDescriptor = 2;
    rootSignatureConf.offsetInDescriptorsFromTableStartSRV = 0;
    rootSignatureConf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    this->rootSignature = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void CompositeRenderTarget::initShader(ID3D12Device* device)
{
    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/MultiPathCompositingVS.hlsl";
        conf.entryFuncName = "VSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        this->shaderPair.vertexShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }

    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/MultiPathCompositingPS.hlsl";
        conf.entryFuncName = "PSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        this->shaderPair.pixelShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

void CompositeRenderTarget::initPipelineStateObject(ID3D12Device* device)
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
    };

    //�[�x�X�e���V���X�e�[�g
    D3D12_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = FALSE; // �[�x�e�X�g�𖳌��ɂ���
    dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // �[�x�l�̏������݂𖳌��ɂ���
    dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // �[�x��r�֐�����ɐ^�ɂ���
    // �X�e���V���ݒ�������ɂ���Ȃ�ȉ����ݒ�
    dsDesc.StencilEnable = FALSE;
    dsDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    dsDesc.FrontFace = defaultStencilOp;
    dsDesc.BackFace = defaultStencilOp;


    // �C���v�b�g���C�A�E�g
    PipelineStateObject::PipelineStateObjectConf PSOConf = {};
    PSOConf.desc.pRootSignature = this->rootSignature->getRootSignature();
    PSOConf.desc.VS = CD3DX12_SHADER_BYTECODE(this->shaderPair.vertexShader->getShaderBlob());
    PSOConf.desc.PS = CD3DX12_SHADER_BYTECODE(this->shaderPair.pixelShader->getShaderBlob());
    PSOConf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PSOConf.desc.SampleMask = UINT_MAX;
    PSOConf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PSOConf.desc.DepthStencilState = dsDesc;
    //PSOConf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    PSOConf.desc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    PSOConf.desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    PSOConf.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSOConf.desc.NumRenderTargets = 1;
    PSOConf.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    PSOConf.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    PSOConf.desc.SampleDesc.Count = 1;
    PSOConf.desc.SampleDesc.Quality = 0;
    PSOConf.desc.NodeMask = 1;
    PSOConf.desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    this->pso = PSOCacheManager::getInstance().getPSO(device, PSOConf);
}

void CompositeRenderTarget::initVertexBuffer(ID3D12Device* device)
{
    //���_�o�b�t�@�̍쐬
    Vertex vertices[] = {
        {{ -1.0f, -1.0f, 0.0f }, {0,1}}, // ����
        {{ -1.0f,  1.0f, 0.0f }, {0,0}}, // ����
        {{  1.0f, -1.0f, 0.0f }, {1,1}}, // �E��
        {{  1.0f,  1.0f, 0.0f }, {1,0}}, // �E��
    };

    VertexBufferConf conf = {};
    conf.device = device;
    conf.size = sizeof(vertices);
    conf.stride = sizeof(Vertex);
    vb = std::make_shared<VertexBuffer>();
    vb->init(conf);
    vb->copy(vertices);
}
