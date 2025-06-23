#include "AuctionVisualizer.h"
#include <glad/glad.h>
#include "Shader.h"
#include "Model.h"
#include "ParticleSystem.h"
#include "ScreenEffect.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <random>

AuctionVisualizer::AuctionVisualizer() 
    : framebuffer(0), renderTexture(0), depthBuffer(0)
    , viewportWidth(1280), viewportHeight(720)
    , currentPlayer(nullptr)
    , globalTime(0.0f), slowMotionSpeed(1.0f), slowMotionEnabled(false)
    , visualQuality(2), targetFrameRate(60), vsyncEnabled(true)
    , auctionTimer(0.0f), currentBid(0.0f), remainingTimeSeconds(30) {
    
    // Initialize camera
    cameraPosition = glm::vec3(0, 15, 25);
    cameraTarget = glm::vec3(0, 0, 0);
    cameraUp = glm::vec3(0, 1, 0);
    cameraFov = 45.0f;
    cameraNear = 0.1f;
    cameraFar = 1000.0f;
    cameraAspect = 16.0f / 9.0f;
    cameraMode = AuctionCameraMode::HALL_OVERVIEW;
    cameraTransitionTime = 0.0f;
    
    // Initialize lighting
    ambientColor = glm::vec3(0.2f, 0.2f, 0.3f);
    directionalColor = glm::vec3(1.0f, 0.95f, 0.8f);
    directionalDirection = glm::vec3(0.5f, 1.0f, 0.3f);
    ambientIntensity = 0.3f;
    directionalIntensity = 0.7f;
    shadowsEnabled = true;
    
    // Initialize auction hall
    hall.stagePosition = glm::vec3(0, 0, 0);
    hall.stageSize = glm::vec3(20, 0.1f, 10);
    hall.auctioneerPosition = glm::vec3(0, 1, -5);
    hall.playerDisplayPosition = glm::vec3(0, 2, 0);
    hall.screenPosition = glm::vec3(0, 8, -15);
    hall.hallSize = glm::vec3(50, 20, 40);
    
    // Setup lighting positions
    hall.lightingPositions[0] = glm::vec3(-20, 15, 0);
    hall.lightingPositions[1] = glm::vec3(20, 15, 0);
    hall.lightingPositions[2] = glm::vec3(0, 15, -20);
    hall.lightingPositions[3] = glm::vec3(0, 15, 20);
    
    // Initialize UI
    ui.showPlayerStats = true;
    ui.showBiddingHistory = true;
    ui.showTeamBudgets = true;
    ui.showAuctionProgress = true;
    ui.showTimer = true;
    ui.uiScale = 1.0f;
    ui.screenSize = glm::vec2(1280, 720);
    
    // Initialize auction state
    auctionState = AuctionState::SETUP;
}

AuctionVisualizer::~AuctionVisualizer() {
    cleanup();
}

bool AuctionVisualizer::initialize(int windowWidth, int windowHeight) {
    viewportWidth = windowWidth;
    viewportHeight = windowHeight;
    cameraAspect = static_cast<float>(windowWidth) / windowHeight;
    ui.screenSize = glm::vec2(windowWidth, windowHeight);
    
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
    particleSystems[0]->initialize(300);
    
    // Initialize screen effects
    screenEffects.push_back(std::make_unique<ScreenEffect>());
    screenEffects[0]->initialize(viewportWidth, viewportHeight);
    
    setupLighting();
    setupCamera();
    setupHall();
    
    std::cout << "AuctionVisualizer initialized successfully" << std::endl;
    return true;
}

void AuctionVisualizer::cleanup() {
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
    
    hallShader.reset();
    teamShader.reset();
    playerShader.reset();
    uiShader.reset();
    particleShader.reset();
    
    hallModel.reset();
    teamRepModel.reset();
    playerModel.reset();
    stageModel.reset();
    screenModel.reset();
    textures.clear();
    
    particleSystems.clear();
    screenEffects.clear();
}

