#include "AnimationHandler.h"
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

bool AnimationHandler::loadModel(const std::string& modelPath, const std::string& modelName) {
    try {
        // Placeholder for model loading - in a real implementation, this would use Assimp or similar
        std::cout << "Loading model: " << modelPath << " as " << modelName << std::endl;
        
        // Create a placeholder model structure
        Model* model = new Model();
        model->name = modelName;
        model->path = modelPath;
        
        // Add some basic geometry for testing
        model->vertices = {
            // Player body (simplified cube)
            -0.5f, -1.0f, -0.3f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
             0.5f, -1.0f, -0.3f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
             0.5f,  1.0f, -0.3f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f,  1.0f, -0.3f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
            
            // Back face
            -0.5f, -1.0f,  0.3f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
             0.5f, -1.0f,  0.3f,  0.0f, 0.0f,  1.0f,  1.0f, 0.0f,
             0.5f,  1.0f,  0.3f,  0.0f, 0.0f,  1.0f,  1.0f, 1.0f,
            -0.5f,  1.0f,  0.3f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,
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
        glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * sizeof(float), 
                    model->vertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indices.size() * sizeof(unsigned int), 
                    model->indices.data(), GL_STATIC_DRAW);
        
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        glBindVertexArray(0);
        
        loadedModels[modelName] = model;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading model " << modelPath << ": " << e.what() << std::endl;
        return false;
    }
}

bool AnimationHandler::loadAnimation(const std::string& animPath, const std::string& animName) {
    try {
        std::cout << "Loading animation: " << animPath << " as " << animName << std::endl;
        
        // Create placeholder animation
        Animation* animation = new Animation();
        animation->name = animName;
        animation->duration = 2.0f; // 2 seconds
        animation->ticksPerSecond = 30.0f;
        
        // Add some keyframes for testing
        for (int i = 0; i < 60; ++i) { // 60 keyframes over 2 seconds
            KeyFrame frame;
            frame.time = (float)i / 30.0f; // 30 fps
            
            // Simple rotation animation
            float angle = (float)i * 6.0f; // 360 degrees over 60 frames
            frame.transform = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            
            animation->keyframes.push_back(frame);
        }
        
        loadedAnimations[animName] = animation;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading animation " << animPath << ": " << e.what() << std::endl;
        return false;
    }
}

void AnimationHandler::playAnimation(const std::string& animName, bool loop) {
    auto it = loadedAnimations.find(animName);
    if (it != loadedAnimations.end()) {
        currentAnimation = it->second;
        animationTime = 0.0f;
        isPlaying = true;
        loopAnimation = loop;
        std::cout << "Playing animation: " << animName << std::endl;
    } else {
        std::cerr << "Animation not found: " << animName << std::endl;
    }
}

void AnimationHandler::stopAnimation() {
    isPlaying = false;
    currentAnimation = nullptr;
    animationTime = 0.0f;
}

void AnimationHandler::pauseAnimation() {
    isPlaying = false;
}

void AnimationHandler::resumeAnimation() {
    if (currentAnimation) {
        isPlaying = true;
    }
}

void AnimationHandler::update(float deltaTime) {
    if (!isPlaying || !currentAnimation) {
        return;
    }
    
    animationTime += deltaTime;
    
    // Check if animation should loop
    if (animationTime >= currentAnimation->duration) {
        if (loopAnimation) {
            animationTime = fmod(animationTime, currentAnimation->duration);
        } else {
            stopAnimation();
            return;
        }
    }
}

glm::mat4 AnimationHandler::getCurrentTransform() const {
    if (!currentAnimation || currentAnimation->keyframes.empty()) {
        return glm::mat4(1.0f);
    }
    
    // Find the two keyframes to interpolate between
    size_t frameIndex = 0;
    for (size_t i = 0; i < currentAnimation->keyframes.size() - 1; ++i) {
        if (animationTime >= currentAnimation->keyframes[i].time && 
            animationTime < currentAnimation->keyframes[i + 1].time) {
            frameIndex = i;
            break;
        }
    }
    
    if (frameIndex >= currentAnimation->keyframes.size() - 1) {
        return currentAnimation->keyframes.back().transform;
    }
    
    const KeyFrame& frame1 = currentAnimation->keyframes[frameIndex];
    const KeyFrame& frame2 = currentAnimation->keyframes[frameIndex + 1];
    
    // Calculate interpolation factor
    float factor = (animationTime - frame1.time) / (frame2.time - frame1.time);
    factor = glm::clamp(factor, 0.0f, 1.0f);
    
    // Linear interpolation between transforms
    glm::mat4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = glm::mix(frame1.transform[i][j], frame2.transform[i][j], factor);
        }
    }
    
    return result;
}

void AnimationHandler::renderModel(const std::string& modelName, const glm::mat4& modelMatrix, 
                                 const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    auto it = loadedModels.find(modelName);
    if (it == loadedModels.end()) {
        std::cerr << "Model not found: " << modelName << std::endl;
        return;
    }
    
    Model* model = it->second;
    
    // Get current animation transform
    glm::mat4 animationTransform = getCurrentTransform();
    glm::mat4 finalModelMatrix = modelMatrix * animationTransform;
    
    // Set up shader uniforms (assuming shader is already bound)
    // In a real implementation, you'd pass the shader program here
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(finalModelMatrix));
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    
    // Render the model
    glBindVertexArray(model->vao);
    glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void AnimationHandler::renderPlayer(const Player& player, const glm::vec3& position, 
                                  const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    // Create model matrix for player position
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    
    // Scale based on player role (batsmen slightly taller, bowlers broader)
    float scale = 1.0f;
    if (player.getRole() == PlayerRole::Batsman) {
        scale = 1.1f;
    } else if (player.getRole() == PlayerRole::Bowler) {
        scale = 1.05f;
    }
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    
    // Render player model
    renderModel("player", modelMatrix, viewMatrix, projectionMatrix);
}

void AnimationHandler::renderBall(const glm::vec3& position, const glm::mat4& viewMatrix, 
                                const glm::mat4& projectionMatrix) {
    // Create model matrix for ball position
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f)); // Ball is smaller
    
    // Render ball model
    renderModel("ball", modelMatrix, viewMatrix, projectionMatrix);
}

void AnimationHandler::renderPitch(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    // Create model matrix for pitch
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f, 0.1f, 3.0f)); // Pitch dimensions
    
    // Render pitch model
    renderModel("pitch", modelMatrix, viewMatrix, projectionMatrix);
}

std::vector<std::string> AnimationHandler::getLoadedModels() const {
    std::vector<std::string> models;
    for (const auto& pair : loadedModels) {
        models.push_back(pair.first);
    }
    return models;
}

std::vector<std::string> AnimationHandler::getLoadedAnimations() const {
    std::vector<std::string> animations;
    for (const auto& pair : loadedAnimations) {
        animations.push_back(pair.first);
    }
    return animations;
}

bool AnimationHandler::isAnimationPlaying() const {
    return isPlaying;
}

float AnimationHandler::getAnimationProgress() const {
    if (!currentAnimation) {
        return 0.0f;
    }
    return animationTime / currentAnimation->duration;
} 