#include "MainMenuManager.h"
#include "UIRenderer.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>

MainMenuManager::MainMenuManager(UIRenderer* renderer)
    : uiRenderer(renderer), currentState(MenuState::MAIN_MENU), previousState(MenuState::MAIN_MENU),
      globalTime(0.0f), backgroundOffset(0.0f), carouselRotation(0.0f),
      backgroundAnimating(true), carouselVisible(true) {
}

MainMenuManager::~MainMenuManager() {
    cleanup();
}

bool MainMenuManager::initialize() {
    setupMainMenu();
    return true;
}

void MainMenuManager::cleanup() {
    buttons.clear();
    teamLogos.clear();
}

void MainMenuManager::update(float deltaTime) {
    globalTime += deltaTime;
    
    // Update background animation
    if (backgroundAnimating) {
        backgroundOffset += deltaTime * 20.0f;
        if (backgroundOffset > 800.0f) backgroundOffset = 0.0f;
    }
    
    // Update carousel
    if (carouselVisible) {
        updateCarousel(deltaTime);
    }
    
    // Update button animations
    updateButtonAnimations(deltaTime);
}

void MainMenuManager::render() {
    if (!uiRenderer) {
        return;
    }
    
    // Render animated background with solid colors
    float time = static_cast<float>(SDL_GetTicks()) / 1000.0f;
    float offset = sin(time * 0.5f) * 50.0f;
    
    // Render background panels
    for (int i = 0; i < 5; ++i) {
        float x = 200.0f * i + offset;
        if (x >= -200.0f && x <= 1280.0f) {
            uiRenderer->renderPanel(glm::vec2(x, 0), glm::vec2(200, 600), 
                                  glm::vec4(0.1f, 0.3f, 0.1f, 0.3f), "");
        }
    }
    
    // Render team carousel
    std::vector<glm::vec4> colors = {
        glm::vec4(0.2f, 0.6f, 1.0f, 1.0f),   // Blue
        glm::vec4(1.0f, 0.8f, 0.0f, 1.0f),   // Yellow
        glm::vec4(1.0f, 0.2f, 0.2f, 1.0f),   // Red
        glm::vec4(0.8f, 0.2f, 0.8f, 1.0f),   // Purple
        glm::vec4(0.2f, 0.8f, 0.2f, 1.0f)    // Green
    };
    
    for (size_t i = 0; i < colors.size(); ++i) {
        float angle = time * 0.5f + i * 2.0f * M_PI / colors.size();
        float radius = 100.0f;
        float centerX = 400.0f;
        float centerY = 300.0f;
        
        float x = centerX + cos(angle) * radius;
        float y = centerY + sin(angle) * radius;
        
        uiRenderer->renderPanel(glm::vec2(x, y), glm::vec2(64, 64), colors[i], "");
    }
    
    // Render game logo
    uiRenderer->renderPanel(glm::vec2(350, 50), glm::vec2(100, 100), 
                          glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), "");
    
    // Render state-specific content
    renderStateSpecificContent();
}

void MainMenuManager::setState(MenuState state) {
    previousState = currentState;
    currentState = state;
    
    if (stateChangeCallback) {
        stateChangeCallback(state);
    }
}

void MainMenuManager::handleMouseMove(float x, float y) {
    for (auto& button : buttons) {
        button.isHovered = (x >= button.position.x && x <= button.position.x + button.size.x &&
                           y >= button.position.y && y <= button.position.y + button.size.y);
    }
}

void MainMenuManager::handleMouseClick(float x, float y) {
    MenuButton* clickedButton = findButtonAt(x, y);
    if (clickedButton && clickedButton->onClick) {
        clickedButton->onClick();
    }
}

void MainMenuManager::handleKeyPress(int key) {
    switch (key) {
        case 27: // ESC
            if (currentState != MenuState::MAIN_MENU) {
                setState(MenuState::MAIN_MENU);
            }
            break;
    }
}

void MainMenuManager::setupMainMenu() {
    buttons.clear();
    
    // Create simple buttons
    MenuButton btn1 = {"Team Management", {300, 200}, {200, 50}, {0.2f, 0.6f, 1.0f, 1.0f}, {0.3f, 0.7f, 1.0f, 1.0f}, false, false, true, [this]() { setState(MenuState::TEAM_MANAGEMENT); }, 0.0f};
    MenuButton btn2 = {"Match Setup", {300, 270}, {200, 50}, {0.2f, 0.8f, 0.2f, 1.0f}, {0.3f, 0.9f, 0.3f, 1.0f}, false, false, true, [this]() { setState(MenuState::MATCH_SETUP); }, 0.0f};
    MenuButton btn3 = {"Exit", {300, 340}, {200, 50}, {0.8f, 0.2f, 0.2f, 1.0f}, {0.9f, 0.3f, 0.3f, 1.0f}, false, false, true, [this]() { setState(MenuState::EXIT); }, 0.0f};
    
    buttons.push_back(btn1);
    buttons.push_back(btn2);
    buttons.push_back(btn3);
}

