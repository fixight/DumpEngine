//
// Created by egors on 18.01.2026.
//

#include "ResourceManager.h"


#include <glad/glad.h>

#include "../Core/DStaticMesh.h"

ResourceManager& ResourceManager::Get() {
    static ResourceManager instance; // Создается один раз при первом вызове
    return instance;
}

std::shared_ptr<DStaticMesh> ResourceManager::LoadStaticMesh(const std::string& path) {
    // Если меш уже есть в кэше, возвращаем его
    if (meshCache.count(path)) {
        return meshCache[path];
    }


    auto mesh = std::make_shared<DStaticMesh>(path);
    meshCache[path] = mesh;
    return mesh;
}

#include "ResourceManager.h"
#include <iostream>
#include <algorithm> // Для std::replace
#include <glad/glad.h>
#include "stb_image.h" // Убедитесь, что STB_IMAGE_IMPLEMENTATION определен где-то в проекте (обычно в main.cpp или ModelLoader.cpp)

// ... (Ваш код Singleton и InitDefaultTextures) ...

unsigned int ResourceManager::LoadTexture(const std::string& path, const std::string& typeName) {
    // 1. Нормализация пути
    // Заменяем обратные слэши на прямые, чтобы "Assets\Tex.png" и "Assets/Tex.png" считались одним и тем же файлом
    std::string filename = path;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    // 2. Проверка кэша
    // Если текстура с таким путем уже загружена, просто возвращаем её ID
    if (textureCache.count(filename)) {
        return textureCache[filename];
    }

    // 3. Загрузка данных через stb_image
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    unsigned int textureID;

    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else {
            // Если формат странный, фоллбэк на RGB
            format = GL_RGB;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Загружаем данные в видеопамять
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Настройка параметров (повторение и фильтрация)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Освобождаем память на CPU, так как данные уже в GPU
        stbi_image_free(data);

        // 4. Сохраняем в кэш
        textureCache[filename] = textureID;

        std::cout << "Texture loaded: " << filename << std::endl;
    }
    else {
        // ОШИБКА ЗАГРУЗКИ
        std::cout << "Texture failed to load at path: " << filename << ". Using default for type: " << typeName << std::endl;

        // stbi_image_free безопасен даже с null, но хорошая практика
        stbi_image_free(data);

        // 5. Возвращаем дефолтную текстуру ("розовую заглушку" или другую, в зависимости от типа)
        // В кэш "сломанный" путь НЕ сохраняем, чтобы при следующей попытке (если файл починят) он загрузился.
        return GetDefaultTexture(typeName);
    }

    return textureID;
}


// Вспомогательная функция, которая создает 1x1 текстуру заданного цвета
unsigned int CreateDefaultTexture(unsigned char r, unsigned char g, unsigned char b) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    unsigned char data[] = { r, g, b };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void ResourceManager::InitDefaultTextures() {
    // Создаем дефолтные 1x1 текстуры
    defaultAlbedo    = CreateDefaultTexture(255, 20, 147); // Ярко-розовый
    defaultNormal    = CreateDefaultTexture(128, 128, 255); // "Плоская" нормаль
    defaultMetallic  = CreateDefaultTexture(0, 0, 0);       // Черный (не металл)
    defaultRoughness = CreateDefaultTexture(128, 128, 128); // Серый (средняя шероховатость)
    defaultAO        = CreateDefaultTexture(255, 255, 255); // Белый (нет AO)
}

unsigned int ResourceManager::GetDefaultTexture(const std::string& typeName) {
    if (typeName == "texture_diffuse") return defaultAlbedo;
    if (typeName == "texture_normal") return defaultNormal;
    if (typeName == "texture_metallic") return defaultMetallic;
    if (typeName == "texture_roughness") return defaultRoughness;
    if (typeName == "texture_ao") return defaultAO;

    // По умолчанию возвращаем альбедо, если тип неизвестен
    return defaultAlbedo;
}

// Не забудьте вызвать ResourceManager::Get().InitDefaultTextures()
// где-то в самом начале вашей программы, после инициализации OpenGL!