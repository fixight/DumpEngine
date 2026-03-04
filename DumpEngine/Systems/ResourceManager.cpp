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



unsigned int ResourceManager::LoadTexture(const std::string& path, const std::string& typeName) {

    std::string filename = path;
    std::replace(filename.begin(), filename.end(), '\\', '/');


    if (textureCache.count(filename)) {
        return textureCache[filename];
    }

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

            format = GL_RGB;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);


        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        textureCache[filename] = textureID;

        std::cout << "Texture loaded: " << filename << std::endl;
    }
    else {

        std::cout << "Texture failed to load at path: " << filename << ". Using default for type: " << typeName << std::endl;

        stbi_image_free(data);

        return GetDefaultTexture(typeName);
    }

    return textureID;
}


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
    defaultAlbedo    = CreateDefaultTexture(255, 20, 147);
    defaultNormal    = CreateDefaultTexture(128, 128, 255);
    defaultMetallic  = CreateDefaultTexture(0, 0, 0);
    defaultRoughness = CreateDefaultTexture(128, 128, 128);
    defaultAO        = CreateDefaultTexture(255, 255, 255);
}

unsigned int ResourceManager::GetDefaultTexture(const std::string& typeName) {
    if (typeName == "texture_diffuse") return defaultAlbedo;
    if (typeName == "texture_normal") return defaultNormal;
    if (typeName == "texture_metallic") return defaultMetallic;
    if (typeName == "texture_roughness") return defaultRoughness;
    if (typeName == "texture_ao") return defaultAO;


    return defaultAlbedo;
}

