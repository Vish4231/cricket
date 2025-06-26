#include "AnimatedScoreboard.h"
#include "UIRenderer.h"
#include <algorithm>

AnimatedScoreboard::AnimatedScoreboard(UIRenderer* renderer)
    : uiRenderer(renderer),
      displayedScore1(0), displayedWickets1(0), displayedOvers1(0),
      displayedScore2(0), displayedWickets2(0), displayedOvers2(0),
      targetScore1(0), targetWickets1(0), targetOvers1(0),
      targetScore2(0), targetWickets2(0), targetOvers2(0),
      animationTime(0), animationDuration(0.5f),
      eventAlpha(0), eventActive(false) {}

void AnimatedScoreboard::setTeams(const std::string& t1, const std::string& t2) {
    team1 = t1;
    team2 = t2;
}

void AnimatedScoreboard::setScores(int score1, int wickets1, float overs1, int score2, int wickets2, float overs2) {
    targetScore1 = score1;
    targetWickets1 = wickets1;
    targetOvers1 = overs1;
    targetScore2 = score2;
    targetWickets2 = wickets2;
    targetOvers2 = overs2;
    animationTime = 0.0f;
}

void AnimatedScoreboard::triggerEvent(const std::string& text, const glm::vec4& color) {
    eventText = text;
    eventColor = color;
    eventAlpha = 1.0f;
    eventActive = true;
}

void AnimatedScoreboard::update(float deltaTime) {
    // Animate score changes
    animationTime = std::min(animationTime + deltaTime, animationDuration);
    float t = animationDuration > 0 ? animationTime / animationDuration : 1.0f;
    
    displayedScore1 = static_cast<int>(displayedScore1 + (targetScore1 - displayedScore1) * t);
    displayedWickets1 = static_cast<int>(displayedWickets1 + (targetWickets1 - displayedWickets1) * t);
    displayedOvers1 = displayedOvers1 + (targetOvers1 - displayedOvers1) * t;
    displayedScore2 = static_cast<int>(displayedScore2 + (targetScore2 - displayedScore2) * t);
    displayedWickets2 = static_cast<int>(displayedWickets2 + (targetWickets2 - displayedWickets2) * t);
    displayedOvers2 = displayedOvers2 + (targetOvers2 - displayedOvers2) * t;
    
    // Fade out event
    if (eventActive) {
        eventAlpha -= deltaTime * 0.5f;
        if (eventAlpha <= 0.0f) {
            eventAlpha = 0.0f;
            eventActive = false;
        }
    }
}

void AnimatedScoreboard::render(const glm::vec2& position) {
    // Highlight leading team
    glm::vec4 team1Color = (displayedScore1 > displayedScore2) ? glm::vec4(0.2f, 0.8f, 0.2f, 1.0f) : glm::vec4(1.0f);
    glm::vec4 team2Color = (displayedScore2 > displayedScore1) ? glm::vec4(0.2f, 0.8f, 0.2f, 1.0f) : glm::vec4(1.0f);
    
    // Draw scoreboard panel
    uiRenderer->renderPanel(position, {420, 120}, glm::vec4(0.1f, 0.1f, 0.1f, 0.95f), "panel_bg");
    
    // Team 1
    uiRenderer->renderText(position + glm::vec2(20, 20), team1, team1Color, 1.2f);
    uiRenderer->renderText(position + glm::vec2(20, 50), std::to_string(displayedScore1) + "/" + std::to_string(displayedWickets1), team1Color, 1.1f);
    uiRenderer->renderText(position + glm::vec2(120, 50), "(" + std::to_string(static_cast<int>(displayedOvers1)) + "." + std::to_string(static_cast<int>((displayedOvers1 - static_cast<int>(displayedOvers1)) * 6)) + ")", glm::vec4(1.0f), 1.0f);
    
    // Team 2
    uiRenderer->renderText(position + glm::vec2(220, 20), team2, team2Color, 1.2f);
    uiRenderer->renderText(position + glm::vec2(220, 50), std::to_string(displayedScore2) + "/" + std::to_string(displayedWickets2), team2Color, 1.1f);
    uiRenderer->renderText(position + glm::vec2(320, 50), "(" + std::to_string(static_cast<int>(displayedOvers2)) + "." + std::to_string(static_cast<int>((displayedOvers2 - static_cast<int>(displayedOvers2)) * 6)) + ")", glm::vec4(1.0f), 1.0f);
    
    // Event overlay
    if (eventActive && eventAlpha > 0.0f) {
        glm::vec4 c = eventColor;
        c.a *= eventAlpha;
        uiRenderer->renderPanel(position + glm::vec2(0, 90), {420, 30}, c);
        uiRenderer->renderText(position + glm::vec2(20, 95), eventText, glm::vec4(1.0f, 1.0f, 1.0f, eventAlpha), 1.0f);
    }
} 