void AuctionVisualizer::setupAuction(const std::vector<Team*>& teams, const std::vector<Player*>& players) {
    setupTeams();
    setupPlayers();
    
    // Setup team representatives
    teamReps.clear();
    for (size_t i = 0; i < teams.size(); ++i) {
        TeamRepresentative rep;
        rep.teamName = teams[i]->getName();
        rep.position = hall.teamTablePositions[i % hall.teamTablePositions.size()];
        rep.rotation = glm::vec3(0, 0, 0);
        rep.scale = glm::vec3(1, 1, 1);
        rep.currentAnimation = "idle";
        rep.animationTime = 0.0f;
        rep.isActive = false;
        rep.isBidding = false;
        rep.bidAmount = 0.0f;
        rep.teamColor = getTeamColor(teams[i]->getName());
        rep.representativeName = "Team Rep " + std::to_string(i + 1);
        rep.teamId = i;
        teamReps.push_back(rep);
    }
    
    // Setup player models
    playerModels.clear();
    for (const auto& player : players) {
        PlayerModel pm;
        pm.playerId = player->getName();
        pm.playerName = player->getName();
        pm.position = hall.playerDisplayPosition;
        pm.rotation = glm::vec3(0, 0, 0);
        pm.scale = glm::vec3(1, 1, 1);
        pm.currentAnimation = "idle";
        pm.animationTime = 0.0f;
        pm.isVisible = false;
        pm.isHighlighted = false;
        pm.highlightIntensity = 0.0f;
        pm.playerData = player;
        pm.basePrice = calculateBasePrice(player);
        pm.currentBid = 0.0f;
        pm.status = "Available";
        
        // Add player stats
        pm.stats.push_back("Batting: " + std::to_string(player->getBattingSkill()));
        pm.stats.push_back("Bowling: " + std::to_string(player->getBowlingSkill()));
        pm.stats.push_back("Fielding: " + std::to_string(player->getFieldingSkill()));
        pm.stats.push_back("Experience: " + std::to_string(player->getExperience()));
        pm.stats.push_back("Age: " + std::to_string(player->getAge()));
        
        playerModels.push_back(pm);
    }
    
    currentPlayer = nullptr;
    auctionState = AuctionState::SETUP;
}

void AuctionVisualizer::render(float deltaTime) {
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
    renderHall();
    renderTeams();
    renderCurrentPlayer();
    renderEffects();
    
    // Render UI elements
    renderUI();
    
    // End framebuffer rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Apply post-processing effects
    screenEffects[0]->beginRender();
    screenEffects[0]->renderEffects();
    screenEffects[0]->endRender();
}

void AuctionVisualizer::renderHall() {
    if (!hallShader || !hallModel) return;
    
    hallShader->use();
    
    // Set matrices
    glm::mat4 modelMatrix = getModelMatrix(glm::vec3(0), glm::vec3(0), glm::vec3(1));
    hallShader->setMat4("model", modelMatrix);
    hallShader->setMat4("view", getViewMatrix());
    hallShader->setMat4("projection", getProjectionMatrix());
    
    // Set lighting
    hallShader->setVec3("lightPos", directionalDirection);
    hallShader->setVec3("viewPos", cameraPosition);
    hallShader->setVec3("lightColor", directionalColor);
    hallShader->setFloat("ambientStrength", ambientIntensity);
    hallShader->setFloat("diffuseStrength", directionalIntensity);
    
    // Set auction hall specific uniforms
    hallShader->setFloat("time", globalTime);
    hallShader->setBool("isAuctionActive", auctionState == AuctionState::BIDDING_ACTIVE);
    hallShader->setFloat("auctionIntensity", 1.0f);
    hallShader->setVec3("hallAmbientColor", ambientColor);
    hallShader->setVec3("hallSpotlightColor", glm::vec3(1.0f, 0.9f, 0.7f));
    hallShader->setVec3("stageSpotlightPos", hall.playerDisplayPosition);
    
    // Render hall
    hallModel->draw(*hallShader);
    
    // Render stage
    if (stageModel) {
        glm::mat4 stageMatrix = getModelMatrix(hall.stagePosition, glm::vec3(0), hall.stageSize);
        hallShader->setMat4("model", stageMatrix);
        stageModel->draw(*hallShader);
    }
    
    // Render screen
    if (screenModel) {
        glm::mat4 screenMatrix = getModelMatrix(hall.screenPosition, glm::vec3(0), glm::vec3(15, 9, 0.1f));
        hallShader->setMat4("model", screenMatrix);
        screenModel->draw(*hallShader);
    }
    
    hallShader->unuse();
}

