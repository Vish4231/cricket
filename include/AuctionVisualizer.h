#pragma once

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <queue>
#include "Player.h"
#include "Team.h"
#include "AuctionManager.h"

// Forward declarations
class Shader;
class Model;
class Texture;

enum class AuctionCameraMode {
    HALL_OVERVIEW,      // Wide view of entire auction hall
    STAGE_FOCUS,        // Focus on auction stage
    TEAM_TABLE,         // Focus on specific team table
    PLAYER_DETAIL,      // Close-up of player model
    BIDDING_VIEW,       // Dynamic view during bidding
    CELEBRATION_VIEW    // Celebration after successful bid
};

enum class AuctionState {
    SETUP,
    PLAYER_PRESENTATION,
    BIDDING_ACTIVE,
    BID_WON,
    PLAYER_SOLD,
    BREAK,
    COMPLETE
};

// Animation states for team representatives and players
enum class RepAnimationState {
    Idle,
    Bidding,
    Applauding,
    Celebrating,
    Disappointed,
    Waving
};

enum class PlayerAnimationState {
    Idle,
    WalkingToStage,
    Waving,
    Sold,
    Unsold,
    Celebrating
};

struct TeamRepresentative {
    std::string teamName;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::string currentAnimation;
    float animationTime;
    bool isActive;
    bool isBidding;
    float bidAmount;
    glm::vec3 teamColor;
    std::string representativeName;
    int teamId;
    // Animation state machine
    RepAnimationState animState = RepAnimationState::Idle;
    RepAnimationState prevAnimState = RepAnimationState::Idle;
    float animBlend = 0.0f; // 0 = prev, 1 = current
    float animBlendTime = 0.0f;
    float animBlendDuration = 0.2f;
};

struct PlayerModel {
    std::string playerId;
    std::string playerName;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::string currentAnimation;
    float animationTime;
    bool isVisible;
    bool isHighlighted;
    float highlightIntensity;
    Player* playerData;
    std::vector<std::string> stats;
    float basePrice;
    float currentBid;
    std::string status; // "Available", "Bidding", "Sold", "Unsold"
    // Animation state machine
    PlayerAnimationState animState = PlayerAnimationState::Idle;
    PlayerAnimationState prevAnimState = PlayerAnimationState::Idle;
    float animBlend = 0.0f;
    float animBlendTime = 0.0f;
    float animBlendDuration = 0.2f;
};

struct AuctionHall {
    glm::vec3 stagePosition;
    glm::vec3 stageSize;
    std::vector<glm::vec3> teamTablePositions;
    std::vector<glm::vec3> audiencePositions;
    glm::vec3 auctioneerPosition;
    glm::vec3 playerDisplayPosition;
    glm::vec3 screenPosition;
    glm::vec3 lightingPositions[4];
    glm::vec3 hallSize;
};

struct BiddingEvent {
    std::string teamName;
    float bidAmount;
    float timestamp;
    bool isWinningBid;
    glm::vec3 teamColor;
    std::string representativeName;
};

struct AuctionUI {
    bool showPlayerStats;
    bool showBiddingHistory;
    bool showTeamBudgets;
    bool showAuctionProgress;
    bool showTimer;
    float uiScale;
    glm::vec2 screenSize;
};

class AuctionVisualizer {
public:
    AuctionVisualizer();
    ~AuctionVisualizer();
    
    // Initialization
    bool initialize(int windowWidth, int windowHeight);
    void cleanup();
    
    // Setup
    void setupAuction(const std::vector<Team*>& teams, const std::vector<Player*>& players);
    void setAuctionState(AuctionState state);
    void setCameraMode(AuctionCameraMode mode);
    
    // Rendering
    void render(float deltaTime);
    void renderHall();
    void renderTeams();
    void renderCurrentPlayer();
    void renderUI();
    void renderEffects();
    
