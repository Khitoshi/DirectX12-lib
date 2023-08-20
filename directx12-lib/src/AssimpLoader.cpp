#include "AssimpLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "d3dx12.h"
#include <filesystem>
#include "StringUtil.h"
#include "Mesh.h"
#include "FileUtil.h"
#include <DirectXTex.h>

void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const aiMatrix4x4& parentTransform, const char* model_file_path, bool inverse_u, bool inverse_v);
//std::vector<Vertex> FetchVertices(const aiMesh* src, bool inverseU, bool inverseV);
std::vector<Vertex> FetchVertices(const aiMesh* src, const aiMatrix4x4& transform, bool inverseU, bool inverseV);
std::vector<uint32_t> FetchIndices(const aiMesh* src);
Material FetchMaterial(const char* filename, const aiScene* scene, const aiMaterial* src);
void CreateTextureFile(const char* filename, const aiScene* scene, const aiMaterial* src);


std::vector<Mesh> AssimpLoader::Load(const char* model_file_path, bool inverse_u, bool inverse_v)
{
    if (model_file_path == nullptr) {
        throw std::exception("AssimpLoader::Loader model_file_path is empty");
    }

    //インポーター作成
    Assimp::Importer importer = {};
    int flag = aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_GenUVCoords |
        aiProcess_OptimizeMeshes |
        aiProcess_ValidateDataStructure |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_LimitBoneWeights |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipWindingOrder;

    const aiScene* scene = importer.ReadFile(model_file_path, flag);
    if (scene == nullptr) {
        throw std::exception("AssimpLoader::Loader Failed ReadFile");
    }

    //メッシュを読み込む
    /*
    std::vector<Mesh> meshes(scene->mNumMeshes);
    for (size_t i = 0; i < meshes.size(); i++) {
        const auto mesh = scene->mMeshes[i];
        //meshes[i].vertices = FetchVertices(mesh, inverse_u, inverse_v);

        meshes[i].indices = FetchIndices(mesh);

        //const auto material = scene->mMaterials[i];
        const auto material_i = mesh->mMaterialIndex; // メッシュに関連するマテリアルのインデックスを取得
        const auto material = scene->mMaterials[material_i];
        meshes[i].material = FetchMaterial(model_file_path, scene, material);
    }
    */


    std::vector<Mesh> meshes;
    ProcessNode(scene->mRootNode, scene, meshes, aiMatrix4x4(), model_file_path, inverse_u, inverse_v);

    return meshes;
}

void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const aiMatrix4x4& parentTransform, const char* model_file_path, bool inverse_u, bool inverse_v)
{
    aiMatrix4x4 globalTransform = parentTransform * node->mTransformation;

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh newMesh;
        newMesh.vertices = FetchVertices(mesh, globalTransform, inverse_u, inverse_v);
        newMesh.indices = FetchIndices(mesh);
        const auto material_i = mesh->mMaterialIndex;
        const auto material = scene->mMaterials[material_i];
        newMesh.material = FetchMaterial(model_file_path, scene, material);
        meshes.push_back(newMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, meshes, globalTransform, model_file_path, inverse_u, inverse_v);
    }
}

std::vector<Vertex> FetchVertices(const aiMesh* src, const aiMatrix4x4& transform, bool inverseU, bool inverseV)
{
    aiVector3D zero3D(0.0f, 0.0f, 0.0f);
    aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::vector<Vertex> vertices(src->mNumVertices);
    for (auto i = 0u; i < src->mNumVertices; ++i)
    {
        aiVector3D position = src->mVertices[i];
        aiVector3D transformed_position;
        transformed_position.x = transform.a1 * position.x + transform.a2 * position.y + transform.a3 * position.z + transform.a4;
        transformed_position.y = transform.b1 * position.x + transform.b2 * position.y + transform.b3 * position.z + transform.b4;
        transformed_position.z = transform.c1 * position.x + transform.c2 * position.y + transform.c3 * position.z + transform.c4;

        auto normal = &(src->mNormals[i]);
        auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
        auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
        auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

        if (inverseU)uv->x = 1 - uv->x;
        if (inverseV)uv->y = 1 - uv->y;

        Vertex vertex = {};
        vertex.Position = DirectX::XMFLOAT3(transformed_position.x, transformed_position.y, transformed_position.z);
        vertex.Normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
        vertex.UV = DirectX::XMFLOAT2(uv->x, uv->y);
        vertex.Tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
        vertex.Color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);

        vertices[i] = vertex;
    }

    return vertices;
}

