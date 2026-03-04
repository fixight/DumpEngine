//
// Created by egors on 29.11.2025.
//

#ifndef DUMPENGINE_DMESHCOMPONENT_H
#define DUMPENGINE_DMESHCOMPONENT_H
#include "DSceneComponent.h"
#include "../Core/DStaticMesh.h"


class DMeshComponent : public DSceneComponent{

public:
    DMeshComponent() = default;

private:
    DStaticMesh* _staticMesh = nullptr;

public:
    void SetStaticMesh(const std::string& path);

    void SetStaticMesh(DStaticMesh* mesh);

    void Tick(float DeltaTime) override;

    void Start() override;

    void CollectRenderCommands(std::vector<RenderCommand>& outArray) const;

private:


};


#endif //DUMPENGINE_DMESHCOMPONENT_H