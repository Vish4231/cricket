#include "MatchVisualizer.h"
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
    , globalTime(0.0f), slowMotionSpeed(1.0f), slowMotionEnabled(false)
    , ballTrailEnabled(false)
    , scoreboardVisible(true), playerStatsVisible(false)
    , fieldingPositionsVisible(false), ballTrajectoryVisible(false)
    , replayControlsVisible(false)
    , visualQuality(VisualQuality::HIGH), targetFrameRate(60)
    , vsyncEnabled(true), shadowQuality(2), textureQuality(2) {
    
    // Initialize camera
    camera.position = glm::vec3(0, 10, 20);
    camera.target = glm::vec3(0, 0, 0);
    camera.up = glm::vec3(0, 1, 0);
    camera.fov = 45.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 1000.0f;
    camera.aspectRatio = 16.0f / 9.0f;
    camera.mode = CameraMode::BROADCAST;
    camera.transitionTime = 0.0f;
    
    // Initialize lighting
    lighting.ambientColor = glm::vec3(0.2f, 0.2f, 0.3f);
    lighting.directionalColor = glm::vec3(1.0f, 0.95f, 0.8f);
    lighting.directionalDirection = glm::vec3(0.5f, 1.0f, 0.3f);
    lighting.ambientIntensity = 0.3f;
    lighting.directionalIntensity = 0.7f;
    lighting.shadowsEnabled = true;
    lighting.shadowBias = 0.005f;
    
    // Initialize weather
    weather.isRaining = false;
    weather.rainIntensity = 0.0f;
    weather.isOvercast = false;
    weather.cloudCover = 0.0f;
    weather.windStrength = 0.0f;
    weather.windDirection = glm::vec3(1, 0, 0);
    weather.fogDensity = 0.0f;
    weather.fogColor = glm::vec3(0.7f, 0.7f, 0.8f);
    
    // Initialize ball
    ball.position = glm::vec3(0, 0, 0);
    ball.velocity = glm::vec3(0, 0, 0);
    ball.acceleration = glm::vec3(0, -9.81f, 0);
    ball.spin = 0.0f;
    ball.seam = 0.0f;
    ball.isVisible = false;
    ball.bounceHeight = 0.0f;
    ball.trajectoryCurve = 0.0f;
}

MatchVisualizer::~MatchVisualizer() {
    cleanup();
}

bool MatchVisualizer::initialize(int windowWidth, int windowHeight) {
    viewportWidth = windowWidth;
    viewportHeight = windowHeight;
    camera.aspectRatio = static_cast<float>(windowWidth) / windowHeight;
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }
    
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
    
    // Initialize particle systems
    particleSystems.push_back(std::make_unique<ParticleSystem>());
    particleSystems[0]->initialize(500);
    
    // Initialize screen effects
    screenEffects.push_back(std::make_unique<ScreenEffect>());
    screenEffects[0]->initialize(viewportWidth, viewportHeight);
    
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
    
    particleSystems.clear();
    screenEffects.clear();
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
        camera.position = glm::vec3(0, venue->getCapacity() * 0.1f, venue->getCapacity() * 0.2f);
        camera.target = glm::vec3(0, 0, 0);
    }
}

void MatchVisualizer::render(float deltaTime) {
    // Apply slow motion
    if (slowMotionEnabled) {
        deltaTime *= slowMotionSpeed;
    }
    
    globalTime += deltaTime;
    
    // Update systems
    updateCamera(deltaTime);
    updateAnimations(deltaTime);
    updateParticles(deltaTime);
    updateEffects(deltaTime);
    
    // Begin rendering to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Get matrices
    glm::mat4 viewMatrix = getViewMatrix();
    glm::mat4 projectionMatrix = getProjectionMatrix();
    
    // Render scene
    renderField();
    renderPlayers();
    renderBall();
    renderParticles();
    
    // Render UI elements
    if (scoreboardVisible) renderUI();
    
    // End framebuffer rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Apply post-processing effects
    screenEffects[0]->beginRender();
    screenEffects[0]->renderEffects();
    screenEffects[0]->endRender();
}

