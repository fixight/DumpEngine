#include "ModelLoader.h"
#include "ResourceManager.h" // <-- ВАЖНО: подключаем наш менеджер!
#include <iostream>
#include <algorithm> // для std::replace

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>

// Вспомогательная функция, вынесена в этот файл, так как используется только здесь
unsigned int TextureFromFile(const char* path, const std::string& directory, const std::string& typeName);


std::unique_ptr<Model> ModelLoader::LoadModel(const std::string& path) {
    auto model = std::make_unique<Model>();
    textures_loaded.clear();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    directory = path.substr(0, path.find_last_of('/'));

    // Заранее обрабатываем все материалы в сцене
    model->materials.resize(scene->mNumMaterials);
    for(unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* material = scene->mMaterials[i];
        // Теперь передаем typeName в loadMaterialTexture
        model->materials[i].albedoMap    = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
        model->materials[i].normalMap    = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal"); // Assimp часто грузит карты нормалей как HEIGHT
        model->materials[i].metallicMap  = loadMaterialTexture(material, aiTextureType_METALNESS, "texture_metallic");
        model->materials[i].roughnessMap = loadMaterialTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
        model->materials[i].aoMap        = loadMaterialTexture(material, aiTextureType_AMBIENT_OCCLUSION, "texture_ao");
    }

    processNode(scene->mRootNode, scene, *model);

    return model;
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene, Model& model) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.push_back(processMesh(assimpMesh, scene , model));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, model);
    }
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, Model& model) {
    Mesh newMesh;

    // Загрузка вершин (код остается таким же)
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        if (mesh->HasNormals()) {
            vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.TexCoords = glm::vec2(0.0f);
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
            vertex.Bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
        }
        newMesh.vertices.push_back(vertex);
    }

    // Загрузка индексов (код остается таким же)
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            newMesh.indices.push_back(face.mIndices[j]);
        }
    }

    // Присваиваем мешу индекс его материала.
    newMesh.materialIndex = mesh->mMaterialIndex;

    return newMesh;
}

// ИСПРАВЛЕННАЯ ВЕРСИЯ
unsigned int ModelLoader::loadMaterialTexture(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
    if (mat->GetTextureCount(type) == 0) {
        return ResourceManager::Get().GetDefaultTexture(typeName);
    }

    aiString str;
    mat->GetTexture(type, 0, &str);

    // Собираем путь: папка модели + имя файла текстуры
    std::string fullPath = this->directory + "/" + std::string(str.C_Str());

    // Вся магия теперь тут:
    return ResourceManager::Get().LoadTexture(fullPath, typeName);
}

// ИСПРАВЛЕННАЯ ФИНАЛЬНАЯ ВЕРСИЯ
unsigned int TextureFromFile(const char* path, const std::string& directory, const std::string& typeName) {
    std::string filename = std::string(path);
    std::replace(filename.begin(), filename.end(), '\\', '/');

    if (filename.length() > 2 && filename[1] == ':' && filename[2] == '/') {
        // Путь абсолютный, используем как есть
    } else {
        filename = directory + '/' + filename;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << filename << ". Using default." << std::endl;
        stbi_image_free(data);

        // Освобождаем ID, который мы сгенерировали, но не смогли использовать
        glDeleteTextures(1, &textureID);

        // ВОЗВРАЩАЕМ ДЕФОЛТНУЮ ТЕКСТУРУ ИЗ МЕНЕДЖЕРА!
        textureID = ResourceManager::Get().GetDefaultTexture(typeName);
    }

    return textureID;
}