    // Camera control
    void setCameraPosition(const glm::vec3& position);
    void setCameraTarget(const glm::vec3& target);
    void transitionCamera(const glm::vec3& newPosition, const glm::vec3& newTarget, float duration);
    void focusOnTeam(const std::string& teamName);
    void focusOnPlayer(const std::string& playerId);
    void resetCamera();
    
    // Team management
    void updateTeamPosition(const std::string& teamName, const glm::vec3& position);
    void updateTeamAnimation(const std::string& teamName, const std::string& animation);
    void setTeamBidding(const std::string& teamName, bool isBidding, float bidAmount = 0.0f);
    void highlightTeam(const std::string& teamName, const glm::vec3& color);
    
    // Player management
    void setCurrentPlayer(const std::string& playerId);
    void updatePlayerAnimation(const std::string& playerId, const std::string& animation);
    void setPlayerHighlight(const std::string& playerId, bool highlighted, float intensity = 1.0f);
    void showPlayerStats(const std::string& playerId, bool show);
    
    // Bidding events
    void onBidPlaced(const std::string& teamName, float bidAmount);
    void onBidWon(const std::string& teamName, float finalBid);
    void onPlayerSold(const std::string& playerId, const std::string& teamName, float price);
    void onPlayerUnsold(const std::string& playerId);
    
    // Effects
    void addParticleEffect(const glm::vec3& position, const std::string& effectType);
    void addScreenEffect(const std::string& effectType, float duration);
    void setSlowMotion(bool enabled, float speed = 0.5f);
    void addCelebrationEffect(const std::string& teamName);
    
    // UI and HUD
    void showPlayerStats(bool show);
    void showBiddingHistory(bool show);
    void showTeamBudgets(bool show);
    void showAuctionProgress(bool show);
    void showTimer(bool show);
    
    // Performance
    void setVisualQuality(int quality);
    void setFrameRate(int fps);
    void enableVSync(bool enabled);
    
    // Getters
    AuctionState getAuctionState() const { return auctionState; }
    const std::vector<TeamRepresentative>& getTeamRepresentatives() const { return teamReps; }
    const std::vector<PlayerModel>& getPlayerModels() const { return playerModels; }
    const AuctionHall& getAuctionHall() const { return hall; }
    
    // Callbacks
    void setBidPlacedCallback(std::function<void(const std::string&, float)> callback);
    void setPlayerSoldCallback(std::function<void(const std::string&, const std::string&, float)> callback);
    void setCameraChangeCallback(std::function<void(AuctionCameraMode)> callback);

    // Camera shake
    void triggerCameraShake(float intensity, float duration);

private:
    // OpenGL and rendering
    GLuint framebuffer;
    GLuint renderTexture;
    GLuint depthBuffer;
    int viewportWidth, viewportHeight;
    
    // Shaders
    std::unique_ptr<Shader> hallShader;
    std::unique_ptr<Shader> teamShader;
    std::unique_ptr<Shader> playerShader;
    std::unique_ptr<Shader> uiShader;
    std::unique_ptr<Shader> particleShader;
    
    // Models and textures
    std::unique_ptr<Model> hallModel;
    std::unique_ptr<Model> teamRepModel;
    std::unique_ptr<Model> playerModel;
    std::unique_ptr<Model> stageModel;
    std::unique_ptr<Model> screenModel;
    std::map<std::string, std::unique_ptr<Texture>> textures;
    
    // Auction objects
    std::vector<TeamRepresentative> teamReps;
    std::vector<PlayerModel> playerModels;
    PlayerModel* currentPlayer;
    AuctionHall hall;
    AuctionUI ui;
    
    // Camera
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    glm::vec3 cameraUp;
    float cameraFov;
    float cameraNear;
    float cameraFar;
    float cameraAspect;
    AuctionCameraMode cameraMode;
    float cameraTransitionTime;
    glm::vec3 targetPosition;
    glm::vec3 targetTarget;
    
