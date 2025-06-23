#include "AuctionVisualizer.h"
#include "AuctionManager.h"
#include "Player.h"
#include "Team.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>

class AuctionDemo {
private:
    std::unique_ptr<AuctionVisualizer> visualizer;
    std::unique_ptr<AuctionManager> auctionManager;
    std::vector<std::unique_ptr<Team>> teams;
    std::vector<std::unique_ptr<Player>> players;
    
    // Demo state
    float demoTime = 0.0f;
    int currentPlayerIndex = 0;
    bool demoRunning = true;
    
    // Demo sequence
    enum class DemoPhase {
        SETUP,
        PLAYER_PRESENTATION,
        BIDDING_WAR,
        PLAYER_SOLD,
        NEXT_PLAYER,
        COMPLETE
    };
    DemoPhase currentPhase = DemoPhase::SETUP;
    float phaseTimer = 0.0f;
    
public:
    AuctionDemo() {
        initializeDemo();
    }
    
    void run() {
        std::cout << "=== 3D Auction Visualization Demo ===" << std::endl;
        std::cout << "Press 'Q' to quit, 'SPACE' to pause/resume" << std::endl;
        std::cout << "Press '1-6' to change camera modes" << std::endl;
        std::cout << "Press 'R' to reset demo" << std::endl;
        std::cout << std::endl;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (demoRunning) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Cap delta time to prevent large jumps
            deltaTime = std::min(deltaTime, 0.016f);
            
            updateDemo(deltaTime);
            renderDemo();
            
            // Handle input
            handleInput();
            
            // Frame rate control
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    }
    
private:
    void initializeDemo() {
        // Initialize SDL and OpenGL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        SDL_Window* window = SDL_CreateWindow(
            "3D Auction Visualization Demo",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            1280, 720,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
        
        SDL_GLContext glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
        
        // Initialize visualizer
        visualizer = std::make_unique<AuctionVisualizer>();
        if (!visualizer->initialize(1280, 720)) {
            std::cerr << "Failed to initialize AuctionVisualizer!" << std::endl;
            return;
        }
        
        // Initialize auction manager
        auctionManager = std::make_unique<AuctionManager>();
        auctionManager->initialize();
        
        // Create demo teams
        createDemoTeams();
        
        // Create demo players
        createDemoPlayers();
        
        // Setup auction
        setupAuction();
        
        // Setup callbacks
        setupCallbacks();
        
        std::cout << "Demo initialized successfully!" << std::endl;
    }
    
    void createDemoTeams() {
        teams.clear();
        
        // Create IPL-style teams
        std::vector<std::string> teamNames = {
            "Mumbai Indians", "Chennai Super Kings", "Delhi Capitals",
            "Kolkata Knight Riders", "Punjab Kings", "Rajasthan Royals",
            "Royal Challengers Bangalore", "Sunrisers Hyderabad"
        };
        
        std::vector<float> budgets = {2000, 1800, 1900, 1700, 1600, 1500, 2000, 1800};
        
        for (size_t i = 0; i < teamNames.size(); ++i) {
            auto team = std::make_unique<Team>();
            team->setName(teamNames[i]);
            team->setBudget(budgets[i]);
            teams.push_back(std::move(team));
        }
    }
    
    void createDemoPlayers() {
        players.clear();
        
        // Create star players with different skills
        std::vector<std::string> playerNames = {
            "Virat Kohli", "Rohit Sharma", "MS Dhoni", "KL Rahul",
            "Jasprit Bumrah", "Rashid Khan", "Andre Russell", "Ben Stokes"
        };
        
        std::vector<std::string> roles = {
            "Batsman", "Batsman", "Wicketkeeper", "Batsman",
            "Bowler", "Bowler", "All-rounder", "All-rounder"
        };
        
        for (size_t i = 0; i < playerNames.size(); ++i) {
            auto player = std::make_unique<Player>();
            player->setName(playerNames[i]);
            player->setRole(roles[i]);
            player->setAge(25 + (i % 10)); // Varying ages
            
            // Set skills based on role
            if (roles[i] == "Batsman") {
                player->setBattingSkill(85 + (i % 15));
                player->setBowlingSkill(30 + (i % 20));
                player->setFieldingSkill(70 + (i % 20));
            } else if (roles[i] == "Bowler") {
                player->setBattingSkill(40 + (i % 30));
                player->setBowlingSkill(85 + (i % 15));
                player->setFieldingSkill(75 + (i % 15));
            } else { // All-rounder
                player->setBattingSkill(75 + (i % 20));
                player->setBowlingSkill(75 + (i % 20));
                player->setFieldingSkill(80 + (i % 15));
            }
            
            player->setExperience(5 + (i % 8));
            players.push_back(std::move(player));
        }
    }
    
