#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "Player.h"
#include "Team.h"
#include "MatchEngine.h"

// Forward declarations
class Shader;
class Model;
class Texture;

enum class CameraMode {
    BROADCAST,      // TV-style camera
    PLAYER_FOLLOW,  // Follow specific player
    BALL_FOLLOW,    // Follow the ball
    FIELD_VIEW,     // Wide field view
    BOWLER_VIEW,    // From bowler's perspective
    BATSMAN_VIEW,   // From batsman's perspective
    UMPIRE_VIEW,    // From umpire's perspective
    DRONE_VIEW      // Aerial view
};

enum class VisualQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA
};

struct PlayerModel {
    std::string playerId;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::string currentAnimation;
    float animationTime;
    bool isVisible;
    int teamId;
    std::string role; // batsman, bowler, fielder, wicket-keeper
};

struct BallModel {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float spin;
    float seam;
    bool isVisible;
    float bounceHeight;
    float trajectoryCurve;
};

struct FieldModel {
    glm::vec3 pitchPosition;
    glm::vec3 pitchSize;
    glm::vec3 boundarySize;
    std::vector<glm::vec3> fieldingPositions;
    std::vector<glm::vec3> boundaryRopes;
    std::vector<glm::vec3> sightScreens;
    glm::vec3 umpirePosition;
};

struct Camera {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    CameraMode mode;
    float transitionTime;
    glm::vec3 targetPosition;
    glm::vec3 targetTarget;
};

struct Lighting {
    glm::vec3 ambientColor;
    glm::vec3 directionalColor;
    glm::vec3 directionalDirection;
    float ambientIntensity;
    float directionalIntensity;
    bool shadowsEnabled;
    float shadowBias;
};

struct WeatherEffects {
    bool isRaining;
    float rainIntensity;
    bool isOvercast;
    float cloudCover;
    float windStrength;
    glm::vec3 windDirection;
    float fogDensity;
    glm::vec3 fogColor;
};

struct AnimationState {
    std::string name;
    float duration;
    float currentTime;
    bool isLooping;
    bool isComplete;
    std::function<void(float)> updateCallback;
};

class MatchVisualizer {
public:
    MatchVisualizer();
    ~MatchVisualizer();
    
    // Initialization
    bool initialize(int windowWidth, int windowHeight);
    void cleanup();
    
    // Setup
    void setupMatch(Team* team1, Team* team2, Venue* venue, MatchType matchType);
    void setVisualQuality(VisualQuality quality);
    void setCameraMode(CameraMode mode);
    
    // Rendering
    void render(float deltaTime);
    void renderField();
    void renderPlayers();
    void renderBall();
    void renderUI();
    void renderEffects();
    
    // Camera control
    void setCameraPosition(const glm::vec3& position);
    void setCameraTarget(const glm::vec3& target);
    void transitionCamera(const glm::vec3& newPosition, const glm::vec3& newTarget, float duration);
    void followPlayer(const std::string& playerId);
    void followBall();
    void resetCamera();
    
    // Player management
    void updatePlayerPosition(const std::string& playerId, const glm::vec3& position);
    void updatePlayerAnimation(const std::string& playerId, const std::string& animation);
    void setPlayerVisibility(const std::string& playerId, bool visible);
    void highlightPlayer(const std::string& playerId, const glm::vec3& color);
    
    // Ball physics
    void setBallPosition(const glm::vec3& position);
    void setBallVelocity(const glm::vec3& velocity);
    void setBallSpin(float spin);
    void setBallSeam(float seam);
    void animateBallTrajectory(const std::vector<glm::vec3>& trajectory, float duration);
    void showBallTrail(bool show);
    
    // Field management
    void setFieldingPositions(const std::map<std::string, FieldingPosition>& positions);
    void updatePitchConditions(const PitchConditions& conditions);
    void updateWeatherConditions(const WeatherConditions& conditions);
    
    // Effects
    void addParticleEffect(const glm::vec3& position, const std::string& effectType);
    void addScreenEffect(const std::string& effectType, float duration);
    void setSlowMotion(bool enabled, float speed = 0.5f);
    void setReplayMode(bool enabled);
    
    // Match events
    void onBallBowled(const BallEvent& event);
    void onBallHit(const BallEvent& event);
    void onWicket(const BallEvent& event);
    void onBoundary(const BallEvent& event);
    void onOverComplete(const Over& over);
    void onInningsComplete(const Innings& innings);
    
    // UI and HUD
    void showScoreboard(bool show);
    void showPlayerStats(bool show);
    void showFieldingPositions(bool show);
    void showBallTrajectory(bool show);
    void showReplayControls(bool show);
    
