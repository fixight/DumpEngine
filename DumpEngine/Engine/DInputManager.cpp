#include "DInputManager.h"

#include <iostream>
#include <ostream>

std::map<std::string, InputAxis> DInputManager::s_Axes;

bool DInputManager::keys[GLFW_KEY_LAST] = { false };
double DInputManager::mouseX = 0.0;
double DInputManager::mouseY = 0.0;
double DInputManager::lastMouseX = 0.0;
double DInputManager::lastMouseY = 0.0;
double DInputManager::mouseDeltaX = 0.0;
double DInputManager::mouseDeltaY = 0.0;

void DInputManager::Init(GLFWwindow* window) {
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
}

void DInputManager::Update() {

    mouseDeltaX = mouseX - lastMouseX;
    mouseDeltaY = lastMouseY - mouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    for (auto& pair : s_Axes) {
        InputAxis& axis = pair.second;
        axis.CurrentValue = 0.0f;

        for (auto& mapping : axis.KeyMappings) {
            int key = mapping.first;
            float scale = mapping.second;

            if (IsKeyPressed(key)) {
                axis.CurrentValue += scale;
            }
        }
    }
}

bool DInputManager::IsKeyPressed(int key) { return keys[key]; }
double DInputManager::GetMouseX() { return mouseX; }
double DInputManager::GetMouseY() { return mouseY; }
double DInputManager::GetMouseDeltaX() { return mouseDeltaX; }
double DInputManager::GetMouseDeltaY() { return mouseDeltaY; }

void DInputManager::RegisterAxis(std::string name, int positiveKey, int negativeKey) {
    InputAxis newAxis;
    newAxis.Name = name;
    newAxis.KeyMappings[positiveKey] = 1.0f;
    newAxis.KeyMappings[negativeKey] = -1.0f;
    s_Axes[name] = newAxis;

}

float DInputManager::GetAxisValue(const std::string &name) {
    auto it = s_Axes.find(name);
    if (it != s_Axes.end()) {
        return it->second.CurrentValue;
    }

    return 0.0f;
}

void DInputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_RELEASE) {
        keys[key] = true;
    } else {
        keys[key] = false;
    }
}

void DInputManager::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}