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

std::vector<Vertex> FetchVertices(const aiMesh* src, bool inverseU, bool inverseV);
std::vector<uint32_t> FetchIndices(const aiMesh* src);
Material FetchMaterial(const char* filename, const aiScene* scene, const aiMaterial* src);
void CreateTextureFile(const char* filename, const aiScene* scene, const aiMaterial* src, Material& material);
std::vector<Mesh> AssimpLoader::Load(const char* model_file_path, bool inverse_u, bool inverse_v)
{
    if (model_file_path == nullptr) {
        throw std::exception("AssimpLoader::Loader model_file_path is empty");
    }

    //インポーター作成
    Assimp::Importer importer = {};
    /*
    int flag = 0;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_PreTransformVertices;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_GenSmoothNormals;
    flag |= aiProcess_GenUVCoords;
    flag |= aiProcess_RemoveRedundantMaterials;
    flag |= aiProcess_OptimizeMeshes;
    */

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
    std::vector<Mesh> meshes(scene->mNumMeshes);
    for (size_t i = 0; i < meshes.size(); i++) {
        const auto mesh = scene->mMeshes[i];
        meshes[i].vertices = FetchVertices(mesh, inverse_u, inverse_v);

        meshes[i].indices = FetchIndices(mesh);

        //const auto material = scene->mMaterials[i];
        const auto material_i = mesh->mMaterialIndex; // メッシュに関連するマテリアルのインデックスを取得
        const auto material = scene->mMaterials[material_i];
        meshes[i].material = FetchMaterial(model_file_path, scene, material);
    }

    return meshes;
}

std::vector<Vertex> FetchVertices(const aiMesh* src, bool inverseU, bool inverseV)
{
    aiVector3D zero3D(0.0f, 0.0f, 0.0f);
    aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::vector<Vertex> vertices(src->mNumVertices);
    for (auto i = 0u; i < src->mNumVertices; ++i)
    {
        auto position = &(src->mVertices[i]);
        auto normal = &(src->mNormals[i]);
        auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
        auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
        auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

        if (inverseU)uv->x = 1 - uv->x;
        if (inverseV)uv->y = 1 - uv->y;

        Vertex vertex = {};
        vertex.Position = DirectX::XMFLOAT3(position->x, position->y, position->z);
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

    CreateTextureFile(filename, scene, src, material);

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

void CreateTextureFile(const char* filename, const aiScene* scene, const aiMaterial* src, Material& material)
{
    aiString texturePath;
    const auto dir = GetDirFromPath(filename);

    for (UINT i = 0; src->GetTexture(aiTextureType_DIFFUSE, i, &texturePath) == AI_SUCCESS; i++) {
        const aiTexture* embedded_texture = scene->GetEmbeddedTexture(texturePath.C_Str());
        // 画像をファイルに保存
        const auto filepath = dir + std::string(texturePath.C_Str());

        // テクスチャが存在しない場合&ファイルが存在する場合はスキップ
        if (!embedded_texture)continue;
        if (DoesPathExist(filepath)) continue;

        const unsigned char* texture_data = reinterpret_cast<const unsigned char*>(embedded_texture->pcData);
        const size_t texture_size = embedded_texture->mWidth; // 画像サイズを適切に設定する必要があります

        // DirectXTexで画像をロード
        DirectX::ScratchImage scratchImg;
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

}