void AuctionVisualizer::renderTeams() {
    if (!teamShader || !teamRepModel) return;
    
    teamShader->use();
    
    // Set matrices
    teamShader->setMat4("view", getViewMatrix());
    teamShader->setMat4("projection", getProjectionMatrix());
    
    // Set lighting
    teamShader->setVec3("lightPos", directionalDirection);
    teamShader->setVec3("viewPos", cameraPosition);
    teamShader->setVec3("lightColor", directionalColor);
    teamShader->setFloat("ambientStrength", ambientIntensity);
    teamShader->setFloat("diffuseStrength", directionalIntensity);
    
    // Render each team representative
    for (const auto& team : teamReps) {
        renderTeam(team);
    }
    
    teamShader->unuse();
}

void AuctionVisualizer::renderTeam(const TeamRepresentative& team) {
    if (!teamRepModel) return;
    
    glm::mat4 modelMatrix = getModelMatrix(team.position, team.rotation, team.scale);
    teamShader->setMat4("model", modelMatrix);
    
    // Set team-specific uniforms
    teamShader->setVec3("teamColor", team.teamColor);
    teamShader->setBool("isBidding", team.isBidding);
    teamShader->setBool("isActive", team.isActive);
    teamShader->setFloat("bidIntensity", team.isBidding ? 1.0f : 0.0f);
    teamShader->setFloat("bidAmount", team.bidAmount);
    teamShader->setBool("isWinningBidder", team.teamName == winningBidder);
    teamShader->setFloat("time", globalTime);
    teamShader->setFloat("animationTime", team.animationTime);
    
    teamRepModel->draw(*teamShader);
}

void AuctionVisualizer::renderCurrentPlayer() {
    if (!currentPlayer || !playerShader || !playerModel) return;
    
    playerShader->use();
    
    // Set matrices
    glm::mat4 modelMatrix = getModelMatrix(currentPlayer->position, currentPlayer->rotation, currentPlayer->scale);
    playerShader->setMat4("model", modelMatrix);
    playerShader->setMat4("view", getViewMatrix());
    playerShader->setMat4("projection", getProjectionMatrix());
    
    // Set lighting
    playerShader->setVec3("lightPos", directionalDirection);
    playerShader->setVec3("viewPos", cameraPosition);
    playerShader->setVec3("lightColor", directionalColor);
    playerShader->setFloat("ambientStrength", ambientIntensity);
    playerShader->setFloat("diffuseStrength", directionalIntensity);
    
    // Set player-specific uniforms
    playerShader->setBool("isHighlighted", currentPlayer->isHighlighted);
    playerShader->setFloat("highlightIntensity", currentPlayer->highlightIntensity);
    playerShader->setFloat("basePrice", currentPlayer->basePrice);
    playerShader->setFloat("currentBid", currentPlayer->currentBid);
    playerShader->setFloat("time", globalTime);
    
    playerModel->draw(*playerShader);
    
    playerShader->unuse();
}

void AuctionVisualizer::renderUI() {
    // This would render the auction UI elements
    // Implementation depends on the UI system being used
    // For now, this is a placeholder for the UI rendering
}

void AuctionVisualizer::setCameraMode(AuctionCameraMode mode) {
    if (cameraMode == mode) return;
    
    cameraMode = mode;
    cameraTransitionTime = 0.0f;
    
    // Calculate new camera position based on mode
    calculateCameraPosition(mode);
    
    if (cameraChangeCallback) {
        cameraChangeCallback(mode);
    }
}

