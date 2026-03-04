#include "DBaseMesh.h"
#include <glad/glad.h>
#include <utility> // для std::swap



DBaseMesh::~DBaseMesh() {
    release();
}


DBaseMesh::DBaseMesh(DBaseMesh&& other) noexcept {
    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;
    IndexCount = other.IndexCount;
    materialIndex = other.materialIndex;


    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.IndexCount = 0;
}


DBaseMesh& DBaseMesh::operator=(DBaseMesh&& other) noexcept {
    if (this != &other) {

        release();

        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        IndexCount = other.IndexCount;
        materialIndex = other.materialIndex;

        // Обнуляем "донора".
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

void DBaseMesh::release() {
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    EBO = VBO = VAO = 0;
}



void DBaseMesh::InitMesh(const Mesh& mesh) {
    this->IndexCount = static_cast<unsigned int>(mesh.indices.size());


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glBindVertexArray(0);
}