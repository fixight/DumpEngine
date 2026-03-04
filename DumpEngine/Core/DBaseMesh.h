#ifndef DUMPENGINE_DBASEMESH_H
#define DUMPENGINE_DBASEMESH_H

#include "../Systems/ModelLoader.h"

class DBaseMesh {
public:
    DBaseMesh() = default;


    ~DBaseMesh();


    DBaseMesh(const DBaseMesh&) = delete;
    DBaseMesh& operator=(const DBaseMesh&) = delete;

    DBaseMesh(DBaseMesh&& other) noexcept;
    DBaseMesh& operator=(DBaseMesh&& other) noexcept;


    void InitMesh(const Mesh& mesh);


    unsigned int materialIndex = 0;

    unsigned int GetVAO() const { return VAO; }
    unsigned int GetIndexCount() const { return IndexCount; }

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    unsigned int IndexCount = 0;


    void release();
};

#endif // DUMPENGINE_DBASEMESH_H