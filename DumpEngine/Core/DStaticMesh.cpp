//
// Created by egors on 09.12.2025.
//

#include "DStaticMesh.h"
#include "DBaseMesh.h"


DStaticMesh::~DStaticMesh() {
    for (DBaseMesh* mesh : _meshes) {
        delete mesh;
    }
    _meshes.clear();
}



