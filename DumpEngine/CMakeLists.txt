# 1. Находим библиотеки, установленные через vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# 2. Создаем исполняемый файл (ваш exe)
# Убедитесь, что здесь перечислены ваши .cpp и .h файлы
add_executable (DumpEngine 
    "DumpEngine.cpp" 
    "DumpEngine.h"
        Objects/DObject.cpp
        Objects/DObject.h
        Objects/DActorComponent.cpp
        Objects/DActorComponent.h
        Engine/DEngine.cpp
        Engine/DEngine.h
        Engine/TickManager.cpp
        Engine/TickManager.h
        Systems/ModelLoader.cpp
        Systems/ModelLoader.h
        Core/Types.h
        Objects/DSceneComponent.cpp
        Objects/DSceneComponent.h
        Objects/DMeshComponent.cpp
        Objects/DMeshComponent.h
        Core/DBaseMesh.cpp
        Core/DBaseMesh.h
        Core/DStaticMesh.cpp
        Core/DStaticMesh.h
        Engine/DRenderer.cpp
        Engine/DRenderer.h
        Engine/DRenderSystem.cpp
        Engine/DRenderSystem.h
        Engine/DInputManager.cpp
        Engine/DInputManager.h
        Objects/DBaseController.cpp
        Objects/DBaseController.h
        Objects/DCamera.cpp
        Objects/DCamera.h
        Objects/DBaseLight.cpp
        Objects/DBaseLight.h
        Systems/ResourceManager.cpp
        Systems/ResourceManager.h
        DMemory/DLinearAllocator.cpp
        DMemory/DLinearAllocator.h
        DMemory/DStackAllocator.cpp
        DMemory/DStackAllocator.h
)
target_compile_options(DumpEngine PRIVATE /utf-8)

# 3. Линкуем библиотеки к проекту
target_link_libraries(DumpEngine PRIVATE 
    glfw       # Окно и ввод
    glm::glm   # Математика
    OpenGL::GL
    glad::glad#
    assimp::assimp
)

# 4. Копируем DLL файлы (если нужно) в папку с exe при сборке
# Для статической линковки (стандарт vcpkg) это часто не нужно, но полезно знать.