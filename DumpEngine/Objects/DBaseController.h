//
// Created by egors on 02.01.2026.
//

#ifndef DUMPENGINE_DBASECONTROLLER_H
#define DUMPENGINE_DBASECONTROLLER_H
#include "DActorComponent.h"
#include "DSceneComponent.h"


class DBaseController : public DActorComponent{
public:
    DBaseController() = default;

    void Tick(float DeltaTime) override;

    void Start() override;

public:
    void Possess(DSceneComponent* Pawn) {
        if (Pawn == nullptr) return;

        _controlledComponent = Pawn;
    };

private:
    DSceneComponent* _controlledComponent = nullptr;

    float totalYaw = 0.0f;
    float totalPitch = 0.0f;

    float MouseSensitivity = 0.1f;
    bool _isFirstUpdate = true;
};



#endif //DUMPENGINE_DBASECONTROLLER_H
