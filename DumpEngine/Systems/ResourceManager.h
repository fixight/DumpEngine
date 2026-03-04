//
// Created by egors on 18.01.2026.
//

#ifndef DUMPENGINE_RESOURCEMANAGER_H
#define DUMPENGINE_RESOURCEMANAGER_H
#include <map>
#include <memory>
#include <string>


class DStaticMesh;

class ResourceManager {
public:
    static ResourceManager& Get();

    std::shared_ptr<DStaticMesh> LoadStaticMesh(const std::string& path);

    unsigned int LoadTexture(const std::string& path, const std::string& typeName = "texture_diffuse");

    void InitDefaultTextures();

    unsigned int GetDefaultTexture(const std::string& typeName);

private:
    ResourceManager() = default;
    std::map<std::string, std::shared_ptr<DStaticMesh>> meshCache;
    std::map<std::string, unsigned int> textureCache;


    unsigned int defaultAlbedo = 0;
    unsigned int defaultNormal = 0;
    unsigned int defaultMetallic = 0;
    unsigned int defaultRoughness = 0;
    unsigned int defaultAO = 0;
};

#endif //DUMPENGINE_RESOURCEMANAGER_H