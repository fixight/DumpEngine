//
// Created by egors on 28.11.2025.
//

#include "DActorComponent.h"

#include "../Engine/DEngine.h"
#include "../Engine/TickManager.h"

//не имеет физического представления в мире

void DActorComponent::Start() {
    DObject::Start();

    if (CanEverTick) {;
        TickableObject.TickFunction = [&](float deltaTime) {Tick(deltaTime);};
        DEngine::Get().GetTickManager()->Register(&TickableObject);
        std::cout<<"Registered"<<std::endl;
    }
}

void DActorComponent::Tick(float DeltaTime) {
    if (!CanTick) return;

    //std::cout<<"Ticking"<<std::endl;
}
