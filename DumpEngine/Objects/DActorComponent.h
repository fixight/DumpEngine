//
// Created by egors on 28.11.2025.
//
#pragma once


#ifndef DUMPENGINE_DACTORCOMPONENT_H
#define DUMPENGINE_DACTORCOMPONENT_H
#include "DObject.h"
#include "../Engine/TickManager.h"


class DActorComponent : public DObject{
public:
    DActorComponent() : DObject() {

    }

protected:
    bool CanEverTick = true;

    bool CanTick = true;

public:
    virtual void Start() override;

protected:
    virtual void Tick(float DeltaTime);

private:
    TickableObject TickableObject;
};


#endif //DUMPENGINE_DACTORCOMPONENT_H