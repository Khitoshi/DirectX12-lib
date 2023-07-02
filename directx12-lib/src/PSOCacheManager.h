#pragma once
#include "PipelineStateObject.h"
#include "ShaderCacheManager.h"
#include "RootSignature.h"
#include <map>
#include "RootSignatureCacheManager.h"
#include <stdexcept>



class PSOCacheManager
{
public:
    struct PipelineStateSettings {
        struct PipelineStateDesc
        {
            D3D12_STREAM_OUTPUT_DESC streamOutput;
            D3D12_BLEND_DESC blendState;
            UINT sampleMask;
            D3D12_RASTERIZER_DESC rasterizerState;
            D3D12_DEPTH_STENCIL_DESC depthStencilState;
            std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE iBStripCutValue;
            D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
            UINT numRenderTargets;
            DXGI_FORMAT rtvFormat;
            DXGI_FORMAT dsvFormat;
            DXGI_SAMPLE_DESC sampleDesc;
            UINT nodeMask;
            D3D12_PIPELINE_STATE_FLAGS flags;
        };
        PipelineStateDesc pipelineStateDesc;

        std::shared_ptr<RootSignature> rootSignature;
        BasicShaderPair shaderPair;

        bool operator==(const PipelineStateSettings& other) const
        {
            return
                memcmp(&pipelineStateDesc.streamOutput, &other.pipelineStateDesc.streamOutput, sizeof(pipelineStateDesc.streamOutput)) == 0 &&
                memcmp(&pipelineStateDesc.blendState, &other.pipelineStateDesc.blendState, sizeof(pipelineStateDesc.blendState)) == 0 &&
                pipelineStateDesc.sampleMask == other.pipelineStateDesc.sampleMask &&
                memcmp(&pipelineStateDesc.rasterizerState, &other.pipelineStateDesc.rasterizerState, sizeof(pipelineStateDesc.rasterizerState)) == 0 &&
                memcmp(&pipelineStateDesc.depthStencilState, &other.pipelineStateDesc.depthStencilState, sizeof(pipelineStateDesc.depthStencilState)) == 0 &&
                pipelineStateDesc.inputLayout.size() == other.pipelineStateDesc.inputLayout.size() &&
                pipelineStateDesc.iBStripCutValue == other.pipelineStateDesc.iBStripCutValue &&
                pipelineStateDesc.primitiveTopologyType == other.pipelineStateDesc.primitiveTopologyType &&
                pipelineStateDesc.numRenderTargets == other.pipelineStateDesc.numRenderTargets &&
                pipelineStateDesc.rtvFormat == other.pipelineStateDesc.rtvFormat &&
                pipelineStateDesc.dsvFormat == other.pipelineStateDesc.dsvFormat &&
                pipelineStateDesc.sampleDesc.Count == other.pipelineStateDesc.sampleDesc.Count &&
                pipelineStateDesc.sampleDesc.Quality == other.pipelineStateDesc.sampleDesc.Quality &&
                pipelineStateDesc.nodeMask == other.pipelineStateDesc.nodeMask &&
                pipelineStateDesc.flags == other.pipelineStateDesc.flags &&
                std::equal(pipelineStateDesc.inputLayout.begin(), pipelineStateDesc.inputLayout.end(), other.pipelineStateDesc.inputLayout.begin());
        }
    };



    struct PSOParameters
    {
        ShaderConf vertexShaderConf;
        ShaderConf pixelShaderConf;

        PipelineStateSettings pipelineStateSettings;

        // Root Signature parameters
        RootSignatureConf rootSignatureConf;

        bool operator==(const PSOParameters& other) const
        {
            return
                vertexShaderConf == other.vertexShaderConf &&
                pixelShaderConf == other.pixelShaderConf &&
                pipelineStateSettings == other.pipelineStateSettings &&
                rootSignatureConf == other.rootSignatureConf;
        }
    };

    struct PSOParametersHasher
    {
        std::size_t operator()(const PSOParameters& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, k.vertexShaderConf);
            hash_combine(seed, k.pixelShaderConf);
            hash_combine(seed, k.pipelineStateSettings);
            hash_combine(seed, k.rootSignatureConf);
            return seed;
        }
    };

private:
    PSOCacheManager() {};
    ~PSOCacheManager() {};
public:

    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns></returns>
    static PSOCacheManager& getInstance()
    {
        static PSOCacheManager instance;
        return instance;
    }

    std::shared_ptr<PipelineStateObject> getPSO(ID3D12Device* device, const PSOParameters params) {
        auto it = this->psoCache.find(params);
        // �L���b�V���ɂ��邩�m�F
        if (it != this->psoCache.end()) {
            // �L���b�V���ɂ���ꍇ�͂����Ԃ�
            return it->second;
        }

        // �L���b�V���ɂȂ��ꍇ�͐V�K�쐬
        psoCache[params] = createPSO(device, params);;

        if (psoCache[params] == nullptr) {
            throw std::runtime_error("PSO�̍쐬�Ɏ��s���܂����B");
        }

        return psoCache[params];
    }

