#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "ResourceManager.h"

// --- СТРУКТУРЫ ДАННЫХ ---

// Структура Вершины (остается без изменений)
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

// НОВЫЙ КЛАСС: DMaterial
// Инкапсулирует все текстуры и параметры для одной поверхности.
struct DMaterial {
    // ID текстур. 0 - означает отсутствие текстуры.
    unsigned int albedoMap = 0;
    unsigned int normalMap = 0;
    unsigned int metallicMap = 0;
    unsigned int roughnessMap = 0;
    unsigned int aoMap = 0;
    // Сюда можно добавить и другие параметры, например, цвет по умолчанию
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

// Меш теперь не хранит текстуры, а ссылается на материал по индексу.
struct Mesh {
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    unsigned int              materialIndex;
};

// НОВЫЙ КЛАСС: Model
// Объединяет все меши и материалы, загруженные из одного файла.
struct Model {
    std::vector<Mesh>      meshes;
    std::vector<DMaterial> materials;
};

struct Texture {
    unsigned int id;
    std::string type; // "texture_diffuse", "texture_normal", etc.
    std::string path; // Путь к файлу, для кэширования.
};


// --- КЛАСС ЗАГРУЗЧИКА ---

class ModelLoader {
public:
    // Главная функция теперь возвращает указатель на полностью собранную модель.
    // Использование unique_ptr - хорошая практика для управления памятью.
    std::unique_ptr<Model> LoadModel(const std::string& path);

private:
    // Вспомогательные структуры (теперь не статические, а члены класса)
    std::string directory;
    std::vector<Texture> textures_loaded; // Кэш для уже загруженных текстур.

    // Рекурсивно обходит дерево узлов сцены Assimp.
    void processNode(aiNode* node, const aiScene* scene, Model& model);

    // Обрабатывает один меш Assimp.
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);

    // Загружает одну текстуру для материала (возвращает ID).
    unsigned int loadMaterialTexture(aiMaterial* mat, aiTextureType type ,  const std::string& typeName);
};

#endif // MODEL_LOADER_H