    void setupAuction() {
        // Convert to raw pointers for setup
        std::vector<Team*> teamPtrs;
        std::vector<Player*> playerPtrs;
        
        for (auto& team : teams) {
            teamPtrs.push_back(team.get());
        }
        
        for (auto& player : players) {
            playerPtrs.push_back(player.get());
        }
        
        // Setup auction manager
        auctionManager->createAuctionSession("IPL Mega Auction 2024", AuctionType::IPL_STYLE);
        auctionManager->addTeams(teamPtrs);
        auctionManager->addPlayers(playerPtrs);
        
        // Setup team budgets
        std::map<std::string, float> budgets;
        for (auto& team : teams) {
            budgets[team->getName()] = team->getBudget();
        }
        auctionManager->setTeamBudgets(budgets);
        
        // Setup team strategies
        std::map<std::string, BiddingStrategy> strategies;
        strategies["Mumbai Indians"] = BiddingStrategy::AGGRESSIVE;
        strategies["Chennai Super Kings"] = BiddingStrategy::BALANCED;
        strategies["Delhi Capitals"] = BiddingStrategy::CONSERVATIVE;
        strategies["Kolkata Knight Riders"] = BiddingStrategy::AGGRESSIVE;
        strategies["Punjab Kings"] = BiddingStrategy::DESPERATE;
        strategies["Rajasthan Royals"] = BiddingStrategy::STRATEGIC;
        strategies["Royal Challengers Bangalore"] = BiddingStrategy::AGGRESSIVE;
        strategies["Sunrisers Hyderabad"] = BiddingStrategy::BALANCED;
        
        auctionManager->setTeamStrategies(strategies);
        
        // Setup visualizer
        visualizer->setupAuction(teamPtrs, playerPtrs);
        visualizer->setAuctionState(AuctionState::SETUP);
    }
    
    void setupCallbacks() {
        // Setup auction manager callbacks
        auctionManager->setBidPlacedCallback([this](const std::string& teamName, float bidAmount) {
            visualizer->onBidPlaced(teamName, bidAmount);
            std::cout << "Bid placed: " << teamName << " - " << bidAmount << " lakhs" << std::endl;
        });
        
        auctionManager->setLotSoldCallback([this](const std::string& playerId, const std::string& teamName, float price) {
            visualizer->onPlayerSold(playerId, teamName, price);
            std::cout << "Player sold: " << playerId << " to " << teamName << " for " << price << " lakhs" << std::endl;
        });
        
        auctionManager->setAuctionEndCallback([this]() {
            std::cout << "Auction completed!" << std::endl;
            currentPhase = DemoPhase::COMPLETE;
        });
        
        // Setup visualizer callbacks
        visualizer->setCameraChangeCallback([this](AuctionCameraMode mode) {
            std::cout << "Camera mode changed to: " << static_cast<int>(mode) << std::endl;
        });
    }
    
    void updateDemo(float deltaTime) {
        demoTime += deltaTime;
        phaseTimer += deltaTime;
        
        switch (currentPhase) {
            case DemoPhase::SETUP:
                updateSetupPhase(deltaTime);
                break;
            case DemoPhase::PLAYER_PRESENTATION:
                updatePlayerPresentationPhase(deltaTime);
                break;
            case DemoPhase::BIDDING_WAR:
                updateBiddingWarPhase(deltaTime);
                break;
            case DemoPhase::PLAYER_SOLD:
                updatePlayerSoldPhase(deltaTime);
                break;
            case DemoPhase::NEXT_PLAYER:
                updateNextPlayerPhase(deltaTime);
                break;
            case DemoPhase::COMPLETE:
                updateCompletePhase(deltaTime);
                break;
        }
    }
    
    void updateSetupPhase(float deltaTime) {
        if (phaseTimer > 2.0f) {
            std::cout << "Starting auction..." << std::endl;
            visualizer->setAuctionState(AuctionState::PLAYER_PRESENTATION);
            currentPhase = DemoPhase::PLAYER_PRESENTATION;
            phaseTimer = 0.0f;
        }
    }
    
    void updatePlayerPresentationPhase(float deltaTime) {
        if (phaseTimer > 1.0f) {
            // Present current player
            if (currentPlayerIndex < players.size()) {
                std::string playerId = players[currentPlayerIndex]->getName();
                visualizer->setCurrentPlayer(playerId);
                
                // Transition camera to player detail view
                visualizer->setCameraMode(AuctionCameraMode::PLAYER_DETAIL);
                
                std::cout << "Presenting player: " << playerId << std::endl;
                currentPhase = DemoPhase::BIDDING_WAR;
                phaseTimer = 0.0f;
            } else {
                currentPhase = DemoPhase::COMPLETE;
            }
        }
    }
    
