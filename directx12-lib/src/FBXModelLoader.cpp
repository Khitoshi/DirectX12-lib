#include "FBXModelLoader.h"
#include "FBXModelData.h"
#include "DescriptorHeap.h"
#include <vector>
#include <unordered_map>
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include "TextureCacheManager.h"


//  法線を解析
DirectX::XMFLOAT3 parseNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index);
//  uv座標を解析
DirectX::XMFLOAT2 parseUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uvElement);
//  マテリアルを解析
FBXModelData::Material parseMaterial(ID3D12Device* device, DescriptorHeap* descriptor_heap,const fbxsdk::FbxNode* node);

/// <summary>
/// モデルのロード
/// </summary>
/// <param name="device">GPUデバイス</param>
/// <param name="descriptor_heap">ディスクリプタヒープ</param>
/// <param name="model_file_path">モデルのファイルパス</param>
/// <returns>
/// モデルデータのインスタンス
/// </returns>
std::shared_ptr<FBXModelData> FBXModelLoader::load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
{
    // FBX SDKを初期化
    auto manager = FbxManager::Create();
    auto importer = FbxImporter::Create(manager, "");
    if (!importer->Initialize(model_file_path, -1, manager->GetIOSettings())) {
        FbxString error = importer->GetStatus().GetErrorString();
        auto err_message = "Failed to load FBX file" + std::string(error.Buffer());
        throw std::exception(err_message.c_str());
    }

    // シーンを作成
    auto scene = FbxScene::Create(manager, "");
    if (!importer->Import(scene)) {
        FbxString error = importer->GetStatus().GetErrorString();
        auto err_message = "Failed to import scene" + std::string(error.Buffer());
        throw std::exception(err_message.c_str());
    }
    importer->Destroy();

    // 三角ポリゴンへのコンバート
    FbxGeometryConverter geometry_converter(manager);
    if (!geometry_converter.Triangulate(scene, true)) {
        FbxString error = importer->GetStatus().GetErrorString();
        auto err_message = "Failed to triangulate scene" + std::string(error.Buffer());
        throw std::exception(err_message.c_str());
    }

    // 最終的に使用するモデルデータのインスタンスを生成
    std::shared_ptr<FBXModelData> model_data = std::make_shared<FBXModelData>();

    //インデックスの計算をするためのマップ
    std::unordered_map<FBXModelData::Vertex, FBXModelData::USHORT, FBXModelData::ModelDataKeyHasher, std::equal_to<FBXModelData::Vertex>> index_map;

    // ルートノードを取得
    FbxNode* root_node = scene->GetRootNode();
    const UINT child_count = root_node->GetChildCount();
    for (UINT child_i = 0; child_i < child_count; child_i++)
    {
        FbxNode* child_node = root_node->GetChild(child_i);
        FbxMesh* mesh = child_node->GetMesh();
        if (!mesh) continue;

        //法線を取得
        FbxGeometryElementNormal* normal_element = mesh->GetElementNormal();
        //UVを取得
        FbxGeometryElementUV* uv_element = mesh->GetElementUV();

        for (UINT poly_i = 0; poly_i < mesh->GetPolygonCount(); ++poly_i) {
            // ポリゴンの頂点数（通常は3または4）
            UINT polygon_size = mesh->GetPolygonSize(poly_i);
            if (polygon_size != 3) throw std::exception("Polygon size is not 3");

            // このポリゴンが使用する頂点情報を取得
            for (UINT ver_i = 0; ver_i < polygon_size; ++ver_i) {
                FBXModelData::USHORT control_point_index = mesh->GetPolygonVertex(poly_i, ver_i);
                FbxVector4 vertex = mesh->GetControlPointAt(control_point_index);
                FBXModelData::Vertex vertices = {};

                //頂点を取得
                vertices.position = DirectX::XMFLOAT3(DirectX::XMFLOAT3(vertex[0], vertex[1], vertex[2]));

                //法線を取得
                if (normal_element) vertices.normal = parseNormal(*mesh, poly_i, ver_i);

                //UVを取得
                if (uv_element) vertices.uv = parseUV(*mesh, poly_i, ver_i, *uv_element);

                //頂点のインデックスを取得
                UINT new_index;
                auto itr = index_map.find(vertices);
                if (itr == index_map.end()) {
                    model_data->addVertices(vertices);
                    new_index = model_data->getVertices().size() - 1;
                    index_map[vertices] = new_index;
                }
                else {
                    new_index = itr->second;
                }

                model_data->addIndices(new_index);
            }
        }
        //マテリアルの取得
        //model_data->setDiffuseTexture(parseMaterial(device, child_node));
        //model_data->getDiffuseTexture()->CreateShaderResourceView(device, descriptor_heap, 1);
        model_data->addMaterial(parseMaterial(device, descriptor_heap, child_node));

    }

    // FBX SDKを破棄
    scene->Destroy();
    manager->Destroy();

    return model_data;
}