void MatchVisualizer::renderField() {
    if (!fieldShader || !fieldModel) return;
    
    fieldShader->use();
    
    // Set matrices
    glm::mat4 modelMatrix = getModelMatrix(field.pitchPosition, glm::vec3(0), field.pitchSize);
    fieldShader->setMat4("model", modelMatrix);
    fieldShader->setMat4("view", getViewMatrix());
    fieldShader->setMat4("projection", getProjectionMatrix());
    
    // Set lighting
    fieldShader->setVec3("lightPos", lighting.directionalDirection);
    fieldShader->setVec3("viewPos", camera.position);
    fieldShader->setVec3("lightColor", lighting.directionalColor);
    fieldShader->setFloat("ambientStrength", lighting.ambientIntensity);
    fieldShader->setFloat("diffuseStrength", lighting.directionalIntensity);
    
    // Set weather effects
    fieldShader->setBool("isRaining", weather.isRaining);
    fieldShader->setFloat("rainIntensity", weather.rainIntensity);
    fieldShader->setFloat("fogDensity", weather.fogDensity);
    fieldShader->setVec3("fogColor", weather.fogColor);
    
    // Render field
    fieldModel->draw(*fieldShader);
    
    // Render stadium if available
    if (stadiumModel) {
        glm::mat4 stadiumMatrix = getModelMatrix(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(1));
        fieldShader->setMat4("model", stadiumMatrix);
        stadiumModel->draw(*fieldShader);
    }
    
    fieldShader->unuse();
}

void MatchVisualizer::renderPlayers() {
    if (!playerShader || !playerModel) return;
    
    playerShader->use();
    
    // Set matrices
    playerShader->setMat4("view", getViewMatrix());
    playerShader->setMat4("projection", getProjectionMatrix());
    
    // Set lighting
    playerShader->setVec3("lightPos", lighting.directionalDirection);
    playerShader->setVec3("viewPos", camera.position);
    playerShader->setVec3("lightColor", lighting.directionalColor);
    playerShader->setFloat("ambientStrength", lighting.ambientIntensity);
    playerShader->setFloat("diffuseStrength", lighting.directionalIntensity);
    
    // Render each player
    for (const auto& player : playerModels) {
        if (!player.isVisible) continue;
        
        glm::mat4 modelMatrix = getModelMatrix(player.position, player.rotation, player.scale);
        playerShader->setMat4("model", modelMatrix);
        
        // Set team colors
        glm::vec3 teamColor = (player.teamId == 1) ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
        playerShader->setVec3("teamColor", teamColor);
        
        playerModel->draw(*playerShader);
    }
    
    playerShader->unuse();
}

void MatchVisualizer::renderBall() {
    if (!ballShader || !ballModel || !ball.isVisible) return;
    
    ballShader->use();
    
    // Set matrices
    glm::mat4 modelMatrix = getModelMatrix(ball.position, glm::vec3(0), glm::vec3(0.1f));
    ballShader->setMat4("model", modelMatrix);
    ballShader->setMat4("view", getViewMatrix());
    ballShader->setMat4("projection", getProjectionMatrix());
    
    // Set ball properties
    ballShader->setFloat("spin", ball.spin);
    ballShader->setFloat("seam", ball.seam);
    ballShader->setFloat("bounceHeight", ball.bounceHeight);
    ballShader->setFloat("trajectoryCurve", ball.trajectoryCurve);
    
    // Set lighting
    ballShader->setVec3("lightPos", lighting.directionalDirection);
    ballShader->setVec3("viewPos", camera.position);
    ballShader->setVec3("lightColor", lighting.directionalColor);
    
    ballModel->draw(*ballShader);
    
    // Render ball trail if enabled
    if (ballTrailEnabled && ballTrail.size() > 1) {
        renderBallTrail();
    }
    
    ballShader->unuse();
}

void MatchVisualizer::renderBallTrail() {
    if (!particleShader || ballTrail.size() < 2) return;
    
    particleShader->use();
    
    particleShader->setMat4("view", getViewMatrix());
    particleShader->setMat4("projection", getProjectionMatrix());
    
    // Render trail as line segments
    for (size_t i = 1; i < ballTrail.size(); ++i) {
        glm::vec3 start = ballTrail[i-1];
        glm::vec3 end = ballTrail[i];
        
        // Create line segment
        std::vector<glm::vec3> lineVertices = {start, end};
        
        // Set color based on trail age
        float alpha = static_cast<float>(i) / ballTrail.size();
        glm::vec4 color(1.0f, 1.0f, 1.0f, alpha * 0.5f);
        particleShader->setVec4("trailColor", color);
        
        // Render line (simplified - would need proper line rendering)
        // This is a placeholder for actual line rendering
    }
    
    particleShader->unuse();
}