private:
    std::shared_ptr<PipelineStateObject> createPSO(ID3D12Device* device, const PSOParameters params)
    {
        PipelineStateSettings pss = params.pipelineStateSettings;

        // �V�F�[�_�[��ǂݍ���
        pss.shaderPair = loadShader(params);

        //���[�g�V�O�j�`���[���쐬
        pss.rootSignature = createRootSignature(device, params.rootSignatureConf);

        //PSO���쐬
        return createPipelineStateObject(device, params.pipelineStateSettings);
    }

    /// <summary>
    /// �V�F�[�_�[��ǂݍ���
    /// </summary>
    /// <param name="params">�V�F�[�_�[����ێ����Ă���\����</param>
    /// <returns></returns>
    BasicShaderPair loadShader(const PSOParameters params)
    {
        BasicShaderPair shaderPair = {};

        shaderPair.vertexShader = ShaderCacheManager::getInstance().getOrCreate(params.vertexShaderConf);
        shaderPair.pixelShader = ShaderCacheManager::getInstance().getOrCreate(params.pixelShaderConf);

        return shaderPair;
    }

    /// <summary>
    /// ���[�g�V�O�j�`�����쐬����
    /// </summary>
    /// <param name="conf">���[�g�V�O�j�`������or�����Ɏg�p����ݒ�</param>
    /// <returns></returns>
    std::shared_ptr<RootSignature> createRootSignature(ID3D12Device* device, const RootSignatureConf conf)
    {
        return RootSignatureCacheManager::GetInstance().getOrCreate(device, conf);
    }

    /// <summary>
    /// �p�C�v���C���X�e�[�g�I�u�W�F�N�g���쐬����
    /// </summary>
    /// <param name="device">�����f�o�C�X</param>
    /// <param name="pss">�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐������Ɏg�p����ݒ�</param>
    /// <returns></returns>
    std::shared_ptr<PipelineStateObject> createPipelineStateObject(ID3D12Device* device, const PipelineStateSettings pss)
    {
        // Setup PipelineStateObject configuration
        PipelineStateObjectConf psoConf = {};
        psoConf.device = device;
        psoConf.desc = { 0 };
        psoConf.desc.StreamOutput = pss.pipelineStateDesc.streamOutput;
        psoConf.desc.BlendState = pss.pipelineStateDesc.blendState;
        psoConf.desc.SampleMask = pss.pipelineStateDesc.sampleMask;
        psoConf.desc.RasterizerState = pss.pipelineStateDesc.rasterizerState;
        psoConf.desc.DepthStencilState = pss.pipelineStateDesc.depthStencilState;
        psoConf.desc.InputLayout = { pss.pipelineStateDesc.inputLayout.data(), static_cast<UINT>(pss.pipelineStateDesc.inputLayout.size()) };
        psoConf.desc.IBStripCutValue = pss.pipelineStateDesc.iBStripCutValue;
        psoConf.desc.PrimitiveTopologyType = pss.pipelineStateDesc.primitiveTopologyType;
        psoConf.desc.NumRenderTargets = pss.pipelineStateDesc.numRenderTargets;
        psoConf.desc.RTVFormats[0] = pss.pipelineStateDesc.rtvFormat;
        psoConf.desc.DSVFormat = pss.pipelineStateDesc.dsvFormat;
        psoConf.desc.SampleDesc = pss.pipelineStateDesc.sampleDesc;
        psoConf.desc.NodeMask = pss.pipelineStateDesc.nodeMask;
        psoConf.desc.Flags = pss.pipelineStateDesc.flags;


        // Set InputLayout, VS, PS, and RootSignature
        psoConf.desc.InputLayout = { pss.pipelineStateDesc.inputLayout.data(), (UINT)pss.pipelineStateDesc.inputLayout.size() };
        psoConf.desc.VS = CD3DX12_SHADER_BYTECODE(pss.shaderPair.vertexShader->getShaderBlob());
        psoConf.desc.PS = CD3DX12_SHADER_BYTECODE(pss.shaderPair.pixelShader->getShaderBlob());
        psoConf.desc.pRootSignature = pss.rootSignature->getRootSignature();

        // Create PipelineStateObject
        std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
        pso->init(psoConf);

        return pso;
    }

private:
    //std::map<PSOParameters, std::shared_ptr<PipelineStateObject>> psoCache;
    std::unordered_map<PSOParameters, std::shared_ptr<PipelineStateObject>, PSOParametersHasher, std::equal_to<PSOParameters>> psoCache;

};

