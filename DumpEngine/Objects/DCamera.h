//
// Created by egors on 02.01.2026.
//

#ifndef DUMPENGINE_DCAMERA_H
#define DUMPENGINE_DCAMERA_H
#include "DSceneComponent.h"


class DCamera : public DSceneComponent {
public:
    DCamera() = default;

public:
    void Start() override;

    void Tick(float deltaTime) override;

public:

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;


private:
    float FOV = 90.f;
    float AspectRatio = 1.7f;
    float NearPlane = 0.1f;
    float FarPlane = 1000.f;

};



#endif //DUMPENGINE_DCAMERA_H
