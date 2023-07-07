#include "RotationEffect.h"
#include <DirectXMath.h>

void RotationEffect::init(ID3D12Device* device, const Camera const* camera)
{
    this->createRootSignature(device);
	this->createShader();
	this->createPSO(device);
	this->createConstantBuffer(device, camera);
}

void RotationEffect::update(RenderContext* renderContext, const Camera const* camera)
{
    renderContext->setRootSignature(this->rootSignature.get());

    /*
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(3.14 / 2);
    auto matrix = DirectX::XMMatrixMultiply(camera->getWorldMatrix(), rotationMatrix);
    this->constantBuffer->copy(&matrix);
    */

    renderContext->setConstantBufferView(this->constantBuffer.get());
    renderContext->setPipelineState(this->pso.get());
    //renderContext->drawIndexed(0);
}

void RotationEffect::createRootSignature(ID3D12Device* device)
{
    RootSignatureConf rootSignatureConf = {};
    rootSignatureConf.samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    rootSignatureConf.textureAddressModeU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.textureAddressModeW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    rootSignatureConf.numSampler = 1;
    rootSignatureConf.maxCbvDescriptor = 1;
    rootSignatureConf.offsetInDescriptorsFromTableStartCB = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    rootSignatureConf.rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    this->rootSignature = RootSignatureCacheManager::getInstance().getOrCreate(device, rootSignatureConf);
}

void RotationEffect::createShader()
{
    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/RotationVS.hlsl";
        conf.entryFuncName = "VSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Vertex;
        shaderPair.vertexShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }

    {
        ShaderConf conf = {};
        conf.filePath = "./src/shaders/RotationPS.hlsl";
        conf.entryFuncName = "PSMain";
        conf.currentShaderModelType = ShaderConf::ShaderModelType::Pixel;
        shaderPair.pixelShader = ShaderCacheManager::getInstance().getOrCreate(conf);
    }
}

void RotationEffect::createPSO(ID3D12Device* device)
{
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
        {"COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
    };

    // インプットレイアウト
    PipelineStateObject::PipelineStateObjectConf PSOConf = {};
    PSOConf.desc.pRootSignature = this->rootSignature->getRootSignature();
    PSOConf.desc.VS = CD3DX12_SHADER_BYTECODE(shaderPair.vertexShader->getShaderBlob());
    PSOConf.desc.PS = CD3DX12_SHADER_BYTECODE(shaderPair.pixelShader->getShaderBlob());
    PSOConf.desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PSOConf.desc.SampleMask = UINT_MAX;
    PSOConf.desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PSOConf.desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

void RotationEffect::createConstantBuffer(ID3D12Device* device, const Camera const* camera)
{
    this->constantBuffer = std::make_unique<ConstantBuffer>();
    this->constantBuffer->init(device);
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(3.14 / 2);
    auto matrix = DirectX::XMMatrixMultiply(camera->getWorldMatrix(), rotationMatrix);
    //auto matrix = camera->getWorldMatrix() * camera->getViewMatrix() * camera->getProjectionMatrix();

    this->constantBuffer->copy(&matrix);
}