void AuctionVisualizer::focusOnTeam(const std::string& teamName) {
    auto it = std::find_if(teamReps.begin(), teamReps.end(),
                          [&](const TeamRepresentative& t) { return t.teamName == teamName; });
    
    if (it != teamReps.end()) {
        setCameraMode(AuctionCameraMode::TEAM_TABLE);
        // Store team for focusing (would need to add to camera struct)
    }
}

void AuctionVisualizer::focusOnPlayer(const std::string& playerId) {
    auto it = std::find_if(playerModels.begin(), playerModels.end(),
                          [&](const PlayerModel& p) { return p.playerId == playerId; });
    
    if (it != playerModels.end()) {
        setCameraMode(AuctionCameraMode::PLAYER_DETAIL);
        // Store player for focusing
    }
}

void AuctionVisualizer::setTeamBidding(const std::string& teamName, bool isBidding, float bidAmount) {
    auto it = std::find_if(teamReps.begin(), teamReps.end(),
                          [&](const TeamRepresentative& t) { return t.teamName == teamName; });
    
    if (it != teamReps.end()) {
        it->isBidding = isBidding;
        it->bidAmount = bidAmount;
        
        if (isBidding) {
            // Add bidding effects
            addParticleEffect(it->position, "bid_placed");
            addScreenEffect("bid_flash", 0.5f);
        }
    }
}

void AuctionVisualizer::setCurrentPlayer(const std::string& playerId) {
    auto it = std::find_if(playerModels.begin(), playerModels.end(),
                          [&](const PlayerModel& p) { return p.playerId == playerId; });
    
    if (it != playerModels.end()) {
        currentPlayer = &(*it);
        currentPlayer->isVisible = true;
        currentPlayer->isHighlighted = true;
        currentPlayer->highlightIntensity = 1.0f;
        
        // Transition camera to player
        setCameraMode(AuctionCameraMode::PLAYER_DETAIL);
        
        // Add presentation effects
        addParticleEffect(currentPlayer->position, "player_presented");
    }
}

void AuctionVisualizer::onBidPlaced(const std::string& teamName, float bidAmount) {
    // TODO: Implement or leave empty if not needed
}

void AuctionVisualizer::onBidWon(const std::string& teamName, float finalBid) {
    // TODO: Implement or leave empty if not needed
}

void AuctionVisualizer::onPlayerSold(const std::string& playerId, const std::string& teamName, float price) {
    // Update player status
    auto it = std::find_if(playerModels.begin(), playerModels.end(),
                          [&](const PlayerModel& p) { return p.playerId == playerId; });
    
    if (it != playerModels.end()) {
        it->status = "Sold";
        it->currentBid = price;
        it->isHighlighted = false;
        it->highlightIntensity = 0.0f;
    }
    
    // Add celebration effects
    addCelebrationEffect(teamName);
    
    // Add screen effects
    addScreenEffect("player_sold", 3.0f);
    
    if (playerSoldCallback) {
        playerSoldCallback(playerId, teamName, price);
    }
}

void AuctionVisualizer::addCelebrationEffect(const std::string& teamName) {
    auto it = std::find_if(teamReps.begin(), teamReps.end(),
                          [&](const TeamRepresentative& t) { return t.teamName == teamName; });
    
    if (it != teamReps.end()) {
        // Add particle effects
        addParticleEffect(it->position, "celebration");
        
        // Add screen effects
        addScreenEffect("celebration_flash", 1.0f);
    }
}

// Helper methods implementation
bool AuctionVisualizer::initializeShaders() {
    // Initialize hall shader
    hallShader = std::make_unique<Shader>();
    if (!hallShader->loadFromFiles("shaders/auction_hall_vertex.glsl", "shaders/auction_hall_fragment.glsl")) {
        return false;
    }
    
    // Initialize team shader
    teamShader = std::make_unique<Shader>();
    if (!teamShader->loadFromFiles("shaders/team_representative_vertex.glsl", "shaders/team_representative_fragment.glsl")) {
        return false;
    }
    
    // Initialize player shader (reuse from match visualizer)
    playerShader = std::make_unique<Shader>();
    if (!playerShader->loadFromFiles("shaders/player_vertex.glsl", "shaders/player_fragment.glsl")) {
        return false;
    }
    
    // Initialize particle shader
    particleShader = std::make_unique<Shader>();
    if (!particleShader->loadFromFiles("shaders/particle_vertex.glsl", "shaders/particle_fragment.glsl")) {
        return false;
    }
    
    return true;
}