void MatchVisualizer::renderParticles() {
    if (particleSystems.empty()) return;
    
    glm::mat4 viewMatrix = getViewMatrix();
    glm::mat4 projectionMatrix = getProjectionMatrix();
    
    for (auto& particleSystem : particleSystems) {
        if (particleSystem->isActive()) {
            particleSystem->render(*particleShader, viewMatrix, projectionMatrix);
        }
    }
}

void MatchVisualizer::renderUI() {
    // This would render scoreboard, player stats, etc.
    // Implementation depends on the UI system being used
    // For now, this is a placeholder
}

void MatchVisualizer::setCameraMode(CameraMode mode) {
    if (camera.mode == mode) return;
    
    camera.mode = mode;
    camera.transitionTime = 0.0f;
    
    // Calculate new camera position based on mode
    calculateCameraPosition(mode);
    
    if (cameraChangeCallback) {
        cameraChangeCallback(mode);
    }
}

void MatchVisualizer::transitionCamera(const glm::vec3& newPosition, const glm::vec3& newTarget, float duration) {
    camera.targetPosition = newPosition;
    camera.targetTarget = newTarget;
    camera.transitionTime = duration;
}

void MatchVisualizer::followPlayer(const std::string& playerId) {
    auto it = std::find_if(playerModels.begin(), playerModels.end(),
                          [&](const PlayerModel& p) { return p.playerId == playerId; });
    
    if (it != playerModels.end()) {
        setCameraMode(CameraMode::PLAYER_FOLLOW);
        // Store player ID for following (would need to add to camera struct)
    }
}

void MatchVisualizer::followBall() {
    setCameraMode(CameraMode::BALL_FOLLOW);
}

void MatchVisualizer::updatePlayerPosition(const std::string& playerId, const glm::vec3& position) {
    auto it = std::find_if(playerModels.begin(), playerModels.end(),
                          [&](const PlayerModel& p) { return p.playerId == playerId; });
    
    if (it != playerModels.end()) {
        it->position = position;
    }
}

void MatchVisualizer::updatePlayerAnimation(const std::string& playerId, const std::string& animation) {
    auto it = std::find_if(playerModels.begin(), playerModels.end(),
                          [&](const PlayerModel& p) { return p.playerId == playerId; });
    
    if (it != playerModels.end()) {
        it->currentAnimation = animation;
        it->animationTime = 0.0f;
    }
}

void MatchVisualizer::setBallPosition(const glm::vec3& position) {
    ball.position = position;
    
    // Update ball trail
    if (ballTrailEnabled) {
        ballTrail.push_back(position);
        if (ballTrail.size() > 50) { // Limit trail length
            ballTrail.erase(ballTrail.begin());
        }
    }
}

void MatchVisualizer::setBallVelocity(const glm::vec3& velocity) {
    ball.velocity = velocity;
}

void MatchVisualizer::animateBallTrajectory(const std::vector<glm::vec3>& trajectory, float duration) {
    // This would animate the ball along the given trajectory
    // Implementation would involve interpolation over time
}

void MatchVisualizer::onBallBowled(const BallEvent& event) {
    // Show ball being bowled
    ball.isVisible = true;
    
    // Add particle effects
    if (!particleSystems.empty()) {
        particleSystems[0]->createDustCloud(ball.position, 2.0f);
    }
    
    // Add screen effect
    if (!screenEffects.empty()) {
        screenEffects[0]->addSlowMotionEffect(2.0f, 0.3f);
    }
}

void MatchVisualizer::onBallHit(const BallEvent& event) {
    // Show ball being hit
    if (!particleSystems.empty()) {
        particleSystems[0]->createSparkle(ball.position, 30);
    }
    
    // Add screen effect
    if (!screenEffects.empty()) {
        screenEffects[0]->addCelebrationFlashEffect(1.0f);
    }
}

