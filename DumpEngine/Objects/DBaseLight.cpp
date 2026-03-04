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

    float size = 800.0f;
    return glm::ortho(-size, size, -size, size, 1.0f, 350.0f);
}

glm::mat4 DBaseLight::GetViewMatrix() const {

    glm::vec3 lightPos = glm::vec3(20.0f, 10.0f, 10.0f);

    return glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