bool AuctionVisualizer::loadModels() {
    // Load hall model
    hallModel = std::make_unique<Model>();
    if (!hallModel->loadFromFile("assets/models/auction_hall.obj")) {
        std::cout << "Warning: Could not load auction hall model, using default" << std::endl;
    }
    
    // Load team representative model
    teamRepModel = std::make_unique<Model>();
    if (!teamRepModel->loadFromFile("assets/models/team_representative.obj")) {
        std::cout << "Warning: Could not load team representative model, using default" << std::endl;
    }
    
    // Load player model (reuse from match visualizer)
    playerModel = std::make_unique<Model>();
    if (!playerModel->loadFromFile("assets/models/player.obj")) {
        std::cout << "Warning: Could not load player model, using default" << std::endl;
    }
    
    // Load stage model
    stageModel = std::make_unique<Model>();
    if (!stageModel->loadFromFile("assets/models/auction_stage.obj")) {
        std::cout << "Warning: Could not load auction stage model, using default" << std::endl;
    }
    
    // Load screen model
    screenModel = std::make_unique<Model>();
    if (!screenModel->loadFromFile("assets/models/auction_screen.obj")) {
        std::cout << "Warning: Could not load auction screen model, using default" << std::endl;
    }
    
    return true;
}

bool AuctionVisualizer::loadTextures() {
    // Load various textures for auction hall
    // Implementation would load hall textures, team uniforms, etc.
    return true;
}

void AuctionVisualizer::setupLighting() {
    // Lighting is already initialized in constructor
}

void AuctionVisualizer::setupCamera() {
    // Camera is already initialized in constructor
}

void AuctionVisualizer::setupHall() {
    // Calculate team table positions around the hall
    hall.teamTablePositions.clear();
    
    // Arrange teams in a semi-circle around the stage
    int numTeams = 8; // Default number of teams
    float radius = 15.0f;
    float angleStep = 3.14159f / (numTeams + 1);
    
    for (int i = 0; i < numTeams; ++i) {
        float angle = -3.14159f / 2 + angleStep * (i + 1);
        glm::vec3 position(
            cos(angle) * radius,
            0.5f,
            sin(angle) * radius
        );
        hall.teamTablePositions.push_back(position);
    }
    
    // Setup audience positions
    hall.audiencePositions.clear();
    for (int i = 0; i < 50; ++i) {
        float angle = (float)i / 50.0f * 2.0f * 3.14159f;
        float radius = 25.0f + (i % 3) * 5.0f;
        glm::vec3 position(
            cos(angle) * radius,
            1.0f,
            sin(angle) * radius
        );
        hall.audiencePositions.push_back(position);
    }
}

void AuctionVisualizer::setupTeams() {
    // Teams are set up in setupAuction method
}

void AuctionVisualizer::setupPlayers() {
    // Players are set up in setupAuction method
}

glm::mat4 AuctionVisualizer::getViewMatrix() const {
    return glm::lookAt(cameraPosition, cameraTarget, cameraUp);
}

glm::mat4 AuctionVisualizer::getProjectionMatrix() const {
    return glm::perspective(glm::radians(cameraFov), cameraAspect, cameraNear, cameraFar);
}

glm::mat4 AuctionVisualizer::getModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);
    return model;
}