std::vector<uint32_t> FetchIndices(const aiMesh* src)
{
    std::vector<uint32_t> indices(src->mNumFaces * 3);// 3 indices per face
    for (size_t i = 0u; i < src->mNumFaces; ++i)
    {
        const auto& face = src->mFaces[i];
        indices[i * 3 + 0] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    return indices;
}

Material FetchMaterial(const char* filename, const aiScene* scene, const aiMaterial* src)
{
    Material material = {};
    auto dir = GetDirFromPath(filename);

    CreateTextureFile(filename, scene, src);

    aiString path;
    if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS) {
        auto file = std::string(path.C_Str());
        const std::string::size_type pos = file.find('/');
        material.diffuse_map_name = dir + file;
    }
    else {
        material.diffuse_map_name = "asset/models/dummy/dummy.DDS";
    }

    // Diffuse Color
    aiColor3D color(1.f, 1.f, 1.f);
    if (src->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        material.diffuse_color = DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f);
    }
    else {
        material.diffuse_color = DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f);
    }

    return material;
}

void CreateTextureFile(const char* filename, const aiScene* scene, const aiMaterial* src)
{

    /*
    * テクスチャファイルを作成する
    *
    * モデルファイルに埋め込まれているテクスチャをDirectXTexを使用してファイルとして出力する
    * また既にその名前のファイルやディレクトリが存在する場合はスキップする
    *
    */
    auto fn = [](const aiTextureType type, const aiMaterial* src, const aiScene* scene, const std::string& dir) {
        aiString texture_path;
        for (UINT i = 0; src->GetTexture(type, i, &texture_path) == AI_SUCCESS; i++) {
            // テクスチャが存在しない場合はスキップ
            const aiTexture* embedded_texture = scene->GetEmbeddedTexture(texture_path.C_Str());
            if (!embedded_texture)continue;

            //すでにファイルが存在する場合はスキップ
            const std::string filepath = dir + std::string(texture_path.C_Str());
            if (DoesPathExist(filepath)) continue;

            // DirectXTexで画像をロード
            const unsigned char* texture_data = reinterpret_cast<const unsigned char*>(embedded_texture->pcData);
            const size_t texture_size = embedded_texture->mWidth; // 画像サイズを適切に設定する必要があります
            DirectX::ScratchImage scratchImg = {};
            if (FAILED(DirectX::LoadFromWICMemory(texture_data, texture_size, DirectX::WIC_FLAGS_NONE, nullptr, scratchImg))) {
                throw std::exception("AssimpLoader::Loader Failed LoadFromWICMemory");
            }

            // ディレクトリが存在しない場合は作成
            if (!CreateDirectoryIfNotExists(GetDirFromPath(filepath))) {
                throw std::exception("AssimpLoader::Loader Failed CreateDirectoryIfNotExists");
            }

            // DirectXTexで画像を保存
            const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);
            if (DirectX::SaveToWICFile(*img, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), ToWideString(filepath).c_str())) {
                throw std::exception("AssimpLoader::Loader Failed SaveToWICFile");
            }
        }
    };

    //すべてのテクスチャタイプに対して実行
    const std::string dir = GetDirFromPath(filename);
    for (int type = aiTextureType_NONE; type != AI_TEXTURE_TYPE_MAX; type++) {
        fn(static_cast<const aiTextureType>(type), src, scene, dir);
    }

}

