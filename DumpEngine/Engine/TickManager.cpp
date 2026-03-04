//
// Created by egors on 28.11.2025.
//

#include "TickManager.h"

#include <algorithm>

void TickManager::Register(TickableObject *tickable) {
    m_Tickables.push_back(tickable);
}

void TickManager::Unregister(TickableObject *tickable) {
    auto it = std::find(m_Tickables.begin(), m_Tickables.end(), tickable);
    if (it != m_Tickables.end()) {
        std::iter_swap(it, m_Tickables.end() - 1);

        m_Tickables.pop_back();
    }
}

void TickManager::TickWorld(float deltaTime) {
    for (size_t i = 0; i < m_Tickables.size(); ++i) {
        if (m_Tickables[i] && m_Tickables[i]->TickFunction) {
            m_Tickables[i]->TickFunction(deltaTime);
        }
    }
}

void TickManager::Init() {
    std::cout << "TickManager::Init()" << std::endl;
}
