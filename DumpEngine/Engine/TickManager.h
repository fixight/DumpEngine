// TickableObject.h (Вместо интерфейса - структура для регистрации)
#pragma once
#include <functional>
#include <iostream>

struct TickableObject {
    std::function<void(float)> TickFunction;

};


class TickManager {
public:
    void Register(TickableObject* tickable);
    void Unregister(TickableObject* tickable);
    void TickWorld(float deltaTime);
    void Init();

private:
    std::vector<TickableObject*> m_Tickables;
};