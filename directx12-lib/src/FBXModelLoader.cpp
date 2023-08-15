#include "FBXModelLoader.h"
#include "FBXModelData.h"
#include "DescriptorHeap.h"
#include <vector>
#include <unordered_map>
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include "TextureCacheManager.h"


//  法線を解析
DirectX::XMFLOAT3 fetchNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index);
//  uv座標を解析
DirectX::XMFLOAT2 fetchUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uvElement);
//  マテリアルを解析
std::vector<FBXModelData::SubSet> fetchMaterial(ID3D12Device* device, DescriptorHeap* descriptor_heap, const fbxsdk::FbxNode* node);

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

        //マテリアルの取得
        UINT size = 0;
        for (auto& materials : fetchMaterial(device, descriptor_heap, child_node)) {
            model_data->addMaterial(materials);
            size = model_data->getSubsetMaps().size();
        }

        for (UINT poly_i = 0; poly_i < mesh->GetPolygonCount(); ++poly_i) {
            // ポリゴンの頂点数（通常は3または4）
            UINT polygon_size = mesh->GetPolygonSize(poly_i);
            if (polygon_size != 3) throw std::exception("Polygon size is not 3");

            //マテリアルのインデックスを取得
            //int materialIndex = mesh->GetElementMaterial()->GetIndexArray().GetAt(poly_i);
            int materialIndex = size > 0 ? mesh->GetElementMaterial()->GetIndexArray().GetAt(poly_i) : 0;
            FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(materialIndex);

            if (material) {
                uint64_t materialUniqueId = static_cast<uint64_t>(material->GetUniqueID());
                // このポリゴンが使用する頂点情報を取得
                for (UINT ver_i = 0; ver_i < polygon_size; ++ver_i) {
                    FBXModelData::USHORT control_point_index = mesh->GetPolygonVertex(poly_i, ver_i);
                    FbxVector4 vertex = mesh->GetControlPointAt(control_point_index);
                    FBXModelData::Vertex vertices = {};

                    //頂点を取得
                    vertices.position = DirectX::XMFLOAT3(DirectX::XMFLOAT3(vertex[0], vertex[1], vertex[2]));

                    //法線を取得
                    if (normal_element) vertices.normal = fetchNormal(*mesh, poly_i, ver_i);

                    //UVを取得
                    if (uv_element) vertices.uv = fetchUV(*mesh, poly_i, ver_i, *uv_element);

                    //頂点のインデックスを取得
                    UINT new_index;
                    auto itr = index_map.find(vertices);
                    if (itr == index_map.end()) {
                        model_data->addVertices(materialUniqueId, vertices);
                        //new_index = model_data->getVertices().size() - 1;
                        new_index = model_data->getVertex(materialUniqueId).size() - 1;
                        index_map[vertices] = new_index;
                    }
                    else {
                        new_index = itr->second;
                    }

                    model_data->addIndices(materialUniqueId, new_index);
                }
            }
            else {
                {
                    FBXModelData::SubSet subset;
                    subset.unique_id = 0;
                    subset.material.diffuse_map_name = "asset/models/dummy/dummy.DDS";
                    subset.material.shader_material.diffuse_color.x = .0f;
                    subset.material.shader_material.diffuse_color.y = .0f;
                    subset.material.shader_material.diffuse_color.z = .0f;
                    subset.material.shader_material.diffuse_color.w = 1.0f;
                    subset.material.material_index = 0;
                    subset.material.shader_material.diffuse_map_index = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 0;
                    subset.material.diffuse_map = TextureCacheManager::getInstance().getOrCreate(device, "asset/models/dummy/dummy.DDS");
                    subset.material.diffuse_map->CreateShaderResourceView(device, descriptor_heap, 1 + subset.material.material_index);

                    model_data->addMaterial(subset);
                }

                uint64_t materialUniqueId = static_cast<uint64_t>(0);
                // このポリゴンが使用する頂点情報を取得
                for (UINT ver_i = 0; ver_i < polygon_size; ++ver_i) {
                    FBXModelData::USHORT control_point_index = mesh->GetPolygonVertex(poly_i, ver_i);
                    FbxVector4 vertex = mesh->GetControlPointAt(control_point_index);
                    FBXModelData::Vertex vertices = {};

                    //頂点を取得
                    vertices.position = DirectX::XMFLOAT3(DirectX::XMFLOAT3(vertex[0], vertex[1], vertex[2]));

                    //法線を取得
                    if (normal_element) vertices.normal = fetchNormal(*mesh, poly_i, ver_i);

                    //UVを取得
                    if (uv_element) vertices.uv = fetchUV(*mesh, poly_i, ver_i, *uv_element);

                    //頂点のインデックスを取得
                    UINT new_index;
                    auto itr = index_map.find(vertices);
                    if (itr == index_map.end()) {
                        model_data->addVertices(materialUniqueId, vertices);
                        //new_index = model_data->getVertices().size() - 1;
                        new_index = model_data->getVertex(materialUniqueId).size() - 1;
                        index_map[vertices] = new_index;
                    }
                    else {
                        new_index = itr->second;
                    }

                    model_data->addIndices(materialUniqueId, new_index);
                }

            }


        }
    }

    // FBX SDKを破棄
    scene->Destroy();
    manager->Destroy();

    return model_data;
}


