#include "AnimationHandler.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

AnimationHandler::AnimationHandler() : 
    currentAnimation(nullptr),
    animationTime(0.0f),
    isPlaying(false),
    loopAnimation(true) {
    
    // Initialize OpenGL state for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

AnimationHandler::~AnimationHandler() {
    // Clean up loaded models and animations
    for (auto& model : loadedModels) {
        delete model.second;
    }
    loadedModels.clear();
}

bool AnimationHandler::LoadModel(const std::string& filename, const std::string& modelName) {
    try {
        // Placeholder for model loading - in a real implementation, this would use Assimp or similar
        std::cout << "Loading model: " << filename << " as " << modelName << std::endl;
        
        // Create a placeholder model structure
        Model* model = new Model();
        model->name = modelName;
        model->path = filename;
        
        // Add some basic geometry for testing
        model->vertices = {
            // Player body (simplified cube)
            glm::vec3(-0.5f, -1.0f, -0.3f),
            glm::vec3( 0.5f, -1.0f, -0.3f),
            glm::vec3( 0.5f,  1.0f, -0.3f),
            glm::vec3(-0.5f,  1.0f, -0.3f),
            
            // Back face
            glm::vec3(-0.5f, -1.0f,  0.3f),
            glm::vec3( 0.5f, -1.0f,  0.3f),
            glm::vec3( 0.5f,  1.0f,  0.3f),
            glm::vec3(-0.5f,  1.0f,  0.3f),
        };
        
        model->indices = {
            0, 1, 2, 2, 3, 0,  // Front face
            4, 5, 6, 6, 7, 4,  // Back face
            0, 4, 7, 7, 3, 0,  // Left face
            1, 5, 6, 6, 2, 1,  // Right face
            3, 2, 6, 6, 7, 3,  // Top face
            0, 1, 5, 5, 4, 0   // Bottom face
        };
        
        // Generate OpenGL buffers
        glGenVertexArrays(1, &model->vao);
        glGenBuffers(1, &model->vbo);
        glGenBuffers(1, &model->ebo);
        
        glBindVertexArray(model->vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
        glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * sizeof(glm::vec3), 
                    model->vertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indices.size() * sizeof(unsigned int), 
                    model->indices.data(), GL_STATIC_DRAW);
        
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
        
        loadedModels[modelName] = model;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading model " << filename << ": " << e.what() << std::endl;
        return false;
    }
}

void AnimationHandler::PlayAnimation(const std::string& playerName, AnimationType type) {
    auto it = loadedAnimations.find(playerName);
    if (it != loadedAnimations.end()) {
        currentAnimation = it->second;
        animationTime = 0.0f;
        isPlaying = true;
        loopAnimation = true;
        std::cout << "Playing animation: " << playerName << std::endl;
    } else {
        std::cerr << "Animation not found: " << playerName << std::endl;
    }
}

void AnimationHandler::StopAnimation(const std::string& playerName) {
    isPlaying = false;
    currentAnimation = nullptr;
    animationTime = 0.0f;
}

void AnimationHandler::Update(float deltaTime) {
    if (!isPlaying || !currentAnimation) {
        return;
    }
    
    animationTime += deltaTime;
    
    // Check if animation should loop
    if (animationTime >= currentAnimation->duration) {
        if (loopAnimation) {
            animationTime = fmod(animationTime, currentAnimation->duration);
        } else {
            StopAnimation("");
            return;
        }
    }
} 