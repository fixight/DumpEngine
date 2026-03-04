#ifndef DUMPENGINE_DRENDERSYSTEM_H
#define DUMPENGINE_DRENDERSYSTEM_H

#include <vector>

#include "DRenderer.h"
#include "../Objects/DCamera.h"
#include "../Objects/DMeshComponent.h"

class DRenderSystem {
public:

    static DRenderSystem& Get() {
        static DRenderSystem instance;
        return instance;
    }

    DRenderSystem(const DRenderSystem&) = delete;
    DRenderSystem& operator=(const DRenderSystem&) = delete;

private:

    DRenderSystem();


    std::vector<DMeshComponent*> _meshes;

    DRenderer* _renderer;



public:
    void AddToRenderSystem(DMeshComponent* mesh);
    void RenderFrame(DCamera* Camera , DBaseLight* light);

    void InitRender();
};

#endif //DUMPENGINE_DRENDERSYSTEM_H