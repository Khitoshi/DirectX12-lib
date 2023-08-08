#include "Scene3dModel.h"
#include "Model.h"

void Scene3dModel::init(ID3D12Device* device)
{
    model = std::make_shared<Model>();
    model->init(device, "asset/models/YBot.fbx");
}

void Scene3dModel::finalize()
{
}

void Scene3dModel::update()
{
}

void Scene3dModel::render(RenderContext* rc)
{
}
