#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <memory>

class TextureManager {
public:
    static TextureManager& getInstance();
    
    // Load texture from file
    GLuint loadTexture(const std::string& filename, const std::string& textureName);
    
    // Get texture by name
    GLuint getTexture(const std::string& textureName);
    
    // Bind texture
    void bindTexture(const std::string& textureName, GLuint unit = 0);
    
    // Cleanup
    void cleanup();
    
    // Check if texture exists
    bool hasTexture(const std::string& textureName);

private:
    TextureManager() = default;
    ~TextureManager() = default;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    
    std::unordered_map<std::string, GLuint> textures;
}; 