void MatchVisualizer::onWicket(const BallEvent& event) {
    // Show wicket celebration
    if (!particleSystems.empty()) {
        particleSystems[0]->createCelebration(ball.position, 100);
    }
    
    // Add screen effect
    if (!screenEffects.empty()) {
        screenEffects[0]->addWicketHighlightEffect(3.0f);
    }
}

void MatchVisualizer::onBoundary(const BallEvent& event) {
    // Show boundary celebration
    if (!particleSystems.empty()) {
        particleSystems[0]->createCelebration(ball.position, 80);
    }
    
    // Add screen effect
    if (!screenEffects.empty()) {
        glm::vec4 boundaryColor = (event.result == BallResult::FOUR) ? 
            glm::vec4(1, 1, 0, 1) : glm::vec4(1, 0, 0, 1); // Yellow for 4, Red for 6
        screenEffects[0]->addBoundaryGlowEffect(2.0f, boundaryColor);
    }
}

// Helper methods implementation
bool MatchVisualizer::initializeShaders() {
    // Initialize field shader
    fieldShader = std::make_unique<Shader>();
    if (!fieldShader->loadFromFiles("shaders/field_vertex.glsl", "shaders/field_fragment.glsl")) {
        return false;
    }
    
    // Initialize player shader
    playerShader = std::make_unique<Shader>();
    if (!playerShader->loadFromFiles("shaders/player_vertex.glsl", "shaders/player_fragment.glsl")) {
        return false;
    }
    
    // Initialize ball shader
    ballShader = std::make_unique<Shader>();
    if (!ballShader->loadFromFiles("shaders/ball_vertex.glsl", "shaders/ball_fragment.glsl")) {
        return false;
    }
    
    // Initialize particle shader
    particleShader = std::make_unique<Shader>();
    if (!particleShader->loadFromFiles("shaders/particle_vertex.glsl", "shaders/particle_fragment.glsl")) {
        return false;
    }
    
    return true;
}

bool MatchVisualizer::loadModels() {
    // Load player model
    playerModel = std::make_unique<Model>();
    if (!playerModel->loadFromFile("assets/models/player.obj")) {
        std::cout << "Warning: Could not load player model, using default" << std::endl;
    }
    
    // Load ball model
    ballModel = std::make_unique<Model>();
    if (!ballModel->loadFromFile("assets/models/ball.obj")) {
        std::cout << "Warning: Could not load ball model, using default" << std::endl;
    }
    
    // Load field model
    fieldModel = std::make_unique<Model>();
    if (!fieldModel->loadFromFile("assets/models/field.obj")) {
        std::cout << "Warning: Could not load field model, using default" << std::endl;
    }
    
    // Load stadium model
    stadiumModel = std::make_unique<Model>();
    if (!stadiumModel->loadFromFile("assets/models/stadium.obj")) {
        std::cout << "Warning: Could not load stadium model, using default" << std::endl;
    }
    
    return true;
}

bool MatchVisualizer::loadTextures() {
    // Load various textures
    // Implementation would load grass, player uniforms, ball texture, etc.
    return true;
}

void MatchVisualizer::setupLighting() {
    // Lighting is already initialized in constructor
}

void MatchVisualizer::setupCamera() {
    // Camera is already initialized in constructor
}

void MatchVisualizer::setupField() {
    // Setup field dimensions based on venue
    if (venue) {
        field.pitchPosition = glm::vec3(0, 0, 0);
        field.pitchSize = glm::vec3(20, 0.1f, 3); // Standard cricket pitch dimensions
        field.boundarySize = glm::vec3(150, 0, 150); // Approximate boundary size
    }
    
    calculateFieldingPositions();
}

void MatchVisualizer::setupPlayers() {
    playerModels.clear();
    
    // Add players from both teams
    if (team1) {
        for (const auto& player : team1->getPlayers()) {
            PlayerModel pm;
            pm.playerId = player.getName();
            pm.position = glm::vec3(0, 0, 0);
            pm.rotation = glm::vec3(0, 0, 0);
            pm.scale = glm::vec3(1, 1, 1);
            pm.currentAnimation = "idle";
            pm.animationTime = 0.0f;
            pm.isVisible = true;
            pm.teamId = 1;
            pm.role = "fielder";
            playerModels.push_back(pm);
        }
    }
    
    if (team2) {
        for (const auto& player : team2->getPlayers()) {
            PlayerModel pm;
            pm.playerId = player.getName();
            pm.position = glm::vec3(0, 0, 0);
            pm.rotation = glm::vec3(0, 0, 0);
            pm.scale = glm::vec3(1, 1, 1);
            pm.currentAnimation = "idle";
            pm.animationTime = 0.0f;
            pm.isVisible = true;
            pm.teamId = 2;
            pm.role = "fielder";
            playerModels.push_back(pm);
        }
    }
}

