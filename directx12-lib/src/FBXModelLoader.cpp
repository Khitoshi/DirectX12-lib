#include "FBXModelLoader.h"
#include "FBXModelData.h"
#include "DescriptorHeap.h"
#include <vector>
#include <unordered_map>
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include "TextureCacheManager.h"


//  �@�������
DirectX::XMFLOAT3 parseNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index);
//  uv���W�����
DirectX::XMFLOAT2 parseUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uvElement);
//  �}�e���A�������
std::shared_ptr<Texture> parseMaterial(ID3D12Device* device, const fbxsdk::FbxNode* node);

/// <summary>
/// ���f���̃��[�h
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
/// <param name="descriptor_heap">�f�B�X�N���v�^�q�[�v</param>
/// <param name="model_file_path">���f���̃t�@�C���p�X</param>
/// <returns>
/// ���f���f�[�^�̃C���X�^���X
/// </returns>
std::shared_ptr<FBXModelData> FBXModelLoader::load(ID3D12Device* device, DescriptorHeap* descriptor_heap, const char* model_file_path)
{
    // FBX SDK��������
    auto manager = FbxManager::Create();
    auto importer = FbxImporter::Create(manager, "");
    if (!importer->Initialize(model_file_path, -1, manager->GetIOSettings())) {
        FbxString error = importer->GetStatus().GetErrorString();
        auto err_message = "Failed to load FBX file" + std::string(error.Buffer());
        throw std::exception(err_message.c_str());
    }

    // �V�[�����쐬
    auto scene = FbxScene::Create(manager, "");
    if (!importer->Import(scene)) {
        FbxString error = importer->GetStatus().GetErrorString();
        auto err_message = "Failed to import scene" + std::string(error.Buffer());
        throw std::exception(err_message.c_str());
    }
    importer->Destroy();

    // �O�p�|���S���ւ̃R���o�[�g
    FbxGeometryConverter geometry_converter(manager);
    if (!geometry_converter.Triangulate(scene, true)) {
        FbxString error = importer->GetStatus().GetErrorString();
        auto err_message = "Failed to triangulate scene" + std::string(error.Buffer());
        throw std::exception(err_message.c_str());
    }

    // �ŏI�I�Ɏg�p���郂�f���f�[�^�̃C���X�^���X�𐶐�
    std::shared_ptr<FBXModelData> model_data = std::make_shared<FBXModelData>();

    //�C���f�b�N�X�̌v�Z�����邽�߂̃}�b�v
    std::unordered_map<FBXModelData::Vertex, FBXModelData::USHORT, FBXModelData::ModelDataKeyHasher, std::equal_to<FBXModelData::Vertex>> index_map;

    // ���[�g�m�[�h���擾
    FbxNode* root_node = scene->GetRootNode();
    const UINT child_count = root_node->GetChildCount();
    for (UINT i = 0; i < child_count; i++)
    {
        FbxNode* child_node = root_node->GetChild(i);
        FbxMesh* mesh = child_node->GetMesh();
        if (!mesh) continue;

        //�@�����擾
        FbxGeometryElementNormal* normal_element = mesh->GetElementNormal();
        //UV���擾
        FbxGeometryElementUV* uv_element = mesh->GetElementUV();

        for (UINT poly_i = 0; poly_i < mesh->GetPolygonCount(); ++poly_i) {
            // �|���S���̒��_���i�ʏ��3�܂���4�j
            UINT polygon_size = mesh->GetPolygonSize(poly_i);
            if (polygon_size != 3) throw std::exception("Polygon size is not 3");

            // ���̃|���S�����g�p���钸�_�����擾
            for (UINT ver_i = 0; ver_i < polygon_size; ++ver_i) {
                FBXModelData::USHORT control_point_index = mesh->GetPolygonVertex(poly_i, ver_i);
                FbxVector4 vertex = mesh->GetControlPointAt(control_point_index);
                FBXModelData::Vertex vertices = {};

                //���_���擾
                vertices.position = DirectX::XMFLOAT3(DirectX::XMFLOAT3(vertex[0], vertex[1], vertex[2]));

                //�@�����擾
                if (normal_element) vertices.normal = parseNormal(*mesh, poly_i, ver_i);

                //UV���擾
                if (uv_element) vertices.uv = parseUV(*mesh, poly_i, ver_i, *uv_element);

                //���_�̃C���f�b�N�X���擾
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
        //�}�e���A���̎擾
        model_data->setDiffuseTexture(parseMaterial(device, child_node));
        model_data->getDiffuseTexture()->CreateShaderResourceView(device, descriptor_heap, 1);
    }

    // FBX SDK��j��
    scene->Destroy();
    manager->Destroy();

    return model_data;
}

/// <summary>
/// �@�����̉��
/// </summary>
/// <param name="mesh">���b�V��</param>
/// <param name="polygon_index">�|���S���ԍ�</param>
/// <param name="vertex_index">���_�ԍ�</param>
/// <returns>
/// ��͂����@�����
/// </returns>
DirectX::XMFLOAT3 parseNormal(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index)
{
    FbxVector4 normal;
    mesh.GetPolygonVertexNormal(polygon_index, vertex_index, normal);
    return DirectX::XMFLOAT3(DirectX::XMFLOAT3(normal[0], normal[1], normal[2]));
}

/// <summary>
/// UV���̉��
/// </summary>
/// <param name="mesh">���b�V��</param>
/// <param name="polygon_index">�|���S���ԍ�</param>
/// <param name="vertex_index">���_�ԍ�</param>
/// <param name="uvElement">uv���</param>
/// <returns>
/// ��͂���UV���
/// </returns>
DirectX::XMFLOAT2 parseUV(const FbxMesh& mesh, const UINT& polygon_index, const UINT& vertex_index, const FbxGeometryElementUV& uv_element)
{
    FbxVector2 uv;
    bool unmapped;
    mesh.GetPolygonVertexUV(polygon_index, vertex_index, uv_element.GetName(), uv, unmapped);
    return DirectX::XMFLOAT2(uv[0], 1.0f - uv[1]);
}

/// <summary>
/// �e�N�X�`���̉��
/// </summary>
/// <param name="device">GPU�f�o�C�X</param>
/// <param name="descriptor_heap">�e�N�X�`��������ꂽ���f�B�X�N���v�^�q�[�v</param>
/// <param name="node">fbx��node</param>
/// <returns>
/// �e�N�X�`���̃C���X�^���X
/// </returns>
std::shared_ptr<Texture> parseMaterial(ID3D12Device* device, const fbxsdk::FbxNode* node)
{
    std::shared_ptr<Texture> diffuse_texture = nullptr;
    int material_count = node->GetMaterialCount();
    for (int mat_i = 0; mat_i < material_count; mat_i++) {
        //�}�e���A���̎擾
        FbxSurfaceMaterial* material = node->GetMaterial(mat_i);
        if (!material) continue;

        //�e�N�X�`���̐����m�F 0�ȉ��Ȃ玟�̃}�e���A����
        FbxProperty tex_prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
        int texture_count = tex_prop.GetSrcObjectCount<FbxFileTexture>();
        if (texture_count <= 0)continue;

        //�e�N�X�`���̎擾
        FbxFileTexture* tex_file = tex_prop.GetSrcObject<FbxFileTexture>();
        if (!tex_file) continue;

        //�e�N�X�`���̃��[�h
        const char* texture_file_path = tex_file->GetFileName();
        diffuse_texture = TextureCacheManager::getInstance().getOrCreate(device, texture_file_path);
    }

    //�f�B�t���[�Y�}�b�v���Ȃ��ꍇ�̓_�~�[�̃e�N�X�`�����쐬
    if (!diffuse_texture) {
        diffuse_texture = TextureCacheManager::getInstance().getOrCreate(device, "asset/models/dummy/dummy.DDS");
    }

    return diffuse_texture;
}