    void updateBiddingWarPhase(float deltaTime) {
        if (phaseTimer > 1.0f) {
            // Simulate bidding war
            std::string currentPlayerId = players[currentPlayerIndex]->getName();
            float basePrice = 50.0f + (currentPlayerIndex * 10.0f);
            
            // Start bidding
            visualizer->setAuctionState(AuctionState::BIDDING_ACTIVE);
            visualizer->setCameraMode(AuctionCameraMode::BIDDING_VIEW);
            
            // Simulate multiple bids
            simulateBiddingWar(currentPlayerId, basePrice);
            
            currentPhase = DemoPhase::PLAYER_SOLD;
            phaseTimer = 0.0f;
        }
    }
    
    void updatePlayerSoldPhase(float deltaTime) {
        if (phaseTimer > 3.0f) {
            // Show celebration
            visualizer->setCameraMode(AuctionCameraMode::CELEBRATION_VIEW);
            visualizer->triggerCameraShake(0.8f, 1.0f);
            
            // Move to next player
            currentPlayerIndex++;
            if (currentPlayerIndex < players.size()) {
                currentPhase = DemoPhase::NEXT_PLAYER;
                phaseTimer = 0.0f;
            } else {
                currentPhase = DemoPhase::COMPLETE;
            }
        }
    }
    
    void updateNextPlayerPhase(float deltaTime) {
        if (phaseTimer > 1.0f) {
            // Return to overview for next player
            visualizer->setCameraMode(AuctionCameraMode::HALL_OVERVIEW);
            currentPhase = DemoPhase::PLAYER_PRESENTATION;
            phaseTimer = 0.0f;
        }
    }
    
    void updateCompletePhase(float deltaTime) {
        if (phaseTimer > 5.0f) {
            std::cout << "Demo completed! Press 'R' to restart or 'Q' to quit." << std::endl;
        }
    }
    
    void simulateBiddingWar(const std::string& playerId, float basePrice) {
        std::vector<std::string> biddingTeams = {"Mumbai Indians", "Chennai Super Kings", "Royal Challengers Bangalore"};
        float currentBid = basePrice;
        
        for (int round = 0; round < 5; ++round) {
            std::string biddingTeam = biddingTeams[round % biddingTeams.size()];
            currentBid += 10.0f + (round * 5.0f);
            
            // Add delay between bids
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            visualizer->onBidPlaced(biddingTeam, currentBid);
            
            // Focus camera on bidding team
            visualizer->focusOnTeam(biddingTeam);
        }
        
        // Final winning bid
        std::string winningTeam = biddingTeams[0];
        visualizer->onBidWon(winningTeam, currentBid);
        visualizer->onPlayerSold(playerId, winningTeam, currentBid);
    }
    
    void renderDemo() {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        
        // Render auction visualization
        visualizer->render(0.016f); // Fixed delta time for demo
        
        // Swap buffers
        SDL_GL_SwapWindow(SDL_GetWindowFromID(1));
    }
    
    void handleInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    demoRunning = false;
                    break;
                    
                case SDL_KEYDOWN:
                    handleKeyPress(event.key.keysym.sym);
                    break;
            }
        }
    }
    
    void handleKeyPress(SDL_Keycode key) {
        switch (key) {
            case SDLK_q:
                demoRunning = false;
                break;
                
            case SDLK_r:
                resetDemo();
                break;
                
            case SDLK_SPACE:
                // Toggle pause/resume
                break;
                
            case SDLK_1:
                visualizer->setCameraMode(AuctionCameraMode::HALL_OVERVIEW);
                break;
            case SDLK_2:
                visualizer->setCameraMode(AuctionCameraMode::STAGE_FOCUS);
                break;
            case SDLK_3:
                visualizer->setCameraMode(AuctionCameraMode::TEAM_TABLE);
                break;
            case SDLK_4:
                visualizer->setCameraMode(AuctionCameraMode::PLAYER_DETAIL);
                break;
            case SDLK_5:
                visualizer->setCameraMode(AuctionCameraMode::BIDDING_VIEW);
                break;
            case SDLK_6:
                visualizer->setCameraMode(AuctionCameraMode::CELEBRATION_VIEW);
                break;
        }
    }
    
    void resetDemo() {
        currentPlayerIndex = 0;
        currentPhase = DemoPhase::SETUP;
        phaseTimer = 0.0f;
        demoTime = 0.0f;
        
        visualizer->setAuctionState(AuctionState::SETUP);
        visualizer->setCameraMode(AuctionCameraMode::HALL_OVERVIEW);
        
        std::cout << "Demo reset!" << std::endl;
    }
};

int main() {
    try {
        AuctionDemo demo;
        demo.run();
    } catch (const std::exception& e) {
        std::cerr << "Demo error: " << e.what() << std::endl;
        return 1;
    }
    
    SDL_Quit();
    return 0;
} 