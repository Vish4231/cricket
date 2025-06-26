#pragma once
#include <string>
#include <glm/glm.hpp>
#include <chrono>

class UIRenderer;

class AnimatedScoreboard {
public:
    AnimatedScoreboard(UIRenderer* renderer);
    void setTeams(const std::string& team1, const std::string& team2);
    void setScores(int score1, int wickets1, float overs1, int score2, int wickets2, float overs2);
    void triggerEvent(const std::string& eventText, const glm::vec4& color);
    void update(float deltaTime);
    void render(const glm::vec2& position);

private:
    UIRenderer* uiRenderer;
    std::string team1, team2;
    int displayedScore1, displayedWickets1, displayedScore2, displayedWickets2;
    float displayedOvers1, displayedOvers2;
    int targetScore1, targetWickets1, targetScore2, targetWickets2;
    float targetOvers1, targetOvers2;
    float animationTime, animationDuration;
    std::string eventText;
    glm::vec4 eventColor;
    float eventAlpha;
    bool eventActive;
}; 