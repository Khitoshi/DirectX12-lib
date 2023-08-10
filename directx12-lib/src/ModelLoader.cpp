#include "ModelLoader.h"
#include "ModelData.h"
#include "DescriptorHeap.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "TextureCacheManager.h"
#include "StringConvertUtil.h"
//頂点情報解析&格納
std::vector<ModelData::MeshVertex> parseVertex(const aiMesh* mesh);
//インデックス情報解析&格納
std::vector<ModelData::MeshFace> parseIndex(const aiMesh* mesh);
//テクスチャ情報解析&格納
ModelData::Material parseTexture(const aiMaterial* material, const UINT num_materials);

std::shared_ptr<ModelData> ModelLoader::load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
{
    //TODO:リファクタリング対象
    //TODO:モデルファイルの読み込みが遅いので、別スレッドで読み込むようにする
    //TODO:キャッシュを作る
    //TODO:mNumMeshesが1の場合にのみ対応しているので、複数メッシュに対応する

    Assimp::Importer importer;
    //モデルファイルを読み込む
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

    // マテリアルデータを変換.
    for (size_t i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* material = scene->mMaterials[i];
        model_data->addMaterial(parseTexture(material, scene->mNumMaterials));
    }

    //解放
    importer.FreeScene();
    scene = nullptr;

    return model_data;
}


std::vector<ModelData::MeshVertex> parseVertex(const aiMesh* mesh)
{
    //頂点情報
    const UINT num_vertex = mesh->mNumVertices;
    std::vector<ModelData::MeshVertex> vertices(num_vertex);
    for (UINT i = 0; i < num_vertex; i++)
    {
        ModelData::MeshVertex vertex = {};

        //頂点座標
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        //uv座標

        if (mesh->HasTextureCoords(0)) {
            vertex.uv.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
            vertex.uv.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
        }

        //法線ベクトル
        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        //接線ベクトル
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
        }

        //TODO:頂点色を取得する

        vertices[i] = vertex;
    }

    return vertices;
}


std::vector<ModelData::MeshFace> parseIndex(const aiMesh* mesh)
{
    //インデックス情報
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
/// テクスチャ情報解析&格納
/// </summary>
/// <param name="material">マテリアル</param>
/// <param name="num_materials">マテリアル数</param>
/// <returns>
/// 解析したテクスチャ情報
/// </returns>
ModelData::Material parseTexture(const aiMaterial* material, const UINT num_materials)
{
    ModelData::Material materials = {};

    {//拡散反射成分
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

    {//鏡面反射成分
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

    {//鏡面反射強度
        auto shininess = 0.0f;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            materials.shininess = shininess;
        }
        else {
            materials.shininess = 0.0f;
        }
    }

    {// ディフューズマップ.
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS) {
            materials.diffuse_map = Convert(path);
        }
        else {
            materials.diffuse_map.clear();
        }
    }

    {//スペキュラーマップ
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == AI_SUCCESS) {
            materials.specular_map = Convert(path);
        }
        else {
            materials.specular_map.clear();
        }
    }

    {//シャイネスマップ
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_SHININESS(0), path) == AI_SUCCESS) {
            materials.shininess_map = Convert(path);
        }
        else {
            materials.shininess_map.clear();
        }
    }

    {// 法線マップ
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
