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
void ProcessMesh(aiMesh* mesh, const aiScene* scene, Mesh& newMesh, const aiMatrix4x4& globalTransform, const char* model_file_path, bool inverse_u, bool inverse_v);
std::vector<Vertex> FetchVertices(const aiMesh* src, const aiMatrix4x4& transform, bool inverseU, bool inverseV);
std::vector<uint32_t> FetchIndices(const aiMesh* src);
Material FetchMaterial(const char* filename, const aiScene* scene, const aiMaterial* src);
void CreateTextureFile(const char* filename, const aiScene* scene, const aiMaterial* src);


std::vector<Mesh> AssimpLoader::Load(const char* model_file_path, bool inverse_u, bool inverse_v)
{
    if (model_file_path == nullptr) {
        throw std::exception("AssimpLoader::Loader model_file_path is empty");
    }

    //sceneを読み込む
    int flag = 0;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_SortByPType;
    flag |= aiProcess_GenUVCoords;
    flag |= aiProcess_OptimizeMeshes;
    flag |= aiProcess_ValidateDataStructure;
    flag |= aiProcess_GenNormals;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_LimitBoneWeights;
    flag |= aiProcess_JoinIdenticalVertices;
    flag |= aiProcess_FlipWindingOrder;
    Assimp::Importer importer = {};
    const aiScene* scene = importer.ReadFile(model_file_path, flag);
    if (scene == nullptr) {
        throw std::exception("AssimpLoader::Loader Failed ReadFile");
    }

    //メッシュを読み込む
    std::vector<Mesh> meshes;
    ProcessNode(/*node*/scene->mRootNode, scene, meshes, aiMatrix4x4(), model_file_path, inverse_u, inverse_v);

    return meshes;
}

void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const aiMatrix4x4& parentTransform, const char* model_file_path, bool inverse_u, bool inverse_v)
{
    aiMatrix4x4 globalTransform = parentTransform * node->mTransformation;

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh newMesh;
        ProcessMesh(mesh, scene, newMesh, globalTransform, model_file_path, inverse_u, inverse_v);

        meshes.push_back(newMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, meshes, globalTransform, model_file_path, inverse_u, inverse_v);
    }
}

void ProcessMesh(aiMesh* mesh, const aiScene* scene, Mesh& newMesh, const aiMatrix4x4& globalTransform, const char* model_file_path, bool inverse_u, bool inverse_v) {
    newMesh.vertices = FetchVertices(mesh, globalTransform, inverse_u, inverse_v);
    newMesh.indices = FetchIndices(mesh);
    const auto material_i = mesh->mMaterialIndex;
    const auto material = scene->mMaterials[material_i];
    newMesh.material = FetchMaterial(model_file_path, scene, material);

    // ボーン情報の処理
    /*
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        const aiBone* ai_bone = mesh->mBones[i];
        Bone bone;
        bone.name = ai_bone->mName.C_Str();
        bone.transform = ai_bone->mOffsetMatrix;
        newMesh.bones.push_back(bone);
    }
    */
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

    CreateTextureFile(filename, scene, src);

    Material material = {};
    aiString path;
    if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS) {
        auto file = GetFileNameFromPath(std::string(path.C_Str()));
        material.diffuse_map_name = std::string(filename) + ".mat/" + file;
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

void CreateTextureFile(const char* file_path, const aiScene* scene, const aiMaterial* src)
{

    /*
    * テクスチャファイルを作成する
    *
    * モデルファイルに埋め込まれているテクスチャをDirectXTexを使用してファイルとして出力する
    * また既にその名前のファイルやディレクトリが存在する場合はスキップする
    *
    */
    auto fn = [](const aiTextureType type, const aiMaterial* src, const aiScene* scene, const std::string& file_path) {
        aiString texture_path;
        for (UINT i = 0; src->GetTexture(type, i, &texture_path) == AI_SUCCESS; i++) {
            // テクスチャが存在しない場合はスキップ
            const aiTexture* embedded_texture = scene->GetEmbeddedTexture(texture_path.C_Str());
            if (!embedded_texture)continue;

            //すでにファイルが存在する場合はスキップ
            /*
            * モデルのディレクトリ + .mat/ + テクスチャのファイル名
            * hoge/hoge.fbx -> hoge/hoge.matl/texture.png
            */
            const std::string after_file_path = file_path + ".mat/" + GetFileNameFromPath(std::string(texture_path.C_Str()));
            if (DoesPathExist(after_file_path)) continue;

            // DirectXTexで画像をロード
            const unsigned char* texture_data = reinterpret_cast<const unsigned char*>(embedded_texture->pcData);
            const size_t texture_size = embedded_texture->mWidth; // 画像サイズを適切に設定する必要があります
            DirectX::ScratchImage scratchImg = {};
            if (FAILED(DirectX::LoadFromWICMemory(texture_data, texture_size, DirectX::WIC_FLAGS_NONE, nullptr, scratchImg))) {
                throw std::exception("AssimpLoader::Loader Failed LoadFromWICMemory");
            }

            // ディレクトリが存在しない場合は作成
            if (!CreateDirectoryIfNotExists(GetDirFromPath(after_file_path))) {
                throw std::exception("AssimpLoader::Loader Failed CreateDirectoryIfNotExists");
            }

            // DirectXTexで画像を保存
            const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);
            if (DirectX::SaveToWICFile(*img, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), ToWideString(after_file_path).c_str())) {
                throw std::exception("AssimpLoader::Loader Failed SaveToWICFile");
            }
        }
    };

    //すべてのテクスチャタイプに対して実行
    for (int type = aiTextureType_NONE; type != AI_TEXTURE_TYPE_MAX; type++) {
        fn(static_cast<const aiTextureType>(type), src, scene, file_path);
    }

}

