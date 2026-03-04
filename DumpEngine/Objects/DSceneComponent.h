// Created by egors on 29.11.2025.
//

#ifndef DUMPENGINE_DSCENECOMPONENT_H
#define DUMPENGINE_DSCENECOMPONENT_H

#include "DActorComponent.h"
#include "../Core/Types.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory> // для std::shared_ptr и std::weak_ptr

class DSceneComponent : public DActorComponent {
public:
    DSceneComponent() : DActorComponent() {};

protected:
    void Tick(float DeltaTime) override;

public:
    void Start() override;

private:
    // Локальная трансформация этого компонента относительно его родителя
    DTransform Transform;

    // ИЗМЕНЕНО: Переименовано для ясности. Это родитель, к которому прикреплен компонент.
    DSceneComponent* AttachParent;

public:
    // --- Установка абсолютных ЛОКАЛЬНЫХ значений ---

    // Устанавливает всю локальную трансформацию целиком
    void SetTransform(const DTransform& InTransform) { this->Transform = InTransform; }

    // Устанавливает локальную позицию (смещение от родителя)
    void SetLocation(const DVector& InLocation) { this->Transform.Position = InLocation; }

    // Устанавливает локальное вращение (относительно родителя) через кватернион
    void SetRotation(const DQuaternion& InRotation) { this->Transform.Rotation = InRotation; }

    // Удобный метод для установки локального вращения через углы Эйлера (DRotator)
    void SetRotation(const DRotator& InRotator) { this->Transform.Rotation = DQuaternion(InRotator); }

    // Устанавливает локальный масштаб (относительно родителя)
    void SetScale(const DVector& InScale) { this->Transform.Scale = InScale; }

    void LookAt(const DVector& Target, const DVector& Up = DVector(0.0f, 1.0f, 0.0f));


    // НОВОЕ: --- Методы для добавления смещений к ЛОКАЛЬНЫМ значениям ---

    // Добавляет смещение к локальной позиции
    void AddLocation(const DVector& DeltaLocation) {
        this->Transform.Position += DeltaLocation;
    }

    // Добавляет вращение к локальному вращению
    void AddRotation(const DQuaternion& DeltaRotation) {
        // Вращения комбинируются умножением
        this->Transform.Rotation = DeltaRotation * this->Transform.Rotation;
    }

    // Удобный метод для добавления вращения через углы Эйлера
    void AddRotation(const DRotator& DeltaRotator) {
        AddRotation(DQuaternion(DeltaRotator));
    }




    // --- Получение ЛОКАЛЬНЫХ значений ---

    // Возвращает всю локальную трансформацию
    const DTransform& GetTransform() const { return this->Transform; }

    // Возвращает локальную позицию (смещение от родителя)
    const DVector& GetLocation() const { return this->Transform.Position; }

    // Возвращает локальное вращение в виде кватерниона
    const DQuaternion& GetRotation() const { return this->Transform.Rotation; }

    // Возвращает локальный масштаб
    const DVector& GetScale() const { return this->Transform.Scale; }


    // --- Получение МИРОВЫХ (WORLD) значений ---

    // Рекурсивно вычисляет и возвращает мировую трансформацию
    DTransform GetWorldTransform() const {
        // Пытаемся захватить weak_ptr. Если родитель существует, parent станет валидным shared_ptr.
        if (auto parent = AttachParent) {
            // Рекурсивно получаем мировую трансформацию родителя
            DTransform parentWorldTransform = parent->GetWorldTransform();
            // Комбинируем ее со своей локальной трансформацией
            return parentWorldTransform * this->Transform;
        }
        else {
            // Базовый случай рекурсии: нет родителя, наша мировая трансформация равна локальной
            return this->Transform;
        }
    }

    // Возвращает мировую позицию
    DVector GetWorldLocation() const {
        return GetWorldTransform().Position;
    }

    // Возвращает мировое вращение
    DQuaternion GetWorldRotation() const {
        return GetWorldTransform().Rotation;
    }

    // Возвращает мировой масштаб (обратите внимание, он тоже комбинируется)
    DVector GetWorldScale() const {
        return GetWorldTransform().Scale;
    }


    // --- Управление иерархией ---

    // Возвращает родительский компонент
    DSceneComponent* GetParent() const { return this->AttachParent; }

    // Устанавливает родительский компонент
    void SetParent(DSceneComponent* InParent) { this->AttachParent = InParent; }

    DVector GetForwardVector() const;
    DVector GetRightVector() const;
    DVector GetUpVector() const;
};


#endif //DUMPENGINE_DSCENECOMPONENT_H