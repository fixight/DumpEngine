#pragma once
#include <map>
#include <string>
#include <GLFW/glfw3.h>

struct InputAxis {
    std::string Name;
    std::map<int, float> KeyMappings;
    float CurrentValue = 0.0f;
};

class DInputManager {
public:
    static DInputManager& Get() {
        static DInputManager instance;
        return instance;
    }
    DInputManager(const DInputManager&) = delete;
    DInputManager& operator=(const DInputManager&) = delete;

private:
    DInputManager() = default;

    static bool keys[GLFW_KEY_LAST];
    static double mouseX, mouseY;
    static double lastMouseX, lastMouseY;
    static double mouseDeltaX, mouseDeltaY;

    static std::map<std::string, InputAxis> s_Axes;

public:
    static void Init(GLFWwindow* window);
    static void Update();

    static bool IsKeyPressed(int key);
    static double GetMouseX();
    static double GetMouseY();
    static double GetMouseDeltaX();
    static double GetMouseDeltaY();

    static void RegisterAxis(std::string name, int positiveKey, int negativeKey);

    static float GetAxisValue(const std::string& name);

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};