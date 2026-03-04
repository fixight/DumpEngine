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
    // Вращаем вектор (1, 0, 0) мировым кватернионом
    return GetWorldRotation().RotateVector(DVector(0.0f, 0.0f, -1.0f));
}

DVector DSceneComponent::GetRightVector() const {
    // Вращаем (0,1,0) — это ваш Right по Types.h
    return GetWorldRotation().RotateVector(DVector(1.0f, 0.0f, 0.0f));
}

DVector DSceneComponent::GetUpVector() const {
    return GetWorldRotation().RotateVector(DVector(0.0f, 1.0f, 0.0f));
}

// Внутри DSceneComponent.cpp
// Убедитесь, что вы подключили нужные заголовдки GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// ... другие ваши инклуды ...

void DSceneComponent::LookAt(const DVector& Target, const DVector& Up) {
    // 1. Получаем нашу текущую мировую позицию.
    // Это точка, ИЗ которой мы будем смотреть.
    DVector currentPosition = this->GetWorldLocation();

    // 2. Используем магию GLM, чтобы построить матрицу вида "look-at".
    // Эта матрица описывает трансформацию, которая "смотрит" из currentPosition на Target.
    // ВАЖНО: glm::lookAt создает матрицу ВИДА (View Matrix), которая является ОБРАТНОЙ
    // к матрице трансформации объекта. Поэтому нам нужно ее инвертировать,
    // чтобы получить правильную ориентацию объекта в мире.
    glm::mat4 lookAtMatrix = glm::lookAt(
        glm::vec3(currentPosition.x, currentPosition.y, currentPosition.z),
        glm::vec3(Target.x, Target.y, Target.z),
        glm::vec3(Up.x, Up.y, Up.z)
    );

    // 3. Инвертируем матрицу вида, чтобы получить матрицу мировой трансформации.
    glm::mat4 worldTransformMatrix = glm::inverse(lookAtMatrix);

    // 4. Извлекаем из этой матрицы кватернион вращения.
    // GLM делает всю сложную работу за нас.
    DQuaternion newRotation = glm::quat_cast(worldTransformMatrix);

    // 5. Устанавливаем это новое вращение как наше ЛОКАЛЬНОЕ вращение.
    // ПРИМЕЧАНИЕ: Это работает корректно, если у компонента нет родителя
    // или если родитель не повернут. Для сложных иерархий может
    // потребоваться преобразовать мировое вращение в локальное.
    // Но для камеры и света, которые обычно находятся на верхнем уровне, это идеально.
    this->SetRotation(newRotation);

    std::cout <<this->GetRotation().x << this->GetRotation().y << this->GetRotation().z << this->GetRotation().w << std::endl;
}
