#include "Model.h"
#include <string>

Model::Model() {}
Model::~Model() {}

bool Model::loadFromFile(const std::string& path) {
    // Stub: always succeed
    return true;
}

void Model::draw(const Shader& shader) {
    // Stub: do nothing
}

// If Mesh is defined here, add:
Mesh::~Mesh() {
    // TODO: Implement or leave empty if not needed
} 