#pragma once

#include "FBXModelData.h"
#include "FBXModelDataFactory.h"

class FBXModelDataCacheManager
{
private:
    FBXModelDataCacheManager() {};
    ~FBXModelDataCacheManager() {};
public:
    /// <summary>
    /// �V���O���g���ȃC���X�^���X���擾
    /// </summary>
    /// <returns>
    /// �V���O���g���ȃC���X�^���X
    /// </returns>
    static FBXModelDataCacheManager& getInstance()
    {
        static FBXModelDataCacheManager instance;
        return instance;
    }

    std::shared_ptr<FBXModelData> getOrCreate(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
    {
        //TODO:���݃e�N�X�`���ł̓f�B�X�N���v�^�q�[�v�ɂ����ɓ���Ă���̂����\�[�X�̍쐬�����ɂ���
        //�蓮�Ńf�B�X�N���v�^�q�[�v�ɓ����悤�ɂ���

        //�L���b�V���ɂ��邩�ǂ���
        /*
        auto it = fbx_model_cache_.find(model_file_path);
        if (it != fbx_model_cache_.end())
        {
            it->second->
        }
        return FBXModelDataFactory::create(device, model_file_path);

        */
        return nullptr;
    }

private:
    std::unordered_map<const std::string, std::shared_ptr<FBXModelData>> fbx_model_cache_;  //�L���b�V��

};
