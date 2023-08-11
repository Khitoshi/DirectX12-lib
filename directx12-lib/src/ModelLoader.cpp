#include "ModelLoader.h"
#include "ModelData.h"
#include "DescriptorHeap.h"
#include <vector>

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include "TextureFactory.h"


//  法線を解析
DirectX::XMFLOAT3 parseNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index);
//  uv座標を解析
DirectX::XMFLOAT2 parseUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uvElement);
//  マテリアルを解析
std::shared_ptr<Texture> parseMaterial(ID3D12Device* device, DescriptorHeap* descriptor_heap, const fbxsdk::FbxNode* node);

//TODO: 未作成
std::shared_ptr<ModelData> ModelLoader::load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
{
    // FBX SDKを初期化
    auto manager = FbxManager::Create();
    auto importer = FbxImporter::Create(manager, "");
    if (!importer->Initialize(model_file_path, -1, manager->GetIOSettings())) {
        throw std::exception("Failed to load FBX file");
    }

    // シーンを作成
    auto scene = FbxScene::Create(manager, "");
    if (!importer->Import(scene)) {
        throw std::exception("Failed to import scene");
    }
    importer->Destroy();

    // 三角ポリゴンへのコンバート
    FbxGeometryConverter geometryConverter(manager);
    if (!geometryConverter.Triangulate(scene, true)) {
        throw std::exception("Failed to triangulate scene");
    }

    std::shared_ptr<ModelData> model_data = std::make_shared<ModelData>();

    // ルートノードを取得
    FbxNode* rootNode = scene->GetRootNode();
    const UINT child_count = rootNode->GetChildCount();
    for (UINT i = 0; i < child_count; i++)
    {
        FbxNode* childNode = rootNode->GetChild(i);
        FbxMesh* mesh = childNode->GetMesh();
        if (!mesh) continue;

        //法線を取得
        FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
        //UVを取得
        FbxGeometryElementUV* uvElement = mesh->GetElementUV();

        // インデックスを取得
        for (UINT polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); ++polygonIndex) {
            UINT polygonSize = mesh->GetPolygonSize(polygonIndex); // ポリゴンの頂点数（通常は3または4）

            // このポリゴンが使用する頂点情報を取得
            for (UINT vertexIndex = 0; vertexIndex < polygonSize; ++vertexIndex) {
                ModelData::USHORT controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
                FbxVector4 vertex = mesh->GetControlPointAt(controlPointIndex);
                ModelData::Vertex vertices = {};

                //頂点を取得
                vertices.position = DirectX::XMFLOAT3(DirectX::XMFLOAT3(vertex[0], vertex[1], vertex[2]));

                //法線を取得
                if (normalElement) vertices.normal = parseNormal(*mesh, polygonIndex, vertexIndex);

                //UVを取得
                if (uvElement) vertices.uv = parseUV(*mesh, polygonIndex, vertexIndex, *uvElement);

                UINT newIndex;
                auto itr = std::find(model_data->getVertices().begin(), model_data->getVertices().end(), vertices);
                if (itr == model_data->getVertices().end()) {
                    model_data->addVertices(vertices);
                    newIndex = model_data->getVertices().size() - 1;
                }
                else {
                    newIndex = std::distance(model_data->getVertices().begin(), itr);
                }

                model_data->addIndices(newIndex);
            }
        }
        //マテリアルの取得
        model_data->setDiffuseTexture(parseMaterial(device, descriptor_heap, childNode));
    }

    return model_data;
}


DirectX::XMFLOAT3 parseNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index)
{
    FbxVector4 normal;
    mesh.GetPolygonVertexNormal(polygon_index, vertex_index, normal);
    return DirectX::XMFLOAT3(DirectX::XMFLOAT3(normal[0], normal[1], normal[2]));
}

DirectX::XMFLOAT2 parseUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uvElement)
{
    FbxVector2 uv;
    bool unmapped;
    mesh.GetPolygonVertexUV(polygon_index, vertex_index, uvElement.GetName(), uv, unmapped);
    return DirectX::XMFLOAT2(uv[0], 1.0f - uv[1]);
}

std::shared_ptr<Texture> parseMaterial(ID3D12Device* device, DescriptorHeap* descriptor_heap, const fbxsdk::FbxNode* node)
{
    std::shared_ptr<Texture> texture;

    int materialCount = node->GetMaterialCount();
    for (int mat_i = 0; mat_i < materialCount; mat_i++) {
        //マテリアルの取得
        FbxSurfaceMaterial* material = node->GetMaterial(mat_i);
        if (!material) continue;

        //テクスチャの数を確認 0以下なら次のマテリアルへ
        FbxProperty tex_prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
        int texture_count = tex_prop.GetSrcObjectCount<FbxFileTexture>();
        if (texture_count < 0)continue;

        //テクスチャの取得
        FbxFileTexture* texFile = tex_prop.GetSrcObject<FbxFileTexture>();
        if (!texFile) continue;

        //テクスチャのロード
        const char* texture_file_name = texFile->GetFileName();
        texture = TextureFactory::create(device, descriptor_heap, texture_file_name);

    }
    return texture;
}
