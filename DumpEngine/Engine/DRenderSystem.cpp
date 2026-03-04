//
// Created by egors on 09.12.2025.
//

#include "DRenderSystem.h"

DRenderSystem::DRenderSystem() {
}

void DRenderSystem::AddToRenderSystem(DMeshComponent *mesh) {
    _meshes.push_back(mesh);
    std::cout << "AddMesh" << std::endl;
}

void DRenderSystem::RenderFrame(DCamera* Camera , DBaseLight* light) {
    static std::vector<RenderCommand> frameCommands;
    frameCommands.clear();

    for (DMeshComponent* comp : _meshes) {
        comp->CollectRenderCommands(frameCommands);
    }


    glm::mat4 projection = Camera->GetProjectionMatrix();


    glm::mat4 view = Camera->GetViewMatrix();


    _renderer->BeginScene(projection * view);

    _renderer->Submit(frameCommands , light , Camera);


    _renderer->RenderDebugQuad();




}

void DRenderSystem::InitRender() {
    _renderer = new DRenderer();

    _renderer->Init();

}
