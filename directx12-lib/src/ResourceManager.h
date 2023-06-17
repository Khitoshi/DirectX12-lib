#pragma once
#include <string>
#include <memory>
#include <map>

/// <summary>
/// ���\�[�X�}�l�[�W���[
/// �쐬�������\�[�X��o�^����
/// �G���[�����������ꍇ��
/// �A�v���P�[�V�������I���ꍇ
/// �S�ĉ������
/// </summary>
class ResourceManager
{
private:
    ResourceManager() {};
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
public:

    /// <summary>
    /// �C���X�^���X�̎擾
    /// �V���O���g���ō쐬����Ă��邽��
    /// �O���[�o���Ŏg�p�\
    /// </summary>
    /// <returns></returns>
    static ResourceManager* getInstance() {
        static ResourceManager instance;
        return &instance;
    }

    /// <summary>
    /// ���\�[�X�̓o�^
    /// �G���[��I�����ɉ�����邽��
    /// </summary>
    /// <typeparam name="T">�Ǘ����������\�[�X�̌^</typeparam>
    /// <param name="name">�Ǘ��p�̖��O</param>
    /// <param name="resource">���\�[�X�{��</param>
    template <typename T>
    void registerResource(const std::string& name, std::shared_ptr<T> resource) {
        //���O�Ō���
        auto it = resources.find(name);
        //���ɓ������O�œo�^����Ă��Ȃ����m�F
        if (it != resources.end()) {
            //�o�^�ς݂̏ꍇ�̓G���[
            throw std::runtime_error("Resource already exists: " + name);
            return;
        }

        //�}���`�X���b�h�p�̃��b�N
        //std::lock_guard<std::mutex> lock(mtx);

        //�o�^
        resources[name] = std::move(resource);
    }

    /// <summary>
    /// ���\�[�X�̎擾
    /// </summary>
    /// <param name="name">���\�[�X�̊Ǘ��p���O</param>
    /// <param name="resource">���\�[�X�{��</param>
    template<typename T>
    std::shared_ptr<T> getResource(const std::string& name) {
        //�}���`�X���b�h�p�̃��b�N
        //std::lock_guard<std::mutex> lock(mtx);

        //���O�Ō���
        auto it = resources.find(name);
        if (it != resources.end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        throw std::runtime_error("Resource not found: " + name);
    }

private:
    //���\�[�X�̊Ǘ��p
    std::map<std::string, std::shared_ptr<void>> resources;
    //�}���`�X���b�h�p�̃��b�N
    //std::mutex mtx;
};