void MainMenuManager::setupTeamManagement() { buttons.clear(); }
void MainMenuManager::setupMatchSetup() { buttons.clear(); }
void MainMenuManager::setupAuctionRoom() { buttons.clear(); }
void MainMenuManager::setupCareerMode() { buttons.clear(); }
void MainMenuManager::setupSettings() { buttons.clear(); }

void MainMenuManager::renderMainMenu() {
    // Render title
    uiRenderer->renderText(glm::vec2(250, 100), "CRICKET MANAGER", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
    uiRenderer->renderText(glm::vec2(280, 140), "Professional Cricket Management Game", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), 1.0f);
}

void MainMenuManager::renderTeamManagement() {
    uiRenderer->renderText(glm::vec2(250, 100), "TEAM MANAGEMENT", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
}

void MainMenuManager::renderMatchSetup() {
    uiRenderer->renderText(glm::vec2(250, 100), "MATCH SETUP", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
}

void MainMenuManager::renderAuctionRoom() {
    uiRenderer->renderText(glm::vec2(250, 100), "AUCTION ROOM", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
}

void MainMenuManager::renderCareerMode() {
    uiRenderer->renderText(glm::vec2(250, 100), "CAREER MODE", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
}

void MainMenuManager::renderSettings() {
    uiRenderer->renderText(glm::vec2(250, 100), "SETTINGS", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
}

void MainMenuManager::renderStateSpecificContent() {
    switch (currentState) {
        case MenuState::MAIN_MENU:
            renderMainMenu();
            break;
        case MenuState::TEAM_MANAGEMENT:
            renderTeamManagement();
            break;
        case MenuState::MATCH_SETUP:
            renderMatchSetup();
            break;
        case MenuState::AUCTION_ROOM:
            renderAuctionRoom();
            break;
        case MenuState::CAREER_MODE:
            renderCareerMode();
            break;
        case MenuState::SETTINGS:
            renderSettings();
            break;
        default:
            break;
    }
    
    renderAnimatedButtons();
}

void MainMenuManager::renderAnimatedButtons() {
    for (auto& button : buttons) {
        if (button.isVisible) {
            glm::vec4 buttonColor = button.isHovered ? 
                glm::vec4(button.color.r * 1.2f, button.color.g * 1.2f, button.color.b * 1.2f, button.color.a) :
                button.color;
            
            uiRenderer->renderPanel(button.position, button.size, buttonColor, "");
            uiRenderer->renderText(glm::vec2(button.position.x + button.size.x * 0.5f, 
                                           button.position.y + button.size.y * 0.5f), 
                                 button.text, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }
}

MenuButton* MainMenuManager::findButtonAt(float x, float y) {
    for (auto& button : buttons) {
        if (x >= button.position.x && x <= button.position.x + button.size.x &&
            y >= button.position.y && y <= button.position.y + button.size.y) {
            return &button;
        }
    }
    return nullptr;
}

void MainMenuManager::updateButtonAnimations(float deltaTime) {
    for (auto& button : buttons) {
        if (button.animationTime < 0.3f) {
            button.animationTime += deltaTime;
        }
    }
}

void MainMenuManager::updateCarousel(float deltaTime) {
    carouselRotation += deltaTime * 30.0f;
    if (carouselRotation > 360.0f) carouselRotation -= 360.0f;
}

void MainMenuManager::setStateChangeCallback(std::function<void(MenuState)> callback) {
    stateChangeCallback = callback;
}

void MainMenuManager::setAuctionStartCallback(std::function<void()> callback) {
    auctionStartCallback = callback;
}

void MainMenuManager::setMatchStartCallback(std::function<void()> callback) {
    matchStartCallback = callback;
}

void MainMenuManager::triggerButtonAnimation(const std::string& buttonText) {
    for (auto& button : buttons) {
        if (button.text == buttonText) {
            button.animationTime = 0.0f;
            break;
        }
    }
}

void MainMenuManager::showTeamCarousel(bool show) {
    carouselVisible = show;
}

void MainMenuManager::setBackgroundAnimation(bool enabled) {
    backgroundAnimating = enabled;
} 