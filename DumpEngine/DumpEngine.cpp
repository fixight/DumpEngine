#include "DumpEngine.h" // Ваши заголовочные файлы

// --- Стандартные библиотеки и OpenGL ---
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// --- Заголовочные файлы вашего движка ---
#include <filesystem>
#include <__msvc_filebuf.hpp>

#include "DMemory/DLinearAllocator.h"
#include "Engine/DEngine.h"
#include "Engine/DInputManager.h"
#include "Engine/DRenderSystem.h"
#include "Engine/TickManager.h"
#include "Objects/DBaseController.h"
#include "Objects/DMeshComponent.h"
#include "Objects/DSceneComponent.h"
#include "Systems/ResourceManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const unsigned int SCREEN_WIDTH = 1280;
    const unsigned int SCREEN_HEIGHT = 720;

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dump Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ResourceManager::Get().InitDefaultTextures();

    std::string meshPath = "C:/Users/egors/source/repos/DumpEngine/Cube.fbx";
    auto staticMesh = ResourceManager::Get().LoadStaticMesh(meshPath);


    if (staticMesh && staticMesh->GetMaterialCount() > 0) {

        DMaterial& globalMat = staticMesh->GetMaterial(0);


        globalMat.albedoMap = ResourceManager::Get().LoadTexture("C:/Users/egors/source/repos/DumpEngine/floor.jpg");


    }


    glEnable(GL_DEPTH_TEST);

    DEngine::Get().InitEngine();
    DInputManager::Get().Init(window);
    DRenderSystem::Get().InitRender();


    DVector targetPoint(0.0f, 0.0f, 0.0f);
    DVector worldUp(0.0f, 1.0f, 0.0f);

    DLinearAllocator Allocator = DLinearAllocator(1024 * 1024);




    DMeshComponent* Wall = new DMeshComponent();
    Wall->Start();
    Wall->SetStaticMesh("C:/Users/egors/source/repos/DumpEngine/Cube.fbx");
    Wall->SetLocation(DVector(-72.0f, 18.0f, 21.0f));
    Wall->SetScale(DVector(0.2f, 0.2f, 0.2f));


    DMeshComponent* MeshComponent = new DMeshComponent();
    MeshComponent->Start();
    MeshComponent->SetStaticMesh("C:/Users/egors/source/repos/DumpEngine/model.fbx");
    MeshComponent->SetLocation(targetPoint);
    MeshComponent->SetScale(DVector(1.0f, 1.0f, 1.0f));


    DCamera* Camera = new DCamera();
    Camera->Start();
    Camera->SetLocation(DVector(0.0f, 24.0f, 12.0f));


    DBaseLight* BaseLight = new DBaseLight();
    BaseLight->Start();
    BaseLight->SetLocation(DVector(-8.0f, 10.0f, 8.0f));

    BaseLight->LookAt(targetPoint, worldUp);

    DInputManager::RegisterAxis("VerticalInput", GLFW_KEY_W, GLFW_KEY_S);
    DInputManager::RegisterAxis("HorizontalInput", GLFW_KEY_D, GLFW_KEY_A);
    DBaseController* BaseController = new DBaseController();
    BaseController->Start();
    BaseController->Possess(Camera);


    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
        DInputManager::Get().Update();


        DEngine::Get().GetTickManager()->TickWorld(0.01f);


        DRenderSystem::Get().RenderFrame(Camera, BaseLight);
        MeshComponent->AddRotation(DRotator(0.1f, 0.0f, 0.0f));


        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}