//
// Created by egors on 02.01.2026.
//

#include "DBaseController.h"

#include "../Engine/DInputManager.h"

#include "DBaseController.h"
#include "../Engine/DInputManager.h"

// Подключаем GLM для создания кватернионов из оси и угла
#include <glm/gtc/quaternion.hpp>

void DBaseController::Start() {
    DActorComponent::Start();
    totalYaw = 0.0f;
    totalPitch = 0.0f;
    if (_controlledComponent) {
        _controlledComponent->SetRotation(DQuaternion()); // Сброс в Identity (0,0,0)
    }
}

void DBaseController::Tick(float DeltaTime) {
    DActorComponent::Tick(DeltaTime);
    if (!_controlledComponent) return;

    // 1. Получаем дельты мыши
    float mX = (float)DInputManager::GetMouseDeltaX();
    float mY = -(float)DInputManager::GetMouseDeltaY();

    // 2. Правильно распределяем:
    // mX вращает камеру вокруг ВЕРТИКАЛЬНОЙ оси (Yaw)
    // mY вращает камеру вокруг ГОРИЗОНТАЛЬНОЙ оси (Pitch)
    totalYaw   -= mX * MouseSensitivity; // Поворот влево-вправо
    totalPitch -= mY * MouseSensitivity; // Поворот вверх-вниз

    // 3. Ограничиваем только Pitch (чтобы не делать сальто)
    if (totalPitch > 89.0f) totalPitch = 89.0f;
    if (totalPitch < -89.0f) totalPitch = -89.0f;

    // 4. Создаем кватернионы
    // ВАЖНО: Если Up — это Y, то Yaw делаем вокруг (0, 1, 0)
    // Pitch делаем вокруг локальной оси X (1, 0, 0)
    glm::quat qYaw   = glm::angleAxis(glm::radians(totalYaw),   glm::vec3(0, 1, 0));
    glm::quat qPitch = glm::angleAxis(glm::radians(totalPitch), glm::vec3(1, 0, 0));

    // 5. Комбинируем: Yaw (глобальный) * Pitch (локальный)
    // Это исключает "крен" (roll)
    _controlledComponent->SetRotation(DQuaternion(qYaw * qPitch));

    // 6. Движение (используем векторы компонента)
    DVector f = _controlledComponent->GetForwardVector();
    DVector r = _controlledComponent->GetRightVector();
    float v = DInputManager::GetAxisValue("VerticalInput");
    float h = DInputManager::GetAxisValue("HorizontalInput");

    DVector move = (f * v) + (r * h);
    if (move.LengthSquared() > 0.0f) {
        _controlledComponent->AddLocation(move.Normalized() * 80.f * DeltaTime);
    }
}