DirectX::XMFLOAT3 fetchNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index)
{
    FbxVector4 normal;
    mesh.GetPolygonVertexNormal(polygon_index, vertex_index, normal);
    return DirectX::XMFLOAT3(DirectX::XMFLOAT3(normal[0], normal[1], normal[2]));
}


DirectX::XMFLOAT2 fetchUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uv_element)
{
    FbxVector2 uv;
    bool unmapped;
    mesh.GetPolygonVertexUV(polygon_index, vertex_index, uv_element.GetName(), uv, unmapped);
    return DirectX::XMFLOAT2(uv[0], 1.0f - uv[1]);
}


std::vector<FBXModelData::SubSet> fetchMaterial(ID3D12Device* device, DescriptorHeap* descriptor_heap, const fbxsdk::FbxNode* node)
{
    std::vector<FBXModelData::SubSet> subsets = {};
    int material_count = node->GetMaterialCount();
    for (int mat_i = 0; mat_i < material_count; mat_i++) {
        FBXModelData::SubSet subset;
        //マテリアルの取得
        FbxSurfaceMaterial* material = node->GetMaterial(mat_i);
        if (!material) continue;

        subset.unique_id = material->GetUniqueID();
        subset.material.material_index = mat_i;

        //DiffuseMapの取得
        FbxProperty tex_prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
        if (tex_prop.IsValid()) {
            const FbxDouble3 color = tex_prop.Get<FbxDouble3>();
            subset.material.shader_material.diffuse_color.x = static_cast<float>(color[0]);
            subset.material.shader_material.diffuse_color.y = static_cast<float>(color[1]);
            subset.material.shader_material.diffuse_color.z = static_cast<float>(color[2]);
            subset.material.shader_material.diffuse_color.w = 1.0f;


            const FbxFileTexture* fbx_texture = tex_prop.GetSrcObject<FbxFileTexture>();
            subset.material.diffuse_map_name = fbx_texture ? fbx_texture->GetFileName() : "";
        }

        /*
        tex_prop = material->FindProperty(FbxSurfaceMaterial::sSpecular);
        if (tex_prop.IsValid()) {//DiffuseMapの取得
            const FbxDouble3 color = tex_prop.Get<FbxDouble3>();
            model_texture.shader_material.specular_color.x = static_cast<float>(color[0]);
            model_texture.shader_material.specular_color.y = static_cast<float>(color[1]);
            model_texture.shader_material.specular_color.z = static_cast<float>(color[2]);
            model_texture.shader_material.specular_color.w = 1.0f;

            const FbxFileTexture* fbx_texture = tex_prop.GetSrcObject<FbxFileTexture>();
            model_texture.specular_map_name = fbx_texture ? fbx_texture->GetFileName() : "";
        }
        */
        subset.material.shader_material.diffuse_map_index = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * mat_i;
        subsets.push_back(subset);
    }

    for (auto& subset : subsets)
    {
        //diffuseマップがある場合はdiffuseマップを読み込む,ない場合はダミーのテクスチャを読み込む
        if (!subset.material.diffuse_map_name.empty()) {
            subset.material.diffuse_map = TextureCacheManager::getInstance().getOrCreate(device, subset.material.diffuse_map_name.c_str());
            subset.material.diffuse_map->CreateShaderResourceView(device, descriptor_heap, 1 + subset.material.material_index);
        }
        else {
            subset.material.diffuse_map = TextureCacheManager::getInstance().getOrCreate(device, "asset/models/dummy/dummy.DDS");
            subset.material.diffuse_map->CreateShaderResourceView(device, descriptor_heap, 1 + subset.material.material_index);
        }
    }

    //specularマップがある場合
    /*
    if (!model_texture.specular_map_name.empty()) {
        model_texture.specular_map = TextureCacheManager::getInstance().getOrCreate(device, model_texture.specular_map_name.c_str());
        model_texture.specular_map->CreateShaderResourceView(device, descriptor_heap, 3);
    }
    else {//ダミーテクスチャ
        model_texture.specular_map = TextureCacheManager::getInstance().getOrCreate(device, "asset/models/dummy/dummy.DDS");
        model_texture.specular_map->CreateShaderResourceView(device, descriptor_heap, 3);
    }
    */

    return subsets;
}
