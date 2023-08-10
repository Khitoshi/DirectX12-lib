#include "ModelLoader.h"
#include "ModelData.h"
#include "DescriptorHeap.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "TextureCacheManager.h"
#include "StringConvertUtil.h"
//���_�����&�i�[
std::vector<ModelData::MeshVertex> parseVertex(const aiMesh* mesh);
//�C���f�b�N�X�����&�i�[
std::vector<ModelData::MeshFace> parseIndex(const aiMesh* mesh);
//�e�N�X�`�������&�i�[
ModelData::Material parseTexture(const aiMaterial* material, const UINT num_materials);

std::shared_ptr<ModelData> ModelLoader::load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
{
    //TODO:���t�@�N�^�����O�Ώ�
    //TODO:���f���t�@�C���̓ǂݍ��݂��x���̂ŁA�ʃX���b�h�œǂݍ��ނ悤�ɂ���
    //TODO:�L���b�V�������
    //TODO:mNumMeshes��1�̏ꍇ�ɂ̂ݑΉ����Ă���̂ŁA�������b�V���ɑΉ�����

    Assimp::Importer importer;
    //���f���t�@�C����ǂݍ���
    int flag = 0;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_JoinIdenticalVertices;
    flag |= aiProcess_SortByPType;
    const aiScene* scene = importer.ReadFile(model_file_path, flag);
    if (!scene) {
        const std::string err = importer.GetErrorString();
        throw std::runtime_error("Failed to load model file." + err);
    }

    std::shared_ptr<ModelData> model_data = std::make_shared<ModelData>();
    UINT mesh_num = scene->mNumMeshes;
    for (UINT i = 0; i < mesh_num; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        model_data->setVertices(parseVertex(mesh));
        model_data->setFaces(parseIndex(mesh));
    }

    // �}�e���A���f�[�^��ϊ�.
    for (size_t i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* material = scene->mMaterials[i];
        model_data->addMaterial(parseTexture(material, scene->mNumMaterials));
    }

    //���
    importer.FreeScene();
    scene = nullptr;

    return model_data;
}


std::vector<ModelData::MeshVertex> parseVertex(const aiMesh* mesh)
{
    //���_���
    const UINT num_vertex = mesh->mNumVertices;
    std::vector<ModelData::MeshVertex> vertices(num_vertex);
    for (UINT i = 0; i < num_vertex; i++)
    {
        ModelData::MeshVertex vertex = {};

        //���_���W
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        //uv���W

        if (mesh->HasTextureCoords(0)) {
            vertex.uv.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
            vertex.uv.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
        }

        //�@���x�N�g��
        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        //�ڐ��x�N�g��
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
        }

        //TODO:���_�F���擾����

        vertices[i] = vertex;
    }

    return vertices;
}


std::vector<ModelData::MeshFace> parseIndex(const aiMesh* mesh)
{
    //�C���f�b�N�X���
    const UINT num_faces = mesh->mNumFaces;
    std::vector<ModelData::MeshFace> faces(num_faces);
    for (UINT i = 0; i < num_faces; i++)
    {
        faces[i].index[0] = mesh->mFaces[i].mIndices[0];
        faces[i].index[1] = mesh->mFaces[i].mIndices[1];
        faces[i].index[2] = mesh->mFaces[i].mIndices[2];
    }
    return faces;
}


/// <summary>
/// �e�N�X�`�������&�i�[
/// </summary>
/// <param name="material">�}�e���A��</param>
/// <param name="num_materials">�}�e���A����</param>
/// <returns>
/// ��͂����e�N�X�`�����
/// </returns>
ModelData::Material parseTexture(const aiMaterial* material, const UINT num_materials)
{
    ModelData::Material materials = {};

    {//�g�U���ː���
        aiColor3D color(0.0f, 0.0f, 0.0f);
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            materials.diffuse.x = color.r;
            materials.diffuse.y = color.g;
            materials.diffuse.z = color.b;
        }
        else {
            materials.diffuse.x = 0.5f;
            materials.diffuse.y = 0.5f;
            materials.diffuse.z = 0.5f;
        }
    }

    {//���ʔ��ː���
        aiColor3D color(0.0f, 0.0f, 0.0f);
        if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            materials.specular.x = color.r;
            materials.specular.y = color.g;
            materials.specular.z = color.b;
        }
        else {
            materials.specular.x = 0.0f;
            materials.specular.y = 0.0f;
            materials.specular.z = 0.0f;
        }
    }

    {//���ʔ��ˋ��x
        auto shininess = 0.0f;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            materials.shininess = shininess;
        }
        else {
            materials.shininess = 0.0f;
        }
    }

    {// �f�B�t���[�Y�}�b�v.
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS) {
            materials.diffuse_map = Convert(path);
        }
        else {
            materials.diffuse_map.clear();
        }
    }

    {//�X�y�L�����[�}�b�v
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == AI_SUCCESS) {
            materials.specular_map = Convert(path);
        }
        else {
            materials.specular_map.clear();
        }
    }

    {//�V���C�l�X�}�b�v
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_SHININESS(0), path) == AI_SUCCESS) {
            materials.shininess_map = Convert(path);
        }
        else {
            materials.shininess_map.clear();
        }
    }

    {// �@���}�b�v
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == AI_SUCCESS) {
            materials.normal_map = Convert(path);
        }
        else {
            if (material->Get(AI_MATKEY_TEXTURE_HEIGHT(0), path) == AI_SUCCESS) {
                materials.normal_map = Convert(path);
            }
            else {
                materials.normal_map.clear();
            }
        }
    }

    return materials;
}
