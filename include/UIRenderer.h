#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader;

struct UIElement {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
    std::string textureName;
    std::string text;
    bool isVisible;
    bool isHovered;
    bool isClicked;
};

class UIRenderer {
public:
    UIRenderer();
    ~UIRenderer();
    
    bool initialize();
    void cleanup();
    
    // Rendering functions
    void renderPanel(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const std::string& textureName = "");
    void renderButton(const glm::vec2& position, const glm::vec2& size, const std::string& text, const glm::vec4& color, bool isHovered = false);
    void renderText(const glm::vec2& position, const std::string& text, const glm::vec4& color, float scale = 1.0f);
    void renderProgressBar(const glm::vec2& position, const glm::vec2& size, float progress, const glm::vec4& color);
    void renderIcon(const glm::vec2& position, const glm::vec2& size, const std::string& textureName);
    
    // Cricket-specific UI elements
    void renderScoreboard(const glm::vec2& position, const std::string& team1, const std::string& team2, int score1, int score2, int overs1, int overs2);
    void renderPlayerCard(const glm::vec2& position, const std::string& playerName, int battingSkill, int bowlingSkill, int fieldingSkill);
    void renderTeamLogo(const glm::vec2& position, const std::string& teamName);
    void renderMatchStatus(const glm::vec2& position, const std::string& status, const glm::vec4& color);
    
    // Set viewport
    void setViewport(int width, int height);

private:
    Shader* uiShader;
    GLuint quadVAO, quadVBO;
    int viewportWidth, viewportHeight;
    
    // Helper functions
    void setupQuad();
    glm::mat4 getProjectionMatrix();
}; 