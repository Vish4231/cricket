#include "UIRenderer.h"
#include "Shader.h"
#include "TextureManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

UIRenderer::UIRenderer() : uiShader(nullptr), quadVAO(0), quadVBO(0), viewportWidth(800), viewportHeight(600) {
}

UIRenderer::~UIRenderer() {
    cleanup();
}

bool UIRenderer::initialize() {
    // Create UI shader
    uiShader = new Shader();
    if (!uiShader->loadFromFiles("shaders/ui_vertex.glsl", "shaders/ui_fragment.glsl")) {
        std::cerr << "Failed to load UI shader" << std::endl;
        return false;
    }
    
    // Setup quad for UI rendering
    setupQuad();
    
    // Note: Texture loading removed for now - will use solid colors
    // TODO: Add proper texture loading when assets are available
    
    return true;
}

void UIRenderer::cleanup() {
    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }
    if (quadVBO) {
        glDeleteBuffers(1, &quadVBO);
        quadVBO = 0;
    }
    if (uiShader) {
        delete uiShader;
        uiShader = nullptr;
    }
}

void UIRenderer::setupQuad() {
    float vertices[] = {
        // positions   // texCoords
         0.0f,  1.0f,  0.0f, 1.0f,  // top-left
         0.0f,  0.0f,  0.0f, 0.0f,  // bottom-left
         1.0f,  0.0f,  1.0f, 0.0f,  // bottom-right
         1.0f,  1.0f,  1.0f, 1.0f   // top-right
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

void UIRenderer::renderPanel(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const std::string& textureName) {
    if (!uiShader) {
        return;
    }
    
    uiShader->use();
    
    // Set uniforms
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    
    uiShader->setMat4("model", model);
    uiShader->setMat4("projection", getProjectionMatrix());
    uiShader->setVec4("color", color);
    uiShader->setBool("useTexture", !textureName.empty());
    
    // Bind texture if provided
    if (!textureName.empty()) {
        auto& textureManager = TextureManager::getInstance();
        if (textureManager.hasTexture(textureName)) {
            textureManager.bindTexture(textureName, 0);
            uiShader->setInt("texture1", 0);
        }
    }
    
    // Render quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void UIRenderer::renderButton(const glm::vec2& position, const glm::vec2& size, const std::string& text, const glm::vec4& color, bool isHovered) {
    // Use solid colors instead of textures
    glm::vec4 buttonColor = isHovered ? 
        glm::vec4(color.r * 1.2f, color.g * 1.2f, color.b * 1.2f, color.a) :
        color;
    
    renderPanel(position, size, buttonColor, "");
    
    // Render text (simplified - in a real implementation you'd use a text renderer)
    renderText(glm::vec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f), text, glm::vec4(1.0f));
}

void UIRenderer::renderText(const glm::vec2& position, const std::string& text, const glm::vec4& color, float scale) {
    // Simplified text rendering - in a real implementation you'd use FreeType or similar
    // For now, we'll just render a colored rectangle as placeholder
    renderPanel(position, glm::vec2(text.length() * 10 * scale, 20 * scale), glm::vec4(color), "");
}

void UIRenderer::renderProgressBar(const glm::vec2& position, const glm::vec2& size, float progress, const glm::vec4& color) {
    // Render background
    renderPanel(position, size, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), "");
    
    // Render progress
    glm::vec2 progressSize = glm::vec2(size.x * progress, size.y);
    renderPanel(position, progressSize, glm::vec4(color), "");
}

void UIRenderer::renderIcon(const glm::vec2& position, const glm::vec2& size, const std::string& textureName) {
    renderPanel(position, size, glm::vec4(1.0f), textureName);
}

void UIRenderer::renderScoreboard(const glm::vec2& position, const std::string& team1, const std::string& team2, 
                                 int score1, int score2, int overs1, int overs2) {
    // Render scoreboard background
    renderPanel(position, glm::vec2(400, 200), glm::vec4(0.1f, 0.1f, 0.1f, 0.9f), "");
    
    // Render team names and scores
    renderText(glm::vec2(position.x + 20, position.y + 20), team1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    renderText(glm::vec2(position.x + 20, position.y + 50), std::to_string(score1) + "/" + std::to_string(overs1), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    
    renderText(glm::vec2(position.x + 200, position.y + 20), team2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    renderText(glm::vec2(position.x + 200, position.y + 50), std::to_string(score2) + "/" + std::to_string(overs2), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
}

void UIRenderer::renderPlayerCard(const glm::vec2& position, const std::string& playerName, 
                                 int battingSkill, int bowlingSkill, int fieldingSkill) {
    // Render card background
    renderPanel(position, glm::vec2(200, 150), glm::vec4(0.2f, 0.2f, 0.2f, 0.9f), "");
    
    // Render player name
    renderText(glm::vec2(position.x + 10, position.y + 10), playerName, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Render skill bars
    renderText(glm::vec2(position.x + 10, position.y + 40), "Batting", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    renderProgressBar(glm::vec2(position.x + 10, position.y + 60), glm::vec2(180, 10), battingSkill / 100.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    
    renderText(glm::vec2(position.x + 10, position.y + 80), "Bowling", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    renderProgressBar(glm::vec2(position.x + 10, position.y + 100), glm::vec2(180, 10), bowlingSkill / 100.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    
    renderText(glm::vec2(position.x + 10, position.y + 120), "Fielding", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    renderProgressBar(glm::vec2(position.x + 10, position.y + 140), glm::vec2(180, 10), fieldingSkill / 100.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void UIRenderer::renderTeamLogo(const glm::vec2& position, const std::string& teamName) {
    std::string textureName = "logo_" + teamName;
    renderIcon(position, glm::vec2(64, 64), textureName);
}

void UIRenderer::renderMatchStatus(const glm::vec2& position, const std::string& status, const glm::vec4& color) {
    renderPanel(position, glm::vec2(300, 50), glm::vec4(0.1f, 0.1f, 0.1f, 0.8f), "");
    renderText(glm::vec2(position.x + 10, position.y + 15), status, color);
}

void UIRenderer::setViewport(int width, int height) {
    std::cout << "UIRenderer::setViewport called with " << width << "x" << height << std::endl;
    viewportWidth = width;
    viewportHeight = height;
}

glm::mat4 UIRenderer::getProjectionMatrix() {
    std::cout << "UIRenderer::getProjectionMatrix called with viewport " << viewportWidth << "x" << viewportHeight << std::endl;
    return glm::ortho(0.0f, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0.0f, -1.0f, 1.0f);
} 