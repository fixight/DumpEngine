//
// Created by egors on 29.11.2025.
//

#include "DMeshComponent.h"

#include "../Core/DBaseMesh.h"
#include "../Engine/DRenderSystem.h"

void DMeshComponent::SetStaticMesh(const std::string &path) {
    std::vector<Mesh> outMeshes;
\
    _staticMesh = ResourceManager::Get().LoadStaticMesh(path).get();
    DRenderSystem::Get().AddToRenderSystem(this);
}

void DMeshComponent::SetStaticMesh(DStaticMesh *mesh) {
    if (!_staticMesh) return;
    _staticMesh = mesh;

}

void DMeshComponent::Tick(float DeltaTime) {
    DSceneComponent::Tick(DeltaTime);


}

void DMeshComponent::Start() {
    DSceneComponent::Start();

}

void DMeshComponent::CollectRenderCommands(std::vector<RenderCommand>& outArray) const {
    if (!_staticMesh) return;

    glm::mat4 worldMat = GetWorldTransform().ToMatrix();
    const auto& subMeshes = _staticMesh->GetAllMeshes();

    for (const auto& mesh_ptr : subMeshes) {
        RenderCommand cmd;

        cmd.mesh = mesh_ptr.get();


        cmd.material = &_staticMesh->GetMaterial(mesh_ptr->materialIndex);

        cmd.modelMatrix = worldMat;

        outArray.push_back(cmd);
    }
}