glm::mat4 MatchVisualizer::getViewMatrix() const {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 MatchVisualizer::getProjectionMatrix() const {
    return glm::perspective(glm::radians(camera.fov), camera.aspectRatio, camera.nearPlane, camera.farPlane);
}

glm::mat4 MatchVisualizer::getModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);
    return model;
}

void MatchVisualizer::calculateFieldingPositions() {
    // Calculate standard fielding positions around the pitch
    field.fieldingPositions.clear();
    
    // Add standard fielding positions (simplified)
    field.fieldingPositions.push_back(glm::vec3(-30, 0, 20));  // Slip
    field.fieldingPositions.push_back(glm::vec3(-20, 0, 15));  // Point
    field.fieldingPositions.push_back(glm::vec3(0, 0, 30));    // Cover
    field.fieldingPositions.push_back(glm::vec3(20, 0, 15));   // Mid-off
    field.fieldingPositions.push_back(glm::vec3(30, 0, 20));   // Mid-on
    field.fieldingPositions.push_back(glm::vec3(0, 0, -5));    // Wicket-keeper
    field.fieldingPositions.push_back(glm::vec3(0, 0, 50));    // Bowler
}

void MatchVisualizer::updateCamera(float deltaTime) {
    if (camera.transitionTime > 0) {
        updateCameraTransition(deltaTime);
    } else {
        calculateCameraPosition(camera.mode);
    }
}

void MatchVisualizer::updateAnimations(float deltaTime) {
    for (auto& player : playerModels) {
        updatePlayerAnimation(player.playerId, deltaTime);
    }
}

void MatchVisualizer::updateParticles(float deltaTime) {
    for (auto& particleSystem : particleSystems) {
        particleSystem->update(deltaTime);
    }
}

void MatchVisualizer::updateEffects(float deltaTime) {
    for (auto& screenEffect : screenEffects) {
        screenEffect->update(deltaTime);
    }
}

void MatchVisualizer::calculateCameraPosition(CameraMode mode) {
    switch (mode) {
        case CameraMode::BROADCAST:
            camera.position = glm::vec3(0, 15, 25);
            camera.target = glm::vec3(0, 0, 0);
            break;
        case CameraMode::BOWLER_VIEW:
            camera.position = glm::vec3(0, 2, 15);
            camera.target = glm::vec3(0, 1, 0);
            break;
        case CameraMode::BATSMAN_VIEW:
            camera.position = glm::vec3(0, 2, -15);
            camera.target = glm::vec3(0, 1, 0);
            break;
        case CameraMode::DRONE_VIEW:
            camera.position = glm::vec3(0, 50, 0);
            camera.target = glm::vec3(0, 0, 0);
            break;
        default:
            break;
    }
}

void MatchVisualizer::updateCameraTransition(float deltaTime) {
    camera.transitionTime -= deltaTime;
    if (camera.transitionTime <= 0) {
        camera.transitionTime = 0;
        camera.position = camera.targetPosition;
        camera.target = camera.targetTarget;
    } else {
        float t = 1.0f - (camera.transitionTime / 1.0f); // Assuming 1 second transition
        t = t * t * (3.0f - 2.0f * t); // Smoothstep interpolation
        
        camera.position = glm::mix(camera.position, camera.targetPosition, t);
        camera.target = glm::mix(camera.target, camera.targetTarget, t);
    }
}

void MatchVisualizer::updatePlayerAnimation(const std::string& playerId, float deltaTime) {
    auto it = std::find_if(playerModels.begin(), playerModels.end(),
                          [&](const PlayerModel& p) { return p.playerId == playerId; });
    
    if (it != playerModels.end()) {
        it->animationTime += deltaTime;
        // Animation logic would go here
    }
} 