#include "MatchVisualizer.h"
#include <glad/glad.h>
#include "Shader.h"
#include "Model.h"
#include "ParticleSystem.h"
#include "ScreenEffect.h"
#include <iostream>
#include <algorithm>
#include <cmath>

MatchVisualizer::MatchVisualizer() 
    : framebuffer(0), renderTexture(0), depthBuffer(0)
    , viewportWidth(1280), viewportHeight(720)
    , team1(nullptr), team2(nullptr), venue(nullptr)
    , matchType(MatchType::T20), matchEngine(nullptr)
    , currentCameraMode(CameraMode::FOLLOW_BALL)
    , cameraShakeIntensity(0.0f), cameraShakeDuration(0.0f), cameraShakeTimer(0.0f)
    , shadowMap(0), shadowFBO(0)
    , rainTexture(0), cloudTexture(0)
    , scoreboardVisible(true), playerStatsVisible(false)
    , fieldingPositionsVisible(false), ballTrajectoryVisible(false)
    , replayControlsVisible(false)
    , targetFrameRate(60), vsyncEnabled(true), shadowQuality(2), textureQuality(2) {
    
    // Initialize camera settings
    cameraSettings.position = glm::vec3(0, 10, 20);
    cameraSettings.target = glm::vec3(0, 0, 0);
    cameraSettings.up = glm::vec3(0, 1, 0);
    cameraSettings.fov = 45.0f;
    cameraSettings.nearPlane = 0.1f;
    cameraSettings.farPlane = 1000.0f;
    cameraSettings.transitionSpeed = 2.0f;
    
    // Initialize lighting settings
    lightingSettings.ambientColor = glm::vec3(0.2f, 0.2f, 0.3f);
    lightingSettings.directionalColor = glm::vec3(1.0f, 0.95f, 0.8f);
    lightingSettings.directionalDirection = glm::vec3(0.5f, 1.0f, 0.3f);
    lightingSettings.ambientIntensity = 0.3f;
    lightingSettings.directionalIntensity = 0.7f;
    lightingSettings.shadowsEnabled = true;
    lightingSettings.shadowBias = 0.005f;
    
    // Initialize weather settings
    weatherSettings.condition = WeatherCondition::CLEAR;
    weatherSettings.rainIntensity = 0.0f;
    weatherSettings.windSpeed = 0.0f;
    weatherSettings.windDirection = 0.0f;
    weatherSettings.temperature = 25.0f;
    weatherSettings.humidity = 60.0f;
    weatherSettings.fogEnabled = false;
    weatherSettings.fogDensity = 0.0f;
}

MatchVisualizer::~MatchVisualizer() {
    cleanup();
}

bool MatchVisualizer::initialize(int windowWidth, int windowHeight) {
    viewportWidth = windowWidth;
    viewportHeight = windowHeight;
    
    // Setup OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize shaders
    if (!initializeShaders()) {
        std::cerr << "Failed to initialize shaders" << std::endl;
        return false;
    }
    
    // Load models and textures
    if (!loadModels()) {
        std::cerr << "Failed to load models" << std::endl;
        return false;
    }
    
    if (!loadTextures()) {
        std::cerr << "Failed to load textures" << std::endl;
        return false;
    }
    
    // Setup framebuffer for post-processing
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
    
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportWidth, viewportHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    setupLighting();
    setupCamera();
    
    std::cout << "MatchVisualizer initialized successfully" << std::endl;
    return true;
}

void MatchVisualizer::cleanup() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
    if (renderTexture) {
        glDeleteTextures(1, &renderTexture);
        renderTexture = 0;
    }
    if (depthBuffer) {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }
    
    fieldShader.reset();
    playerShader.reset();
    ballShader.reset();
    uiShader.reset();
    particleShader.reset();
    
    playerModel.reset();
    ballModel.reset();
    fieldModel.reset();
    stadiumModel.reset();
    textures.clear();
}

