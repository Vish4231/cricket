#include "Model.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Model::Model() 
    : transform(1.0f), position(0.0f), rotation(0.0f), scale(1.0f)
    , minBounds(0.0f), maxBounds(0.0f), center(0.0f), radius(0.0f)
    , animationTime(0.0f), isAnimating(false)
    , totalVertices(0), totalIndices(0) {
}

Model::~Model() {
    cleanup();
}

void Model::cleanup() {
    meshes.clear();
    textures.clear();
    animationsMap.clear();
}

bool Model::loadFromFile(const std::string& filepath) {
    this->path = filepath;
    this->name = filepath.substr(filepath.find_last_of("/\\") + 1);
    
    // For now, create a default cricket player model
    createDefaultPlayerModel();
    
    return true;
}

bool Model::loadFromMemory(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // Create a single mesh from the provided data
    Material defaultMaterial;
    defaultMaterial.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    defaultMaterial.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    defaultMaterial.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    defaultMaterial.shininess = 32.0f;
    
    auto mesh = std::make_unique<Mesh>(vertices, indices, std::vector<Texture>(), defaultMaterial);
    meshes.push_back(std::move(mesh));
    
    totalVertices = vertices.size();
    totalIndices = indices.size();
    
    calculateBoundingBox();
    return true;
}

void Model::draw(Shader& shader) {
    // Calculate final model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    
    // Apply animation transform if animating
    if (isAnimating && !currentAnimation.empty()) {
        auto it = animationsMap.find(currentAnimation);
        if (it != animationsMap.end()) {
            glm::mat4 animTransform = interpolateKeyframes(it->second, animationTime);
            model = model * animTransform;
        }
    }
    
    shader.setMat4("model", model);
    
    // Draw all meshes
    for (const auto& mesh : meshes) {
        mesh->draw(shader);
    }
}

void Model::drawInstanced(Shader& shader, int instanceCount) {
    // Simplified instanced drawing
    for (int i = 0; i < instanceCount; ++i) {
        draw(shader);
    }
}

void Model::setTransform(const glm::mat4& transform) {
    this->transform = transform;
}

void Model::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Model::setRotation(const glm::vec3& rot) {
    rotation = rot;
}

void Model::setScale(const glm::vec3& scl) {
    scale = scl;
}

void Model::setMaterial(const Material& mat) {
    material = mat;
    for (auto& mesh : meshes) {
        mesh->material = mat;
    }
}

void Model::setTexture(const std::string& type, GLuint textureId) {
    // Simplified texture setting
}

void Model::setAnimationTime(float time) {
    animationTime = time;
}

void Model::addAnimation(const std::string& name, const Animation& animation) {
    animationsMap[name] = animation;
    animations.push_back(name);
}

void Model::playAnimation(const std::string& name, float time) {
    if (animationsMap.find(name) != animationsMap.end()) {
        currentAnimation = name;
        animationTime = time;
        isAnimating = true;
    }
}

void Model::updateAnimation(float deltaTime) {
    if (!isAnimating || currentAnimation.empty()) return;
    
    auto it = animationsMap.find(currentAnimation);
    if (it != animationsMap.end()) {
        animationTime += deltaTime;
        if (animationTime >= it->second.duration) {
            animationTime = 0.0f; // Loop animation
        }
    }
}

glm::mat4 Model::getCurrentTransform() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

bool Model::hasAnimation(const std::string& name) const {
    return animationsMap.find(name) != animationsMap.end();
}

void Model::createDefaultPlayerModel() {
    // Create a simple cricket player model
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Body (torso) - simplified cube
    float bodyHeight = 1.0f;
    float bodyWidth = 0.4f;
    float bodyDepth = 0.2f;
    
    // Front face
    vertices.push_back({{-bodyWidth/2, -bodyHeight/2, bodyDepth/2}, {0,0,1}, {0,0}, {1,0,0}, {0,1,0}});
    vertices.push_back({{bodyWidth/2, -bodyHeight/2, bodyDepth/2}, {0,0,1}, {1,0}, {1,0,0}, {0,1,0}});
    vertices.push_back({{bodyWidth/2, bodyHeight/2, bodyDepth/2}, {0,0,1}, {1,1}, {1,0,0}, {0,1,0}});
    vertices.push_back({{-bodyWidth/2, bodyHeight/2, bodyDepth/2}, {0,0,1}, {0,1}, {1,0,0}, {0,1,0}});
    
    // Back face
    vertices.push_back({{-bodyWidth/2, -bodyHeight/2, -bodyDepth/2}, {0,0,-1}, {0,0}, {1,0,0}, {0,1,0}});
    vertices.push_back({{bodyWidth/2, -bodyHeight/2, -bodyDepth/2}, {0,0,-1}, {1,0}, {1,0,0}, {0,1,0}});
    vertices.push_back({{bodyWidth/2, bodyHeight/2, -bodyDepth/2}, {0,0,-1}, {1,1}, {1,0,0}, {0,1,0}});
    vertices.push_back({{-bodyWidth/2, bodyHeight/2, -bodyDepth/2}, {0,0,-1}, {0,1}, {1,0,0}, {0,1,0}});
    
    // Create indices for faces
    indices = {0, 1, 2, 2, 3, 0,  // Front
               4, 5, 6, 6, 7, 4,  // Back
               0, 4, 7, 7, 3, 0,  // Left
               1, 5, 6, 6, 2, 1}; // Right
    
    // Create material
    Material playerMaterial;
    playerMaterial.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    playerMaterial.diffuse = glm::vec3(0.8f, 0.6f, 0.4f); // Skin tone
    playerMaterial.specular = glm::vec3(0.1f, 0.1f, 0.1f);
    playerMaterial.shininess = 16.0f;
    
    // Create mesh
    auto mesh = std::make_unique<Mesh>(vertices, indices, std::vector<Texture>(), playerMaterial);
    meshes.push_back(std::move(mesh));
    
    totalVertices = vertices.size();
    totalIndices = indices.size();
    
    // Add default animations
    addDefaultAnimations();
    
    calculateBoundingBox();
}

