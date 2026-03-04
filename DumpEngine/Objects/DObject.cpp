//
// Created by egors on 28.11.2025.
//

#include "DObject.h"

std::atomic<unsigned int> DObject::s_NextID = 0;

void DObject::Start() {
    ID = s_NextID++;
}
