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
#include "Player.h"
#include "Team.h"
#include "MatchEngine.h"

// Forward declarations
struct Model;
class Shader;
class Texture;
class ParticleSystem;
class ScreenEffect;

enum class CameraMode {
    FOLLOW_BALL,
    FOLLOW_BATSMAN,
    FOLLOW_BOWLER,
    FIELD_VIEW,
    BIRD_EYE,
    CLOSE_UP,
    REPLAY
};

enum class WeatherCondition {
    CLEAR,
    CLOUDY,
    RAIN,
    OVERCAST,
    WINDY
};

enum class TimeOfDay {
    MORNING,
    AFTERNOON,
    EVENING,
    NIGHT
};

struct CameraSettings {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float nearPlane;
    float farPlane;
    float transitionSpeed;
};

struct LightingSettings {
    glm::vec3 ambientColor;
    glm::vec3 directionalColor;
    glm::vec3 directionalDirection;
    float ambientIntensity;
    float directionalIntensity;
    bool shadowsEnabled;
    float shadowBias;
};

struct WeatherSettings {
    WeatherCondition condition;
    float rainIntensity;
    float windSpeed;
    float windDirection;
    float temperature;
    float humidity;
    bool fogEnabled;
    float fogDensity;
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
    void setWeather(WeatherCondition condition);
    void setTimeOfDay(TimeOfDay time);
    
    // Rendering
    void render(float deltaTime);
    void renderField();
    void renderPlayers();
    void renderBall();
    void renderUI();
    
    // Camera control
    void setCameraMode(CameraMode mode);
    void setCameraPosition(const glm::vec3& position);
    void setCameraTarget(const glm::vec3& target);
    void setCameraFOV(float fov);
    void setCameraTransitionSpeed(float speed);
    void shakeCamera(float intensity, float duration);
    
    // Player management
    void updatePlayerPosition(const std::string& playerName, const glm::vec3& position);
    void updatePlayerAnimation(const std::string& playerName, const std::string& animation);
    void updateBallPosition(const glm::vec3& position);
    void updateBallTrajectory(const std::vector<glm::vec3>& trajectory);
    
    // Effects
    void addParticleEffect(const glm::vec3& position, const std::string& effectType);
    void addScreenEffect(const std::string& effectType, float duration);
    void setLighting(const LightingSettings& settings);
    
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
    CameraMode getCurrentCameraMode() const { return currentCameraMode; }
    const CameraSettings& getCameraSettings() const { return cameraSettings; }
    const LightingSettings& getLightingSettings() const { return lightingSettings; }
    const WeatherSettings& getWeatherSettings() const { return weatherSettings; }
    
    // Callbacks
    void setCameraChangedCallback(std::function<void(CameraMode)> callback);
    void setPlayerClickedCallback(std::function<void(const std::string&)> callback);

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
    Team* team1;
    Team* team2;
    Venue* venue;
    MatchType matchType;
    MatchEngine* matchEngine;
    
    // Camera system
    CameraMode currentCameraMode;
    CameraSettings cameraSettings;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    float cameraShakeIntensity;
    float cameraShakeDuration;
    float cameraShakeTimer;
    
    // Lighting system
    LightingSettings lightingSettings;
    GLuint shadowMap;
    GLuint shadowFBO;
    
    // Weather system
    WeatherSettings weatherSettings;
    GLuint rainTexture;
    GLuint cloudTexture;
    
    // UI state
    bool scoreboardVisible;
    bool playerStatsVisible;
    bool fieldingPositionsVisible;
    bool ballTrajectoryVisible;
    bool replayControlsVisible;
    
    // Performance settings
    int targetFrameRate;
    bool vsyncEnabled;
    int shadowQuality;
    int textureQuality;
    
    // Callbacks
    std::function<void(CameraMode)> cameraChangedCallback;
    std::function<void(const std::string&)> playerClickedCallback;
    
    // Helper methods
    bool initializeShaders();
    bool loadModels();
    bool loadTextures();
    void setupLighting();
    void setupCamera();
    void setupField();
    void setupPlayers();
    
    void updateCamera(float deltaTime);
    void updateLighting();
    void updateWeather(float deltaTime);
    void updateParticles(float deltaTime);
    void updateScreenEffects(float deltaTime);
    void renderFieldGeometry();
    void renderPlayerGeometry();
    void renderBallGeometry();
    void renderWeatherEffects();
    void renderParticleEffects();
    void renderScreenEffects();
    void renderUIElements();
    void setupShaders();
    void setupModels();
    void setupTextures();
    void setupFramebuffers();
    void calculateMatrices();
    void applyCameraShake();
    void generateShadowMap();
    void renderToShadowMap();
    void calculateCameraPosition(CameraMode mode);
}; 