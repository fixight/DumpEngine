#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "ResourceManager.h"


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};


struct DMaterial {

    unsigned int albedoMap = 0;
    unsigned int normalMap = 0;
    unsigned int metallicMap = 0;
    unsigned int roughnessMap = 0;
    unsigned int aoMap = 0;

    glm::vec3 albedoColor = glm::vec3(1.0f);

    void SetAlbedo(const std::string& path) {
        albedoMap = ResourceManager::Get().LoadTexture(path, "texture_diffuse");
    }

    void SetNormal(const std::string& path) {
        normalMap = ResourceManager::Get().LoadTexture(path, "texture_normal");
    }

    void SetMetallic(const std::string& path) {
        metallicMap = ResourceManager::Get().LoadTexture(path, "texture_metallic");
    }

    void SetRoughness(const std::string& path) {
        roughnessMap = ResourceManager::Get().LoadTexture(path, "texture_roughness");
    }
    void SetAO(const std::string& path) {
        aoMap = ResourceManager::Get().LoadTexture(path, "texture_roughness");
    }

};


struct Mesh {
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    unsigned int              materialIndex;
};


struct Model {
    std::vector<Mesh>      meshes;
    std::vector<DMaterial> materials;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};



class ModelLoader {
public:

    std::unique_ptr<Model> LoadModel(const std::string& path);

private:

    std::string directory;
    std::vector<Texture> textures_loaded;

    void processNode(aiNode* node, const aiScene* scene, Model& model);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);


    unsigned int loadMaterialTexture(aiMaterial* mat, aiTextureType type ,  const std::string& typeName);
};

#endif // MODEL_LOADER_H