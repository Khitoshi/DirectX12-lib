#include "ModelLoader.h"
#include "ModelData.h"
#include "DescriptorHeap.h"
#include <vector>
#include <fbxsdk.h>


//TODO: ���쐬
std::shared_ptr<ModelData> ModelLoader::load(const char* model_file_path)
{
    // FBX SDK��������
    auto manager = FbxManager::Create();
    auto importer = FbxImporter::Create(manager, "");
    if (!importer->Initialize(model_file_path, -1, manager->GetIOSettings())) {
        throw std::exception("Failed to load FBX file");
    }

    // �V�[�����쐬
    auto scene = FbxScene::Create(manager, "");
    if (!importer->Import(scene)) {
        throw std::exception("Failed to import scene");
    }
    importer->Destroy();

    // �O�p�|���S���ւ̃R���o�[�g
    FbxGeometryConverter geometryConverter(manager);
    if (!geometryConverter.Triangulate(scene, true)) {
        throw std::exception("Failed to triangulate scene");
    }

    std::shared_ptr<ModelData> model_data = std::make_shared<ModelData>();

    // ���[�g�m�[�h���擾
    FbxNode* rootNode = scene->GetRootNode();
    const UINT child_count = rootNode->GetChildCount();
    for (UINT i = 0; i < child_count; i++)
    {
        FbxNode* childNode = rootNode->GetChild(i);
        FbxMesh* mesh = childNode->GetMesh();

        if (!mesh) continue;

        //�@�����擾
        FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
        //UV���擾
        FbxGeometryElementUV* uvElement = mesh->GetElementUV();

        // �C���f�b�N�X���擾
        for (UINT polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); ++polygonIndex) {
            UINT polygonSize = mesh->GetPolygonSize(polygonIndex); // �|���S���̒��_���i�ʏ��3�܂���4�j

            // ���̃|���S�����g�p���钸�_�����擾
            for (UINT vertexIndex = 0; vertexIndex < polygonSize; ++vertexIndex) {
                ModelData::USHORT controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
                FbxVector4 vertex = mesh->GetControlPointAt(controlPointIndex);
                ModelData::Vertex vertices = {};

                //���_���擾
                vertices.position = DirectX::XMFLOAT3(DirectX::XMFLOAT3(vertex[0], vertex[1], vertex[2]));
                //�@�����擾
                if (normalElement) {
                    FbxVector4 normal;
                    mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal);
                    vertices.normal = DirectX::XMFLOAT3(DirectX::XMFLOAT3(normal[0], normal[1], normal[2]));
                }
                //UV���擾
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
                // ������texFileName���g�p���ăe�N�X�`�������[�h���܂�
            }
        }
        */

    }



    return model_data;
}
