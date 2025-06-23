#include "TextureManager.h"
#include <iostream>
#include <stb_image.h>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

GLuint TextureManager::loadTexture(const std::string& filename, const std::string& textureName) {
    // Check if texture already loaded
    if (textures.find(textureName) != textures.end()) {
        return textures[textureName];
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
        
        textures[textureName] = textureID;
        std::cout << "Loaded texture: " << filename << " as " << textureName << std::endl;
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
    
    return textureID;
}

GLuint TextureManager::getTexture(const std::string& textureName) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        return it->second;
    }
    return 0;
}

void TextureManager::bindTexture(const std::string& textureName, GLuint unit) {
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, it->second);
    }
}

void TextureManager::cleanup() {
    for (auto& pair : textures) {
        glDeleteTextures(1, &pair.second);
    }
    textures.clear();
}

bool TextureManager::hasTexture(const std::string& textureName) {
    return textures.find(textureName) != textures.end();
} 