    // Performance
    void setFrameRate(int fps);
    void enableVSync(bool enabled);
    void setShadowQuality(int quality);
    void setTextureQuality(int quality);
    
    // Getters
    Camera getCamera() const { return camera; }
    const std::vector<PlayerModel>& getPlayerModels() const { return playerModels; }
    const BallModel& getBallModel() const { return ball; }
    const FieldModel& getFieldModel() const { return field; }
    
    // Callbacks
    void setBallEventCallback(std::function<void(const BallEvent&)> callback);
    void setCameraChangeCallback(std::function<void(CameraMode)> callback);
    void setReplayCallback(std::function<void()> callback);

private:
    // OpenGL and rendering
    GLuint framebuffer;
    GLuint renderTexture;
    GLuint depthBuffer;
    int viewportWidth, viewportHeight;
    
    // Shaders
    std::unique_ptr<Shader> fieldShader;
    std::unique_ptr<Shader> playerShader;
    std::unique_ptr<Shader> ballShader;
    std::unique_ptr<Shader> uiShader;
    std::unique_ptr<Shader> particleShader;
    
    // Models and textures
    std::unique_ptr<Model> playerModel;
    std::unique_ptr<Model> ballModel;
    std::unique_ptr<Model> fieldModel;
    std::unique_ptr<Model> stadiumModel;
    std::map<std::string, std::unique_ptr<Texture>> textures;
    
    // Game objects
    std::vector<PlayerModel> playerModels;
    BallModel ball;
    FieldModel field;
    Camera camera;
    Lighting lighting;
    WeatherEffects weather;
    
    // Match data
    Team* team1;
    Team* team2;
    Venue* venue;
    MatchType matchType;
    MatchEngine* matchEngine;
    
    // Animation
    std::map<std::string, AnimationState> animations;
    float globalTime;
    float slowMotionSpeed;
    bool slowMotionEnabled;
    
    // Effects
    std::vector<std::unique_ptr<class ParticleSystem>> particleSystems;
    std::vector<std::unique_ptr<class ScreenEffect>> screenEffects;
    bool ballTrailEnabled;
    std::vector<glm::vec3> ballTrail;
    
    // UI state
    bool scoreboardVisible;
    bool playerStatsVisible;
    bool fieldingPositionsVisible;
    bool ballTrajectoryVisible;
    bool replayControlsVisible;
    
    // Performance settings
    VisualQuality visualQuality;
    int targetFrameRate;
    bool vsyncEnabled;
    int shadowQuality;
    int textureQuality;
    
    // Callbacks
    std::function<void(const BallEvent&)> ballEventCallback;
    std::function<void(CameraMode)> cameraChangeCallback;
    std::function<void()> replayCallback;
    
    // Helper methods
    bool initializeShaders();
    bool loadModels();
    bool loadTextures();
    void setupLighting();
    void setupCamera();
    void setupField();
    void setupPlayers();
    
    void updateCamera(float deltaTime);
    void updateAnimations(float deltaTime);
    void updateParticles(float deltaTime);
    void updateEffects(float deltaTime);
    
    void renderPlayer(const PlayerModel& player);
    void renderBallTrail();
    void renderParticles();
    void renderShadows();
    void renderPostProcess();
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const;
    
    void calculateFieldingPositions();
    void updateWeatherEffects();
    void applyLightingEffects();
    
    // Physics helpers
    glm::vec3 calculateBallTrajectory(const glm::vec3& start, const glm::vec3& velocity, float time);
    float calculateBounceHeight(float initialHeight, float velocity, float time);
    glm::vec3 calculateSpinEffect(const glm::vec3& velocity, float spin, float time);
    
    // Animation helpers
    void playAnimation(const std::string& playerId, const std::string& animationName, bool loop = false);
    void stopAnimation(const std::string& playerId);
    void updatePlayerAnimation(const std::string& playerId, float deltaTime);
    
    // Camera helpers
    void updateCameraTransition(float deltaTime);
    void calculateCameraPosition(CameraMode mode);
    void smoothCameraMovement(const glm::vec3& targetPos, const glm::vec3& targetLook, float speed);
    
    // Utility
    glm::vec3 worldToScreen(const glm::vec3& worldPos) const;
    glm::vec3 screenToWorld(const glm::vec2& screenPos, float depth) const;
    float distanceToCamera(const glm::vec3& position) const;
    bool isInViewFrustum(const glm::vec3& position, float radius) const;
}; 