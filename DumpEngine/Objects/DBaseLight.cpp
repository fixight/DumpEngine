//
// Created by egors on 03.01.2026.
//

#include "DBaseLight.h"

void DBaseLight::Start() {
    DSceneComponent::Start();
}

void DBaseLight::Tick(float DeltaTime) {
    DSceneComponent::Tick(DeltaTime);
}

glm::mat4 DBaseLight::GetProjectionMatrix() const {
    // Увеличили область с 20 до 40.
    // Если черная граница осталась, ставьте 60, 80 и т.д.
    float size = 800.0f;
    return glm::ortho(-size, size, -size, size, 1.0f, 350.0f);
}

glm::mat4 DBaseLight::GetViewMatrix() const {
    // В шейдере свет бьет в направлении (1.0, 0.5, 0.5).
    // Значит, саму камеру нужно поставить "позади" и направить в (0,0,0).

    // Берем позицию где-то далеко в противоположной стороне
    glm::vec3 lightPos = glm::vec3(20.0f, 10.0f, 10.0f);

    // Смотрим в центр сцены (0,0,0), верх - Y (0,1,0)
    return glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
