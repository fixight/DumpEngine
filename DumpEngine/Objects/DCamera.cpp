//
// Created by egors on 02.01.2026.
//

#include "DCamera.h"

void DCamera::Start() {
    DSceneComponent::Start();
}

void DCamera::Tick(float deltaTime) {
    DSceneComponent::Tick(deltaTime);
}


glm::mat4 DCamera::GetViewMatrix() const {
    DVector pos = GetWorldLocation();
    DVector forward = GetForwardVector();


    if (forward.Length() < 0.0001f) {
        forward = DVector(0.0f, 0.0f, -1.0f);
    }

    return glm::lookAt(
        pos.ToGLM(),
        (pos + forward).ToGLM(),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}
glm::mat4 DCamera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(FOV), AspectRatio, NearPlane, FarPlane);
}
