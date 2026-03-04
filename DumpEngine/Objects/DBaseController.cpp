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
        _controlledComponent->SetRotation(DQuaternion());
    }
}

void DBaseController::Tick(float DeltaTime) {
    DActorComponent::Tick(DeltaTime);
    if (!_controlledComponent) return;

    float mX = (float)DInputManager::GetMouseDeltaX();
    float mY = -(float)DInputManager::GetMouseDeltaY();


    totalYaw   -= mX * MouseSensitivity;
    totalPitch -= mY * MouseSensitivity;


    if (totalPitch > 89.0f) totalPitch = 89.0f;
    if (totalPitch < -89.0f) totalPitch = -89.0f;


    glm::quat qYaw   = glm::angleAxis(glm::radians(totalYaw),   glm::vec3(0, 1, 0));
    glm::quat qPitch = glm::angleAxis(glm::radians(totalPitch), glm::vec3(1, 0, 0));


    _controlledComponent->SetRotation(DQuaternion(qYaw * qPitch));


    DVector f = _controlledComponent->GetForwardVector();
    DVector r = _controlledComponent->GetRightVector();
    float v = DInputManager::GetAxisValue("VerticalInput");
    float h = DInputManager::GetAxisValue("HorizontalInput");

    DVector move = (f * v) + (r * h);
    if (move.LengthSquared() > 0.0f) {
        _controlledComponent->AddLocation(move.Normalized() * 80.f * DeltaTime);
    }
}


