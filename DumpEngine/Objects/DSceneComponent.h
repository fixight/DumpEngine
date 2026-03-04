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
    DTransform Transform;

    DSceneComponent* AttachParent;

public:

    void SetTransform(const DTransform& InTransform) { this->Transform = InTransform; }


    void SetLocation(const DVector& InLocation) { this->Transform.Position = InLocation; }


    void SetRotation(const DQuaternion& InRotation) { this->Transform.Rotation = InRotation; }


    void SetRotation(const DRotator& InRotator) { this->Transform.Rotation = DQuaternion(InRotator); }


    void SetScale(const DVector& InScale) { this->Transform.Scale = InScale; }

    void LookAt(const DVector& Target, const DVector& Up = DVector(0.0f, 1.0f, 0.0f));



    void AddLocation(const DVector& DeltaLocation) {
        this->Transform.Position += DeltaLocation;
    }


    void AddRotation(const DQuaternion& DeltaRotation) {

        this->Transform.Rotation = DeltaRotation * this->Transform.Rotation;
    }

    void AddRotation(const DRotator& DeltaRotator) {
        AddRotation(DQuaternion(DeltaRotator));
    }





    const DTransform& GetTransform() const { return this->Transform; }


    const DVector& GetLocation() const { return this->Transform.Position; }

    const DQuaternion& GetRotation() const { return this->Transform.Rotation; }


    const DVector& GetScale() const { return this->Transform.Scale; }



    DTransform GetWorldTransform() const {
        if (auto parent = AttachParent) {
            DTransform parentWorldTransform = parent->GetWorldTransform();
            return parentWorldTransform * this->Transform;
        }
        else {
            return this->Transform;
        }
    }


    DVector GetWorldLocation() const {
        return GetWorldTransform().Position;
    }


    DQuaternion GetWorldRotation() const {
        return GetWorldTransform().Rotation;
    }


    DVector GetWorldScale() const {
        return GetWorldTransform().Scale;
    }



    DSceneComponent* GetParent() const { return this->AttachParent; }


    void SetParent(DSceneComponent* InParent) { this->AttachParent = InParent; }

    DVector GetForwardVector() const;
    DVector GetRightVector() const;
    DVector GetUpVector() const;
};


#endif //DUMPENGINE_DSCENECOMPONENT_H