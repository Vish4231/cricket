#include "Model.h"
#include <string>

Model::Model() {}
Model::~Model() {}

bool Model::loadFromFile(const std::string& path) {
    // Stub: always succeed
    return true;
}

void Model::draw(const Shader& shader) const {
    // Stub: do nothing
} 