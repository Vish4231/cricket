#include "TextureManager.h"
#include <glad/glad.h>
#include <iostream>
#include <map>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

void TextureManager::cleanup() {
    for (auto& pair : textures) {
        if (pair.second != 0) {
            glDeleteTextures(1, &pair.second);
        }
    }
    textures.clear();
}

GLuint TextureManager::loadTexture(const std::string& filename, const std::string& textureName) {
    // Check if texture already loaded
    if (textures.find(textureName) != textures.end()) {
        return textures[textureName];
    }
    
    // Simplified texture loading - create a 1x1 colored texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Create a simple colored texture (white for now)
    unsigned char data[] = {255, 255, 255, 255}; // RGBA white
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    textures[textureName] = textureID;
    std::cout << "Loaded texture: " << filename << " as " << textureName << std::endl;
    return textureID;
}

bool TextureManager::hasTexture(const std::string& textureName) {
    return textures.find(textureName) != textures.end();
}

void TextureManager::bindTexture(const std::string& textureName, GLuint unit) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, it->second);
    }
}

GLuint TextureManager::getTexture(const std::string& textureName) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        return it->second;
    }
    return 0;
} 