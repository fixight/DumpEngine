//
// Created by egors on 28.11.2025.
//

#include "DEngine.h"
#include "TickManager.h"

DEngine& DEngine::Get()
{
    static DEngine instance;
    return instance;
}

void DEngine::InitEngine() {
    tickManager = std::make_shared<TickManager>();
    tickManager->Init();

}