/// <summary>
/// 法線情報の解析
/// </summary>
/// <param name="mesh">メッシュ</param>
/// <param name="polygon_index">ポリゴン番号</param>
/// <param name="vertex_index">頂点番号</param>
/// <returns>
/// 解析した法線情報
/// </returns>
DirectX::XMFLOAT3 parseNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index)
{
    FbxVector4 normal;
    mesh.GetPolygonVertexNormal(polygon_index, vertex_index, normal);
    return DirectX::XMFLOAT3(DirectX::XMFLOAT3(normal[0], normal[1], normal[2]));
}

/// <summary>
/// UV情報の解析
/// </summary>
/// <param name="mesh">メッシュ</param>
/// <param name="polygon_index">ポリゴン番号</param>
/// <param name="vertex_index">頂点番号</param>
/// <param name="uvElement">uv情報</param>
/// <returns>
/// 解析したUV情報
/// </returns>
DirectX::XMFLOAT2 parseUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uv_element)
{
    FbxVector2 uv;
    bool unmapped;
    mesh.GetPolygonVertexUV(polygon_index, vertex_index, uv_element.GetName(), uv, unmapped);
    return DirectX::XMFLOAT2(uv[0], 1.0f - uv[1]);
}

/// <summary>
/// テクスチャの解析
/// </summary>
/// <param name="device">GPUデバイス</param>
/// <param name="descriptor_heap">テクスチャ情報を入れたいディスクリプタヒープ</param>
/// <param name="node">fbxのnode</param>
/// <returns>
/// テクスチャのインスタンス
/// </returns>
FBXModelData::Material parseMaterial(ID3D12Device* device, DescriptorHeap* descriptor_heap, const fbxsdk::FbxNode* node)
{
    FBXModelData::Material model_texture;
    int material_count = node->GetMaterialCount();
    for (int mat_i = 0; mat_i < material_count; mat_i++) {
        //マテリアルの取得
        FbxSurfaceMaterial* material = node->GetMaterial(mat_i);
        if (!material) continue;

        //テクスチャの数を確認 0以下なら次のマテリアルへ
        FbxProperty tex_prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
        //DiffuseMapの取得
        if (tex_prop.IsValid()) {
            const FbxDouble3 color = tex_prop.Get<FbxDouble3>();
            model_texture.shader_material.diffuse_color.x = static_cast<float>(color[0]);
            model_texture.shader_material.diffuse_color.y = static_cast<float>(color[1]);
            model_texture.shader_material.diffuse_color.z = static_cast<float>(color[2]);
            model_texture.shader_material.diffuse_color.w = 1.0f;

            const FbxFileTexture* fbx_texture = tex_prop.GetSrcObject<FbxFileTexture>();
            //model_texture.diffuse_texture_name = fbx_texture ? fbx_texture->GetRelativeFileName() : "asset/models/dummy/dummy.DDS";
            model_texture.diffuse_texture_name = fbx_texture ? fbx_texture->GetFileName() : "";
        }
    }

    if (!model_texture.diffuse_texture_name.empty()) {
        model_texture.texture = TextureCacheManager::getInstance().getOrCreate(device, model_texture.diffuse_texture_name.c_str());
        model_texture.texture->CreateShaderResourceView(device, descriptor_heap, 2);
    }
    else {//ダミーテクスチャ
        model_texture.texture = TextureCacheManager::getInstance().getOrCreate(device, "asset/models/dummy/dummy.DDS");
        model_texture.texture->CreateShaderResourceView(device, descriptor_heap, 2);


    }

    return model_texture;
}