void MatchVisualizer::setupMatch(Team* t1, Team* t2, Venue* v, MatchType type) {
    team1 = t1;
    team2 = t2;
    venue = v;
    matchType = type;
    
    setupField();
    setupPlayers();
    
    // Setup initial camera position based on venue
    if (venue) {
        cameraSettings.position = glm::vec3(0, venue->GetStats().capacity * 0.1f, venue->GetStats().capacity * 0.2f);
        cameraSettings.target = glm::vec3(0, 0, 0);
    }
}

void MatchVisualizer::render(float deltaTime) {
    // Update systems
    updateCamera(deltaTime);
    updateLighting();
    updateWeather(deltaTime);
    updateParticles(deltaTime);
    updateScreenEffects(deltaTime);
    
    // Bind framebuffer for post-processing
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Calculate matrices
    viewMatrix = glm::lookAt(cameraSettings.position, cameraSettings.target, cameraSettings.up);
    projectionMatrix = glm::perspective(glm::radians(cameraSettings.fov), 
                                       static_cast<float>(viewportWidth) / viewportHeight, 
                                       cameraSettings.nearPlane, cameraSettings.farPlane);
    
    // Render scene
    renderField();
    renderPlayers();
    renderBall();
    
    // Render UI
    renderUI();
    
    // Unbind framebuffer and render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Apply post-processing effects
    renderScreenEffects();
}

void MatchVisualizer::renderField() {
    if (!fieldShader || !fieldModel) return;
    
    fieldShader->use();
    fieldShader->setMat4("view", viewMatrix);
    fieldShader->setMat4("projection", projectionMatrix);
    
    // Set lighting uniforms
    fieldShader->setVec3("lightPos", lightingSettings.directionalDirection);
    fieldShader->setVec3("viewPos", cameraSettings.position);
    fieldShader->setVec3("lightColor", lightingSettings.directionalColor);
    fieldShader->setFloat("ambientStrength", lightingSettings.ambientIntensity);
    fieldShader->setFloat("diffuseStrength", lightingSettings.directionalIntensity);
    
    // Set weather uniforms
    fieldShader->setBool("isRaining", weatherSettings.condition == WeatherCondition::RAIN);
    fieldShader->setFloat("rainIntensity", weatherSettings.rainIntensity);
    fieldShader->setFloat("fogDensity", weatherSettings.fogDensity);
    
    fieldModel->draw(*fieldShader);
}

void MatchVisualizer::renderPlayers() {
    if (!playerShader || !playerModel) return;
    
    playerShader->use();
    playerShader->setMat4("view", viewMatrix);
    playerShader->setMat4("projection", projectionMatrix);
    
    // Set lighting uniforms
    playerShader->setVec3("lightPos", lightingSettings.directionalDirection);
    playerShader->setVec3("viewPos", cameraSettings.position);
    playerShader->setVec3("lightColor", lightingSettings.directionalColor);
    playerShader->setFloat("ambientStrength", lightingSettings.ambientIntensity);
    playerShader->setFloat("diffuseStrength", lightingSettings.directionalIntensity);
    
    // Render players (simplified)
    playerModel->draw(*playerShader);
}

void MatchVisualizer::renderBall() {
    if (!ballShader || !ballModel) return;
    
    ballShader->use();
    ballShader->setMat4("view", viewMatrix);
    ballShader->setMat4("projection", projectionMatrix);
    
    // Set ball properties (simplified)
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    ballShader->setMat4("model", modelMatrix);
    
    // Set lighting uniforms
    ballShader->setVec3("lightPos", lightingSettings.directionalDirection);
    ballShader->setVec3("viewPos", cameraSettings.position);
    ballShader->setVec3("lightColor", lightingSettings.directionalColor);
    
    ballModel->draw(*ballShader);
}

void MatchVisualizer::renderUI() {
    // Simplified UI rendering
    if (scoreboardVisible) {
        // Render scoreboard
    }
    if (playerStatsVisible) {
        // Render player stats
    }
    if (fieldingPositionsVisible) {
        // Render fielding positions
    }
    if (ballTrajectoryVisible) {
        // Render ball trajectory
    }
    if (replayControlsVisible) {
        // Render replay controls
    }
}