void AuctionVisualizer::calculateCameraPosition(AuctionCameraMode mode) {
    switch (mode) {
        case AuctionCameraMode::HALL_OVERVIEW:
            cameraPosition = glm::vec3(0, 20, 30);
            cameraTarget = glm::vec3(0, 0, 0);
            break;
        case AuctionCameraMode::STAGE_FOCUS:
            cameraPosition = glm::vec3(0, 8, 15);
            cameraTarget = hall.stagePosition;
            break;
        case AuctionCameraMode::TEAM_TABLE:
            cameraPosition = glm::vec3(10, 5, 10);
            cameraTarget = glm::vec3(0, 0, 0);
            break;
        case AuctionCameraMode::PLAYER_DETAIL:
            cameraPosition = glm::vec3(0, 3, 8);
            cameraTarget = hall.playerDisplayPosition;
            break;
        case AuctionCameraMode::BIDDING_VIEW:
            cameraPosition = glm::vec3(0, 6, 12);
            cameraTarget = glm::vec3(0, 0, 0);
            break;
        case AuctionCameraMode::CELEBRATION_VIEW:
            cameraPosition = glm::vec3(0, 10, 20);
            cameraTarget = glm::vec3(0, 0, 0);
            break;
        default:
            break;
    }
}

void AuctionVisualizer::updateCamera(float deltaTime) {
    // Camera transition
    if (cameraIsTransitioning) {
        cameraTransitionElapsed += deltaTime;
        float t = glm::clamp(cameraTransitionElapsed / cameraTransitionDuration, 0.0f, 1.0f);
        // Smoothstep for cinematic feel
        t = t * t * (3.0f - 2.0f * t);
        cameraPosition = glm::mix(cameraStartPos, cameraEndPos, t);
        cameraTarget = glm::mix(cameraStartTarget, cameraEndTarget, t);
        if (cameraTransitionElapsed >= cameraTransitionDuration) {
            cameraIsTransitioning = false;
            cameraPosition = cameraEndPos;
            cameraTarget = cameraEndTarget;
        }
    } else {
        calculateCameraPosition(cameraMode);
    }
    // Camera shake
    if (cameraShakeDuration > 0.0f) {
        cameraShakeElapsed += deltaTime;
        float shakeT = 1.0f - glm::clamp(cameraShakeElapsed / cameraShakeDuration, 0.0f, 1.0f);
        float shakeMag = cameraShakeIntensity * shakeT;
        cameraShakeOffset = glm::vec3(randf(-1,1), randf(-1,1), randf(-1,1)) * shakeMag * 0.2f;
        cameraPosition += cameraShakeOffset;
        if (cameraShakeElapsed >= cameraShakeDuration) {
            cameraShakeDuration = 0.0f;
            cameraShakeIntensity = 0.0f;
            cameraShakeOffset = glm::vec3(0.0f);
        }
    }
}

void AuctionVisualizer::updateAnimations(float deltaTime) {
    for (auto& team : teamReps) {
        updateTeamAnimationState(team, deltaTime);
        team.animationTime += deltaTime;
    }
    if (currentPlayer) {
        updatePlayerAnimationState(*currentPlayer, deltaTime);
        currentPlayer->animationTime += deltaTime;
    }
}

void AuctionVisualizer::updateParticles(float deltaTime) {
    for (auto& particleSystem : particleSystems) {
        particleSystem->update(deltaTime);
    }
}

void AuctionVisualizer::updateEffects(float deltaTime) {
    for (auto& screenEffect : screenEffects) {
        screenEffect->update(deltaTime);
    }
}

void AuctionVisualizer::updateTeamAnimationState(TeamRepresentative& team, float deltaTime) {
    if (team.animBlend < 1.0f) {
        team.animBlendTime += deltaTime;
        team.animBlend = glm::clamp(team.animBlendTime / team.animBlendDuration, 0.0f, 1.0f);
    }
    // Example: auto-return to Idle after Bidding/Applauding
    if (team.animState == RepAnimationState::Bidding && team.animationTime > 1.0f) {
        setTeamAnimationState(team.teamName, RepAnimationState::Idle);
        team.animationTime = 0.0f;
    }
    if (team.animState == RepAnimationState::Applauding && team.animationTime > 1.5f) {
        setTeamAnimationState(team.teamName, RepAnimationState::Idle);
        team.animationTime = 0.0f;
    }
}

