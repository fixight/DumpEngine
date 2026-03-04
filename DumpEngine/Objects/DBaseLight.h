//
// Created by egors on 03.01.2026.
//

#ifndef DUMPENGINE_DBASELIGHT_H
#define DUMPENGINE_DBASELIGHT_H
#include "DSceneComponent.h"


class DBaseLight : public DSceneComponent{
public:
    DBaseLight() = default;

public:
    void Start() override;

    void Tick(float DeltaTime) override;


    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

private:
    const float near = 1.0f;
    const float far = 100.0f;
    const float left = 100.0f;
    const float right = 100.0f;
    const float bottom = 1000.0f;
    float top = 100.0f;


};


#endif //DUMPENGINE_DBASELIGHT_H