#include "Model.h"

#ifdef _DEBUG
#pragma comment(lib,"assimp-vc142-mtd.lib")
#else
#pragma comment(lib,"assimp-vc142-md.lib")
#endif // _DEBUG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>

void Model::init(ID3D12Device* device, const char* model_file_path)
{
    Assimp::Importer importer;

    //モデルファイルを読み込む
    int flag = 0;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_JoinIdenticalVertices;
    flag |= aiProcess_SortByPType;
    const aiScene* scene = importer.ReadFile(model_file_path, flag);
    //読み込みに失敗したらエラーを吐く
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("Failed to load model file.");
    }

    std::vector<ModelData::Node> nodes;

}

void Model::draw()
{
}
