//
// Created by egors on 09.12.2025.
//

#ifndef DUMPENGINE_DSTATICMESH_H
#define DUMPENGINE_DSTATICMESH_H
#include <vector>
#include "DBaseMesh.h"

#include "../Systems/ModelLoader.h"


class DBaseMesh;

class DStaticMesh {
public:
    DStaticMesh() = default;
    ~DStaticMesh();

private:
    std::vector<DBaseMesh*> _meshes;




public:
    DStaticMesh(const std::string& path) {
        ModelLoader loader;
        std::unique_ptr<Model> loadedModel = loader.LoadModel(path);

        if (loadedModel) {
            materials = loadedModel->materials;

            for (const auto& meshData : loadedModel->meshes) {

                auto baseMesh = std::make_unique<DBaseMesh>();
                baseMesh->InitMesh(meshData);
                baseMesh->materialIndex = meshData.materialIndex;
                subMeshes.push_back(std::move(baseMesh));
            }
        }
    }

    DMaterial& GetMaterial(unsigned int index) {
        if (index >= materials.size()) {

            static DMaterial dummy;
            return dummy;
        }
        return materials[index];
    }


    void SetMaterial(unsigned int index, const DMaterial& newMat) {
        if (index < materials.size()) {
            materials[index] = newMat;
        }
    }

    const std::vector<std::unique_ptr<DBaseMesh>>& GetAllMeshes() const { return subMeshes; }
    const DMaterial& GetMaterial(unsigned int index) const { return materials[index]; }
    size_t GetMaterialCount() const { return materials.size(); }

private:
    std::vector<std::unique_ptr<DBaseMesh>> subMeshes;
    std::vector<DMaterial> materials;
};




#endif //DUMPENGINE_DSTATICMESH_H