void Model::addDefaultAnimations() {
    // Idle animation
    Animation idle;
    idle.name = "idle";
    idle.duration = 2.0f;
    
    for (int i = 0; i < 10; ++i) {
        float t = (float)i / 9.0f;
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0, sin(t * 6.28f) * 0.02f, 0));
        idle.keyframes.push_back(transform);
        idle.keyframeTimes.push_back(t * idle.duration);
    }
    addAnimation("idle", idle);
    
    // Batting animation
    Animation batting;
    batting.name = "batting";
    batting.duration = 1.5f;
    
    for (int i = 0; i < 15; ++i) {
        float t = (float)i / 14.0f;
        glm::mat4 transform = glm::mat4(1.0f);
        float swingAngle = sin(t * 3.14f) * 45.0f;
        transform = glm::rotate(transform, glm::radians(swingAngle), glm::vec3(0, 1, 0));
        batting.keyframes.push_back(transform);
        batting.keyframeTimes.push_back(t * batting.duration);
    }
    addAnimation("batting", batting);
    
    // Bowling animation
    Animation bowling;
    bowling.name = "bowling";
    bowling.duration = 2.0f;
    
    for (int i = 0; i < 20; ++i) {
        float t = (float)i / 19.0f;
        glm::mat4 transform = glm::mat4(1.0f);
        if (t < 0.7f) {
            float runSpeed = t / 0.7f;
            transform = glm::translate(transform, glm::vec3(0, 0, -runSpeed * 2.0f));
        } else {
            float deliveryPhase = (t - 0.7f) / 0.3f;
            float armSwing = sin(deliveryPhase * 3.14f) * 90.0f;
            transform = glm::rotate(transform, glm::radians(armSwing), glm::vec3(1, 0, 0));
        }
        bowling.keyframes.push_back(transform);
        bowling.keyframeTimes.push_back(t * bowling.duration);
    }
    addAnimation("bowling", bowling);
}

void Model::calculateBoundingBox() {
    if (meshes.empty()) return;
    
    minBounds = glm::vec3(FLT_MAX);
    maxBounds = glm::vec3(-FLT_MAX);
    
    for (const auto& mesh : meshes) {
        for (const auto& vertex : mesh->vertices) {
            minBounds = glm::min(minBounds, vertex.position);
            maxBounds = glm::max(maxBounds, vertex.position);
        }
    }
    
    center = (minBounds + maxBounds) * 0.5f;
    radius = glm::length(maxBounds - minBounds) * 0.5f;
}

void Model::updateTransform() {
    // Transform is calculated on-demand in draw()
}

GLuint Model::loadTexture(const std::string& path) {
    // Simplified texture loading
    return 0;
}

GLuint Model::loadTextureFromMemory(const unsigned char* data, int width, int height, int channels) {
    // Simplified texture loading from memory
    return 0;
}

void Model::setupMesh() {
    // Mesh setup is handled by Mesh constructor
}

glm::mat4 Model::interpolateKeyframes(const Animation& anim, float time) const {
    if (anim.keyframes.empty()) return glm::mat4(1.0f);
    
    // Find the two keyframes to interpolate between
    size_t index1 = 0, index2 = 0;
    for (size_t i = 0; i < anim.keyframeTimes.size(); ++i) {
        if (anim.keyframeTimes[i] <= time) {
            index1 = i;
        }
        if (anim.keyframeTimes[i] > time) {
            index2 = i;
            break;
        }
    }
    
    if (index1 == index2) {
        return anim.keyframes[index1];
    }
    
    // Interpolate between keyframes manually (glm::mix doesn't work with matrices)
    float t1 = anim.keyframeTimes[index1];
    float t2 = anim.keyframeTimes[index2];
    float alpha = (time - t1) / (t2 - t1);
    
    const glm::mat4& mat1 = anim.keyframes[index1];
    const glm::mat4& mat2 = anim.keyframes[index2];
    
    glm::mat4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = glm::mix(mat1[i][j], mat2[i][j], alpha);
        }
    }
    
    return result;
}

// Mesh implementation
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, 
           const std::vector<Texture>& textures, const Material& material)
    : vertices(vertices), indices(indices), textures(textures), material(material), VAO(0), VBO(0), EBO(0) {
    setupMesh();
}

Mesh::~Mesh() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    
    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader) {
    // Set material properties
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setFloat("material.shininess", material.shininess);
    
    // Bind textures
    for (size_t i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        shader.setInt(("material." + textures[i].type).c_str(), i);
    }
    
    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Unbind textures
    for (size_t i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
} 