void AuctionVisualizer::updatePlayerAnimationState(PlayerModel& player, float deltaTime) {
    if (player.animBlend < 1.0f) {
        player.animBlendTime += deltaTime;
        player.animBlend = glm::clamp(player.animBlendTime / player.animBlendDuration, 0.0f, 1.0f);
    }
    // Example: auto-return to Idle after Waving
    if (player.animState == PlayerAnimationState::Waving && player.animationTime > 1.2f) {
        setPlayerAnimationState(player.playerId, PlayerAnimationState::Idle);
        player.animationTime = 0.0f;
    }
}

void AuctionVisualizer::setTeamAnimationState(const std::string& teamName, RepAnimationState state) {
    auto it = std::find_if(teamReps.begin(), teamReps.end(), [&](const TeamRepresentative& t) { return t.teamName == teamName; });
    if (it != teamReps.end() && it->animState != state) {
        it->prevAnimState = it->animState;
        it->animState = state;
        it->animBlend = 0.0f;
        it->animBlendTime = 0.0f;
    }
}

void AuctionVisualizer::setPlayerAnimationState(const std::string& playerId, PlayerAnimationState state) {
    auto it = std::find_if(playerModels.begin(), playerModels.end(), [&](const PlayerModel& p) { return p.playerId == playerId; });
    if (it != playerModels.end() && it->animState != state) {
        it->prevAnimState = it->animState;
        it->animState = state;
        it->animBlend = 0.0f;
        it->animBlendTime = 0.0f;
    }
}

void AuctionVisualizer::triggerCameraShake(float intensity, float duration) {
    cameraShakeIntensity = intensity;
    cameraShakeDuration = duration;
    cameraShakeElapsed = 0.0f;
}

void AuctionVisualizer::transitionCamera(const glm::vec3& newPosition, const glm::vec3& newTarget, float duration) {
    cameraStartPos = cameraPosition;
    cameraStartTarget = cameraTarget;
    cameraEndPos = newPosition;
    cameraEndTarget = newTarget;
    cameraTransitionElapsed = 0.0f;
    cameraTransitionDuration = duration;
    cameraIsTransitioning = true;
}

glm::vec3 AuctionVisualizer::getTeamColor(const std::string& teamName) {
    // Return team colors based on team name
    if (teamName.find("Mumbai") != std::string::npos) return glm::vec3(0.0f, 0.5f, 1.0f);
    if (teamName.find("Chennai") != std::string::npos) return glm::vec3(1.0f, 0.5f, 0.0f);
    if (teamName.find("Delhi") != std::string::npos) return glm::vec3(0.0f, 0.0f, 0.8f);
    if (teamName.find("Kolkata") != std::string::npos) return glm::vec3(0.8f, 0.0f, 0.0f);
    if (teamName.find("Punjab") != std::string::npos) return glm::vec3(1.0f, 0.0f, 0.0f);
    if (teamName.find("Rajasthan") != std::string::npos) return glm::vec3(0.8f, 0.6f, 0.0f);
    if (teamName.find("Bangalore") != std::string::npos) return glm::vec3(1.0f, 0.0f, 0.0f);
    if (teamName.find("Hyderabad") != std::string::npos) return glm::vec3(1.0f, 0.5f, 0.0f);
    
    // Default color
    return glm::vec3(0.5f, 0.5f, 0.5f);
}

float AuctionVisualizer::calculateBasePrice(const Player* player) {
    // Calculate base price based on player stats
    float basePrice = 50.0f; // Base price in lakhs
    
    // Add value based on skills
    basePrice += player->getBattingSkill() * 0.5f;
    basePrice += player->getBowlingSkill() * 0.5f;
    basePrice += player->getFieldingSkill() * 0.3f;
    basePrice += player->getExperience() * 0.2f;
    
    // Adjust for age (younger players might be more valuable)
    if (player->getAge() < 25) basePrice *= 1.2f;
    else if (player->getAge() > 35) basePrice *= 0.8f;
    
    return basePrice;
}

// Utility for random float
static float randf(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
} 