void MatchVisualizer::setCameraMode(CameraMode mode) {
    if (currentCameraMode == mode) return;
    
    currentCameraMode = mode;
    cameraShakeTimer = 0.0f;
    
    // Calculate new camera position based on mode
    calculateCameraPosition(mode);
}

void MatchVisualizer::updateBallPosition(const glm::vec3& position) {
    // Simplified ball position update
}

void MatchVisualizer::updateBallTrajectory(const std::vector<glm::vec3>& trajectory) {
    // Simplified trajectory update
}

void MatchVisualizer::onBallBowled(const BallEvent& event) {
    // Simplified ball bowled event
}

void MatchVisualizer::onBallHit(const BallEvent& event) {
    // Simplified ball hit event
}

void MatchVisualizer::onWicket(const BallEvent& event) {
    // Simplified wicket event
}

void MatchVisualizer::onBoundary(const BallEvent& event) {
    // Simplified boundary event
}

void MatchVisualizer::onOverComplete(const Over& over) {
    // Simplified over complete event
}

void MatchVisualizer::onInningsComplete(const Innings& innings) {
    // Simplified innings complete event
}

void MatchVisualizer::showScoreboard(bool show) {
    scoreboardVisible = show;
}

void MatchVisualizer::showPlayerStats(bool show) {
    playerStatsVisible = show;
}

void MatchVisualizer::showFieldingPositions(bool show) {
    fieldingPositionsVisible = show;
}

void MatchVisualizer::showBallTrajectory(bool show) {
    ballTrajectoryVisible = show;
}

void MatchVisualizer::showReplayControls(bool show) {
    replayControlsVisible = show;
}

void MatchVisualizer::setFrameRate(int fps) {
    targetFrameRate = fps;
}

void MatchVisualizer::enableVSync(bool enabled) {
    vsyncEnabled = enabled;
}

void MatchVisualizer::setShadowQuality(int quality) {
    shadowQuality = quality;
}

void MatchVisualizer::setTextureQuality(int quality) {
    textureQuality = quality;
}

void MatchVisualizer::setCameraChangedCallback(std::function<void(CameraMode)> callback) {
    cameraChangedCallback = callback;
}

void MatchVisualizer::setPlayerClickedCallback(std::function<void(const std::string&)> callback) {
    playerClickedCallback = callback;
}

void MatchVisualizer::setWeather(WeatherCondition condition) {
    weatherSettings.condition = condition;
}

void MatchVisualizer::setTimeOfDay(TimeOfDay time) {
    // Simplified time of day setting
}

void MatchVisualizer::setCameraPosition(const glm::vec3& position) {
    cameraSettings.position = position;
}

void MatchVisualizer::setCameraTarget(const glm::vec3& target) {
    cameraSettings.target = target;
}

void MatchVisualizer::setCameraFOV(float fov) {
    cameraSettings.fov = fov;
}

void MatchVisualizer::setCameraTransitionSpeed(float speed) {
    cameraSettings.transitionSpeed = speed;
}

void MatchVisualizer::shakeCamera(float intensity, float duration) {
    cameraShakeIntensity = intensity;
    cameraShakeDuration = duration;
    cameraShakeTimer = duration;
}

void MatchVisualizer::updatePlayerPosition(const std::string& playerName, const glm::vec3& position) {
    // Simplified player position update
}

void MatchVisualizer::updatePlayerAnimation(const std::string& playerName, const std::string& animation) {
    // Simplified player animation update
}

void MatchVisualizer::addParticleEffect(const glm::vec3& position, const std::string& effectType) {
    // Simplified particle effect
}

void MatchVisualizer::addScreenEffect(const std::string& effectType, float duration) {
    // Simplified screen effect
}

void MatchVisualizer::setLighting(const LightingSettings& settings) {
    lightingSettings = settings;
}

void MatchVisualizer::updateCamera(float deltaTime) {
    // Simplified camera update
    if (cameraShakeTimer > 0) {
        cameraShakeTimer -= deltaTime;
        applyCameraShake();
    }
}

void MatchVisualizer::updateLighting() {
    // Simplified lighting update
}