    // Lighting
    glm::vec3 ambientColor;
    glm::vec3 directionalColor;
    glm::vec3 directionalDirection;
    float ambientIntensity;
    float directionalIntensity;
    bool shadowsEnabled;
    
    // Auction state
    AuctionState auctionState;
    std::queue<BiddingEvent> biddingHistory;
    float auctionTimer;
    float currentBid;
    std::string currentBidder;
    std::string winningBidder;
    
    // Animation
    float globalTime;
    float slowMotionSpeed;
    bool slowMotionEnabled;
    
    // Effects
    std::vector<std::unique_ptr<class ParticleSystem>> particleSystems;
    std::vector<std::unique_ptr<class ScreenEffect>> screenEffects;
    
    // Performance settings
    int visualQuality;
    int targetFrameRate;
    bool vsyncEnabled;
    
    // Callbacks
    std::function<void(const std::string&, float)> bidPlacedCallback;
    std::function<void(const std::string&, const std::string&, float)> playerSoldCallback;
    std::function<void(AuctionCameraMode)> cameraChangeCallback;
    
    // Helper methods
    bool initializeShaders();
    bool loadModels();
    bool loadTextures();
    void setupLighting();
    void setupCamera();
    void setupHall();
    void setupTeams();
    void setupPlayers();
    
    void updateCamera(float deltaTime);
    void updateAnimations(float deltaTime);
    void updateParticles(float deltaTime);
    void updateEffects(float deltaTime);
    
    void renderTeam(const TeamRepresentative& team);
    void renderPlayer(const PlayerModel& player);
    void renderBiddingHistory();
    void renderPlayerStats();
    void renderTeamBudgets();
    void renderAuctionProgress();
    void renderTimer();
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const;
    
    void calculateTeamPositions();
    void updateBiddingHistory();
    void applyLightingEffects();
    
    // Animation helpers
    void playTeamAnimation(const std::string& teamName, const std::string& animationName, bool loop = false);
    void stopTeamAnimation(const std::string& teamName);
    void updateTeamAnimation(const std::string& teamName, float deltaTime);
    
    // Camera helpers
    void updateCameraTransition(float deltaTime);
    void calculateCameraPosition(AuctionCameraMode mode);
    void smoothCameraMovement(const glm::vec3& targetPos, const glm::vec3& targetLook, float speed);
    
    // Utility
    glm::vec3 worldToScreen(const glm::vec3& worldPos) const;
    glm::vec3 screenToWorld(const glm::vec2& screenPos, float depth) const;
    float distanceToCamera(const glm::vec3& position) const;
    bool isInViewFrustum(const glm::vec3& position, float radius) const;
    
    // Bidding helpers
    void processBiddingEvent(const BiddingEvent& event);
    void updateBidDisplay();
    void triggerBiddingEffects(const std::string& teamName);

    // Camera transition state
    glm::vec3 cameraStartPos;
    glm::vec3 cameraStartTarget;
    glm::vec3 cameraEndPos;
    glm::vec3 cameraEndTarget;
    float cameraTransitionElapsed = 0.0f;
    float cameraTransitionDuration = 1.0f;
    bool cameraIsTransitioning = false;
    // Camera shake
    float cameraShakeIntensity = 0.0f;
    float cameraShakeDuration = 0.0f;
    float cameraShakeElapsed = 0.0f;
    glm::vec3 cameraShakeOffset = glm::vec3(0.0f);

    // Animation helpers
    void setTeamAnimationState(const std::string& teamName, RepAnimationState state);
    void setPlayerAnimationState(const std::string& playerId, PlayerAnimationState state);
    void updateTeamAnimationState(TeamRepresentative& team, float deltaTime);
    void updatePlayerAnimationState(PlayerModel& player, float deltaTime);
    
    // Utility functions
    glm::vec3 getTeamColor(const std::string& teamName);
    float calculateBasePrice(const Player* player);
    float randf(float min, float max);
}; 