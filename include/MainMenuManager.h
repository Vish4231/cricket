#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

class UIRenderer;
class AnimatedScoreboard;
class AuctionVisualizer;

enum class MenuState {
    MAIN_MENU,
    TEAM_MANAGEMENT,
    MATCH_SETUP,
    AUCTION_ROOM,
    CAREER_MODE,
    SETTINGS,
    EXIT
};

struct MenuButton {
    std::string text;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
    glm::vec4 hoverColor;
    bool isHovered;
    bool isClicked;
    bool isVisible;
    std::function<void()> onClick;
    float animationTime;
};

struct TeamLogo {
    std::string teamName;
    std::string textureName;
    glm::vec2 position;
    float rotation;
    float scale;
    bool isVisible;
};

class MainMenuManager {
public:
    MainMenuManager(UIRenderer* renderer);
    ~MainMenuManager();
    
    bool initialize();
    void cleanup();
    
    // Main menu functions
    void update(float deltaTime);
    void render();
    
    // Menu state management
    void setState(MenuState state);
    MenuState getState() const { return currentState; }
    
    // Interactive elements
    void handleMouseMove(float x, float y);
    void handleMouseClick(float x, float y);
    void handleKeyPress(int key);
    
    // Callbacks
    void setStateChangeCallback(std::function<void(MenuState)> callback);
    void setAuctionStartCallback(std::function<void()> callback);
    void setMatchStartCallback(std::function<void()> callback);
    
    // Animation controls
    void triggerButtonAnimation(const std::string& buttonText);
    void showTeamCarousel(bool show);
    void setBackgroundAnimation(bool enabled);

private:
    UIRenderer* uiRenderer;
    MenuState currentState;
    MenuState previousState;
    
    // UI Elements
    std::vector<MenuButton> buttons;
    std::vector<TeamLogo> teamLogos;
    
    // Animation state
    float globalTime;
    float backgroundOffset;
    float carouselRotation;
    bool backgroundAnimating;
    bool carouselVisible;
    
    // Callbacks
    std::function<void(MenuState)> stateChangeCallback;
    std::function<void()> auctionStartCallback;
    std::function<void()> matchStartCallback;
    
    // Helper functions
    void setupMainMenu();
    void setupTeamManagement();
    void setupMatchSetup();
    void setupAuctionRoom();
    void setupCareerMode();
    void setupSettings();
    
    void renderMainMenu();
    void renderTeamManagement();
    void renderMatchSetup();
    void renderAuctionRoom();
    void renderCareerMode();
    void renderSettings();
    
    void renderBackground();
    void renderTeamCarousel();
    void renderAnimatedButtons();
    void renderLogo();
    void renderStateSpecificContent();
    
    MenuButton* findButtonAt(float x, float y);
    void updateButtonAnimations(float deltaTime);
    void updateCarousel(float deltaTime);
    MenuButton createButton(const std::string& text, const glm::vec2& position, const glm::vec2& size,
                           const glm::vec4& color, const glm::vec4& hoverColor, std::function<void()> onClick);
}; 