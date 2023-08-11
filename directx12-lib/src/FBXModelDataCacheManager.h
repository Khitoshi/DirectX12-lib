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


        return FBXModelDataFactory::create(device, descriptor_heap, model_file_path);
    }


private:

};
