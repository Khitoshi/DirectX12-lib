#include "ModelLoader.h"
#include "ModelData.h"
#include "DescriptorHeap.h"
#include <vector>
#include <fbxsdk.h>


//TODO: 未作成
std::shared_ptr<ModelData> ModelLoader::load(const char* model_file_path)
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
                if (normalElement) {
                    FbxVector4 normal;
                    mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal);
                    vertices.normal = DirectX::XMFLOAT3(DirectX::XMFLOAT3(normal[0], normal[1], normal[2]));
                }
                //UVを取得
                if (uvElement) {
                    FbxVector2 uv;
                    bool unmapped;
                    mesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvElement->GetName(), uv, unmapped);
                    vertices.uv = DirectX::XMFLOAT2(uv[0], uv[1]);
                }

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


        /*
        FbxProperty* texProp = &childNode->FindProperty(FbxSurfaceMaterial::sDiffuse);
        if (texProp->IsValid()) {
            FbxFileTexture* texFile = texProp->GetSrcObject<FbxFileTexture>();
            if (texFile) {
                const char* texFileName = texFile->GetFileName();
                // ここでtexFileNameを使用してテクスチャをロードします
            }
        }
        */

    }



    return model_data;
}
