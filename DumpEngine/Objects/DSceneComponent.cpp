//
// Created by egors on 29.11.2025.
//
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "DSceneComponent.h"

void DSceneComponent::Tick(float DeltaTime) {
    DActorComponent::Tick(DeltaTime);


}

void DSceneComponent::Start() {
    DActorComponent::Start();
}

DVector DSceneComponent::GetForwardVector() const {
    return GetWorldRotation().RotateVector(DVector(0.0f, 0.0f, -1.0f));
}

DVector DSceneComponent::GetRightVector() const {
    return GetWorldRotation().RotateVector(DVector(1.0f, 0.0f, 0.0f));
}

DVector DSceneComponent::GetUpVector() const {
    return GetWorldRotation().RotateVector(DVector(0.0f, 1.0f, 0.0f));
}



void DSceneComponent::LookAt(const DVector& Target, const DVector& Up) {

    DVector currentPosition = this->GetWorldLocation();


    glm::mat4 lookAtMatrix = glm::lookAt(
        glm::vec3(currentPosition.x, currentPosition.y, currentPosition.z),
        glm::vec3(Target.x, Target.y, Target.z),
        glm::vec3(Up.x, Up.y, Up.z)
    );

    glm::mat4 worldTransformMatrix = glm::inverse(lookAtMatrix);

    DQuaternion newRotation = glm::quat_cast(worldTransformMatrix);


    this->SetRotation(newRotation);

    std::cout <<this->GetRotation().x << this->GetRotation().y << this->GetRotation().z << this->GetRotation().w << std::endl;
}