void MatchVisualizer::updateWeather(float deltaTime) {
    // Simplified weather update
}

void MatchVisualizer::updateParticles(float deltaTime) {
    // Simplified particle update
}

void MatchVisualizer::updateScreenEffects(float deltaTime) {
    // Simplified screen effects update
}

void MatchVisualizer::calculateCameraPosition(CameraMode mode) {
    // Simplified camera position calculation
    switch (mode) {
        case CameraMode::FOLLOW_BALL:
            cameraSettings.position = glm::vec3(0, 15, 25);
            cameraSettings.target = glm::vec3(0, 0, 0);
            break;
        case CameraMode::FOLLOW_BATSMAN:
            cameraSettings.position = glm::vec3(0, 2, 15);
            cameraSettings.target = glm::vec3(0, 1, 0);
            break;
        case CameraMode::FOLLOW_BOWLER:
            cameraSettings.position = glm::vec3(0, 2, -15);
            cameraSettings.target = glm::vec3(0, 1, 0);
            break;
        case CameraMode::BIRD_EYE:
            cameraSettings.position = glm::vec3(0, 50, 0);
            cameraSettings.target = glm::vec3(0, 0, 0);
            break;
        default:
            cameraSettings.position = glm::vec3(0, 10, 20);
            cameraSettings.target = glm::vec3(0, 0, 0);
            break;
    }
}

void MatchVisualizer::applyCameraShake() {
    // Simplified camera shake
}

void MatchVisualizer::setupField() {
    // Simplified field setup
}

void MatchVisualizer::setupPlayers() {
    // Simplified players setup
}

void MatchVisualizer::setupLighting() {
    // Simplified lighting setup
}

void MatchVisualizer::setupCamera() {
    // Simplified camera setup
}

bool MatchVisualizer::initializeShaders() {
    // Simplified shader initialization
    fieldShader = std::make_unique<Shader>();
    playerShader = std::make_unique<Shader>();
    ballShader = std::make_unique<Shader>();
    uiShader = std::make_unique<Shader>();
    particleShader = std::make_unique<Shader>();
    
    return true;
}

bool MatchVisualizer::loadModels() {
    // Simplified model loading
    playerModel = std::make_unique<Model>();
    ballModel = std::make_unique<Model>();
    fieldModel = std::make_unique<Model>();
    stadiumModel = std::make_unique<Model>();
    
    return true;
}

bool MatchVisualizer::loadTextures() {
    // Simplified texture loading
    return true;
}

void MatchVisualizer::setupShaders() {
    // Simplified shader setup
}

void MatchVisualizer::setupModels() {
    // Simplified model setup
}

void MatchVisualizer::setupTextures() {
    // Simplified texture setup
}

void MatchVisualizer::setupFramebuffers() {
    // Simplified framebuffer setup
}

void MatchVisualizer::calculateMatrices() {
    // Simplified matrix calculation
    viewMatrix = glm::lookAt(cameraSettings.position, cameraSettings.target, cameraSettings.up);
    projectionMatrix = glm::perspective(glm::radians(cameraSettings.fov), 
                                       static_cast<float>(viewportWidth) / viewportHeight, 
                                       cameraSettings.nearPlane, cameraSettings.farPlane);
}

void MatchVisualizer::generateShadowMap() {
    // Simplified shadow map generation
}

void MatchVisualizer::renderToShadowMap() {
    // Simplified shadow map rendering
}

void MatchVisualizer::renderFieldGeometry() {
    // Simplified field geometry rendering
}

void MatchVisualizer::renderPlayerGeometry() {
    // Simplified player geometry rendering
}

void MatchVisualizer::renderBallGeometry() {
    // Simplified ball geometry rendering
}

void MatchVisualizer::renderWeatherEffects() {
    // Simplified weather effects rendering
}

void MatchVisualizer::renderParticleEffects() {
    // Simplified particle effects rendering
}

void MatchVisualizer::renderScreenEffects() {
    // Simplified screen effects rendering
}

void MatchVisualizer::renderUIElements() {
    // Simplified UI elements rendering
} 