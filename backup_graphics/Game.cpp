#include "Game.h"
#include <glad/glad.h>
#include "Player.h"
#include "Team.h"
#include "MatchEngine.h"
#include "AuctionManager.h"
#include "CommentaryManager.h"
#include "GUIManager.h"
#include "AnimationHandler.h"
#include "Venue.h"
#include "DataManager.h"
#include "MatchVisualizer.h"
#include "MainMenuManager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

Game::Game() 
    : window(nullptr)
    , glContext(nullptr)
    , windowWidth(1280)
    , windowHeight(720)
    , isRunning(false)
    , currentState(GameState::MAIN_MENU)
    , previousState(GameState::MAIN_MENU)
    , currentTeam1(nullptr)
    , currentTeam2(nullptr)
    , currentVenue(nullptr)
    , currentMatchFormat(MatchFormat::T20)
    , matchInProgress(false)
    , careerTeam(nullptr)
    , careerYear(1)
    , careerMatches(0)
    , graphicsQuality(1)
    , audioEnabled(true)
    , commentaryEnabled(true)
    , lastFrameTime(0)
    , deltaTime(0.0f)
{
    // Initialize subsystems
    guiManager = std::make_unique<GUIManager>(this);
    matchEngine = std::make_unique<MatchEngine>();
    auctionManager = std::make_unique<AuctionManager>();
    commentaryManager = std::make_unique<CommentaryManager>();
    animationHandler = std::make_unique<AnimationHandler>();
    dataManager = std::make_unique<DataManager>();
    matchVisualizer = std::make_unique<MatchVisualizer>();
    mainMenuManager = std::make_unique<MainMenuManager>(guiManager->GetUIRenderer());
}

Game::~Game() {
    cleanup();
}

bool Game::initialize(const std::string& windowTitle, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    // Initialize SDL
    if (!initializeSDL()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return false;
    }
    
    // Create window
    window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize OpenGL
    if (!initializeOpenGL()) {
        std::cerr << "Failed to initialize OpenGL" << std::endl;
        return false;
    }
    
    // Set viewport
    glViewport(0, 0, windowWidth, windowHeight);
    
    // Set UIRenderer viewport
    if (guiManager) {
        guiManager->GetUIRenderer()->setViewport(windowWidth, windowHeight);
    }
    
    if (animationHandler) {
        animationHandler->initializeGL();
    }
    
    // Initialize ImGui
    if (!initializeImGui()) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        return false;
    }
    
    // Initialize subsystems
    if (!guiManager->Initialize()) {
        std::cerr << "Failed to initialize GUI manager" << std::endl;
        return false;
    }
    
    if (!animationHandler) {
        std::cerr << "Failed to initialize Animation Handler" << std::endl;
        return false;
    }
    
    // Initialize MatchVisualizer
    if (!matchVisualizer->initialize(windowWidth, windowHeight)) {
        std::cerr << "Failed to initialize Match Visualizer" << std::endl;
        return false;
    }
    
    // Initialize MainMenuManager
    if (!mainMenuManager->initialize()) {
        std::cerr << "Failed to initialize Main Menu Manager" << std::endl;
        return false;
    }
    
    // Setup menu callbacks
    mainMenuManager->setAuctionStartCallback([this]() {
        setState(GameState::AUCTION);
    });
    
    mainMenuManager->setMatchStartCallback([this]() {
        // Start a demo match
        if (!teams.empty() && !venues.empty()) {
            startMatch(teams[0], teams[1], venues[0], MatchFormat::T20);
        }
    });
    
    // Load settings
    loadSettings();
    
    // Initialize data manager with API key if available
    if (!apiKey.empty()) {
        initializeDataManager(apiKey);
    }
    
    // Load game data
    loadGameData();
    
    isRunning = true;
    lastFrameTime = SDL_GetTicks();
    
    std::cout << "Cricket Manager initialized successfully!" << std::endl;
    return true;
}

void Game::cleanup() {
    if (isRunning) {
        saveGameData();
        saveSettings();
    }
    
    if (guiManager) {
        guiManager->Shutdown();
    }
    
    if (matchVisualizer) {
        matchVisualizer->cleanup();
    }
    
    if (animationHandler) {
        // AnimationHandler cleanup is handled in destructor
    }
    
    // Ensure OpenGL context is current before ImGui cleanup
    if (glContext && window) {
        SDL_GL_MakeCurrent(window, glContext);
    }
    
    // Cleanup ImGui
    try {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    } catch (...) {
        std::cerr << "Warning: ImGui cleanup failed, continuing..." << std::endl;
    }
    
    // Cleanup OpenGL context
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }
    
    // Cleanup window
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    // Quit SDL
    SDL_Quit();
    
    isRunning = false;
}

void Game::run() {
    while (isRunning) {
        updateDeltaTime();
        
        handleEvents();
        processInput();
        update(deltaTime);
        render();
        
        // Cap frame rate
        Uint32 frameTime = SDL_GetTicks() - lastFrameTime;
        if (frameTime < 16) { // ~60 FPS
            SDL_Delay(16 - frameTime);
        }
    }
}

void Game::update(float deltaTime) {
    // Update subsystems
    if (matchEngine) {
        matchEngine->Update(deltaTime);
    }
    
    if (animationHandler) {
        animationHandler->Update(deltaTime);
    }
    
    if (commentaryManager) {
        commentaryManager->Update(deltaTime);
    }
    
    if (auctionManager) {
        auctionManager->update(deltaTime);
    }
    
    if (guiManager) {
        guiManager->Update(deltaTime);
    }
    
    // Update main menu manager
    if (mainMenuManager && currentState == GameState::MAIN_MENU) {
        mainMenuManager->update(deltaTime);
    }
    
    // Update game state
    UpdateGameState(deltaTime);
}

void Game::UpdateGameState(float deltaTime) {
    // Update game state based on current state
    switch (currentState) {
        case GameState::MENU:
        case GameState::MAIN_MENU:
            // Main menu is handled by MainMenuManager
            break;
        case GameState::TEAM_MANAGEMENT:
            // Handle team management updates
            break;
        case GameState::MATCH_SETUP:
            // Handle match setup updates
            break;
        case GameState::MATCH_IN_PROGRESS:
            // Handle match in progress updates
            break;
        case GameState::AUCTION:
            // Handle auction updates
            break;
        case GameState::CAREER:
            // Handle career mode updates
            break;
        case GameState::SETTINGS:
            // Handle settings updates
            break;
        case GameState::PLAYING:
            // Handle playing state updates
            break;
        case GameState::PAUSED:
            // Handle paused state updates
            break;
        case GameState::EXIT:
            isRunning = false;
            break;
    }
}

void Game::render() {
    // Set clear color and clear screen
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f); // Blue background for testing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    std::cout << "Rendering - Current state: " << static_cast<int>(currentState) << std::endl;
    
    // Render main menu if in main menu state
    if (currentState == GameState::MAIN_MENU && mainMenuManager) {
        std::cout << "Rendering main menu..." << std::endl;
        mainMenuManager->render();
    } else {
        std::cout << "Not rendering main menu - state: " << static_cast<int>(currentState) 
                  << ", mainMenuManager: " << (mainMenuManager ? "exists" : "null") << std::endl;
    }
    
    // Render match visualizer if match is in progress
    if (matchInProgress && matchVisualizer) {
        std::cout << "Rendering match visualizer..." << std::endl;
        matchVisualizer->render(deltaTime);
    }
    
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    // Render GUI (only if not in main menu)
    if (guiManager && currentState != GameState::MAIN_MENU) {
        guiManager->Render();
    }
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Swap buffers
    SDL_GL_SwapWindow(window);
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    windowWidth = event.window.data1;
                    windowHeight = event.window.data2;
                    glViewport(0, 0, windowWidth, windowHeight);
                }
                break;
            case SDL_MOUSEMOTION:
                // Handle mouse movement for main menu
                if (currentState == GameState::MAIN_MENU && mainMenuManager) {
                    mainMenuManager->handleMouseMove(event.motion.x, event.motion.y);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Handle mouse click for main menu
                    if (currentState == GameState::MAIN_MENU && mainMenuManager) {
                        mainMenuManager->handleMouseClick(event.button.x, event.button.y);
                    }
                }
                break;
            case SDL_KEYDOWN:
                // Handle keyboard input for main menu
                if (currentState == GameState::MAIN_MENU && mainMenuManager) {
                    mainMenuManager->handleKeyPress(event.key.keysym.sym);
                }
                
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (currentState == GameState::MAIN_MENU) {
                        setState(GameState::EXIT);
                    } else {
                        setState(GameState::MAIN_MENU);
                    }
                }
                break;
        }
    }
}

void Game::processInput() {
    // Process keyboard input
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    
    // Handle global shortcuts
    if (state[SDL_SCANCODE_F1]) {
        setState(GameState::MAIN_MENU);
    }
    if (state[SDL_SCANCODE_F2]) {
        setState(GameState::TEAM_MANAGEMENT);
    }
    if (state[SDL_SCANCODE_F3]) {
        setState(GameState::MATCH_SETUP);
    }
    if (state[SDL_SCANCODE_F4]) {
        setState(GameState::AUCTION);
    }
    if (state[SDL_SCANCODE_F5]) {
        setState(GameState::CAREER);
    }
    if (state[SDL_SCANCODE_F6]) {
        setState(GameState::SETTINGS);
    }
}

void Game::setState(GameState newState) {
    previousState = currentState;
    currentState = newState;
}

Game::GameState Game::getState() const {
    return currentState;
}

void Game::loadGameData() {
    // Try to load from API first if available
    if (dataManager && !apiKey.empty()) {
        try {
            std::cout << "Loading data from API..." << std::endl;
            
            // Load teams
            teams = dataManager->fetchTeams();
            std::cout << "Loaded " << teams.size() << " teams from API" << std::endl;
            
            // Load players
            players = dataManager->fetchPlayers();
            std::cout << "Loaded " << players.size() << " players from API" << std::endl;
            
            // Load venues
            venues = dataManager->fetchVenues();
            std::cout << "Loaded " << venues.size() << " venues from API" << std::endl;
            
            return;
        } catch (const std::exception& e) {
            std::cerr << "API data loading failed: " << e.what() << std::endl;
            std::cout << "Falling back to default data..." << std::endl;
        }
    }
    
    // Load default data if API is not available
    loadDefaultData();
}

void Game::saveGameData() {
    // Save current game state
    std::ofstream file("savegame.dat", std::ios::binary);
    if (file.is_open()) {
        // Save basic game stats
        file.write(reinterpret_cast<const char*>(&stats), sizeof(stats));
        
        // Save career data
        file.write(careerPlayerName.c_str(), careerPlayerName.length() + 1);
        file.write(reinterpret_cast<const char*>(&careerYear), sizeof(careerYear));
        file.write(reinterpret_cast<const char*>(&careerMatches), sizeof(careerMatches));
        
        file.close();
        std::cout << "Game data saved successfully" << std::endl;
    }
}

void Game::initializeDataManager(const std::string& apiKey) {
    if (dataManager) {
        dataManager->setApiKey(apiKey);
        this->apiKey = apiKey;
        std::cout << "DataManager initialized with API key" << std::endl;
    }
}

void Game::refreshLiveData() {
    if (dataManager && !apiKey.empty()) {
        try {
            // Refresh live matches
            auto liveMatches = dataManager->fetchLiveMatches();
            if (!liveMatches.empty()) {
                std::cout << "Refreshed live match data" << std::endl;
            }
            
            // Refresh news
            auto news = dataManager->fetchCricketNews();
            if (!news.empty()) {
                std::cout << "Refreshed cricket news" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Failed to refresh live data: " << e.what() << std::endl;
        }
    }
}

void Game::startMatch(Team& team1, Team& team2, Venue& venue, MatchFormat format) {
    currentTeam1 = &team1;
    currentTeam2 = &team2;
    currentVenue = &venue;
    currentMatchFormat = format;
    
    // Initialize match engine
    if (matchEngine) {
        MatchType matchType;
        switch (format) {
            case MatchFormat::T20:
                matchType = MatchType::T20;
                break;
            case MatchFormat::ODI:
                matchType = MatchType::ODI;
                break;
            default:
                matchType = MatchType::TEST;
                break;
        }
        
        matchEngine->InitializeMatch(&team1, &team2, &venue, matchType);
    }
    
    // Initialize match visualizer
    if (matchVisualizer) {
        matchVisualizer->setupMatch(&team1, &team2, &venue, 
            (format == MatchFormat::T20) ? MatchType::T20 : 
            (format == MatchFormat::ODI) ? MatchType::ODI : MatchType::TEST);
    }
    
    // Initialize commentary
    if (commentaryManager) {
        commentaryManager->StartMatch(team1.GetName(), team2.GetName(), venue.GetName());
    }
    
    // Set game state
    setState(GameState::MATCH_IN_PROGRESS);
    matchInProgress = true;
    
    std::cout << "Match started: " << team1.GetName() << " vs " << team2.GetName() << std::endl;
}

void Game::pauseMatch() {
    if (matchEngine && matchInProgress) {
        matchEngine->PauseMatch();
    }
}

void Game::resumeMatch() {
    if (matchEngine && matchInProgress) {
        matchEngine->ResumeMatch();
    }
}

void Game::endMatch() {
    if (matchEngine && matchInProgress) {
        matchEngine->EndMatch();
    }
    
    matchInProgress = false;
    currentTeam1 = nullptr;
    currentTeam2 = nullptr;
    currentVenue = nullptr;
    
    // Update stats
    stats.matchesPlayed++;
    
    setState(GameState::MAIN_MENU);
}

void Game::startTournament(const std::string& tournamentName, 
                          const std::vector<Team>& participatingTeams) {
    // Tournament implementation would go here
    std::cout << "Starting tournament: " << tournamentName << std::endl;
}

void Game::advanceTournament() {
    // Tournament advancement logic
}

void Game::startCareer(const std::string& playerName, Team& startingTeam) {
    careerPlayerName = playerName;
    careerTeam = &startingTeam;
    careerYear = 1;
    careerMatches = 0;
    
    setState(GameState::CAREER);
}

void Game::advanceCareer() {
    careerYear++;
    // Career advancement logic
}

void Game::setGraphicsQuality(int quality) {
    graphicsQuality = quality;
    
    // Update visualizer quality
    if (matchVisualizer) {
        VisualQuality visualQuality = VisualQuality::MEDIUM;
        // Set graphics quality
        switch (quality) {
            case 0: visualQuality = VisualQuality::LOW; break;
            case 1: visualQuality = VisualQuality::MEDIUM; break;
            case 2: visualQuality = VisualQuality::HIGH; break;
            default: visualQuality = VisualQuality::MEDIUM; break;
        }
    }
}

void Game::setAudioEnabled(bool enabled) {
    audioEnabled = enabled;
}

void Game::setCommentaryEnabled(bool enabled) {
    commentaryEnabled = enabled;
}

void Game::setApiKey(const std::string& key) {
    apiKey = key;
    if (dataManager) {
        dataManager->setApiKey(key);
    }
}

void Game::exportStats(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Cricket Manager Statistics\n";
        file << "========================\n\n";
        file << "Matches Played: " << stats.matchesPlayed << "\n";
        file << "Tournaments Won: " << stats.tournamentsWon << "\n";
        file << "Players Managed: " << stats.playersManaged << "\n";
        file << "Total Runs: " << stats.totalRuns << "\n";
        file << "Total Wickets: " << stats.totalWickets << "\n";
        file << "Win Percentage: " << stats.winPercentage << "%\n";
        file.close();
        std::cout << "Statistics exported to " << filename << std::endl;
    }
}

// Private helper methods
bool Game::initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    return true;
}

bool Game::initializeOpenGL() {
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize OpenGL extensions
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    return true;
}

bool Game::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Note: Docking and Viewports are only available in newer ImGui versions
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Setup ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void Game::updateDeltaTime() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentTime;
}

void Game::renderMainMenu() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    
    ImGui::Begin("Main Menu", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 100, 100));
    ImGui::Text("Cricket Manager");
    
    ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 80, 200));
    if (ImGui::Button("Team Management", ImVec2(160, 40))) {
        setState(GameState::TEAM_MANAGEMENT);
    }
    
    ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 80, 260));
    if (ImGui::Button("Match Simulation", ImVec2(160, 40))) {
        setState(GameState::MATCH_SETUP);
    }
    
    ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 80, 320));
    if (ImGui::Button("Auction", ImVec2(160, 40))) {
        setState(GameState::AUCTION);
    }
    
    ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 80, 380));
    if (ImGui::Button("Career Mode", ImVec2(160, 40))) {
        setState(GameState::CAREER);
    }
    
    ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 80, 440));
    if (ImGui::Button("Settings", ImVec2(160, 40))) {
        setState(GameState::SETTINGS);
    }
    
    ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 80, 500));
    if (ImGui::Button("Exit", ImVec2(160, 40))) {
        setState(GameState::EXIT);
    }
    
    ImGui::End();
}

void Game::renderTeamManagement() {
    ImGui::Begin("Team Management");
    
    if (ImGui::BeginTabBar("TeamTabs")) {
        if (ImGui::BeginTabItem("Teams")) {
            for (auto& team : teams) {
                if (ImGui::TreeNode(team.GetName().c_str())) {
                    ImGui::Text("Country: %s", team.GetCity().c_str());
                    ImGui::Text("Home Venue: %s", team.GetHomeVenue().c_str());
                    ImGui::Text("Players: %zu", team.GetSquad().size());
                    ImGui::TreePop();
                }
            }
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Players")) {
            for (auto& player : players) {
                if (ImGui::TreeNode(player.GetName().c_str())) {
                    ImGui::Text("Age: %d", player.GetAge());
                    ImGui::Text("Role: %s", player.GetRole().c_str());
                    ImGui::Text("Batting Average: %.2f", player.GetPlayerStats().average);
                    ImGui::TreePop();
                }
            }
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    if (ImGui::Button("Back to Main Menu")) {
        setState(GameState::MAIN_MENU);
    }
    
    ImGui::End();
}

void Game::renderMatchSimulation() {
    // Render 3D match visualization
    if (matchVisualizer && matchInProgress) {
        matchVisualizer->render(deltaTime);
    } else {
        // Fallback 2D rendering or loading screen
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void Game::renderAuction() {
    ImGui::Begin("Auction");
    
    if (auctionManager) {
        auto& session = auctionManager->getCurrentSession();
        ImGui::Text("Auction Status: %s", session.isActive ? "Active" : "Inactive");
        ImGui::Text("Current Lot: %d", session.currentLotIndex);
        ImGui::Text("Current Bid: $%.2f", auctionManager->getCurrentBid());
        ImGui::Text("Current Bidder: %s", auctionManager->getCurrentBidder().c_str());
        
        static int bidAmount = 100000;
        ImGui::InputInt("Bid Amount", &bidAmount);
        
        if (ImGui::Button("Place Bid")) {
            auctionManager->placeBid("Player Team", bidAmount);
        }
    }
    
    if (ImGui::Button("Back to Main Menu")) {
        setState(GameState::MAIN_MENU);
    }
    
    ImGui::End();
}

void Game::renderCareer() {
    ImGui::Begin("Career Mode");
    
    ImGui::Text("Player: %s", careerPlayerName.c_str());
    ImGui::Text("Team: %s", careerTeam ? careerTeam->GetName().c_str() : "None");
    ImGui::Text("Year: %d", careerYear);
    ImGui::Text("Matches: %d", careerMatches);
    
    if (ImGui::Button("Advance Career")) {
        advanceCareer();
    }
    
    if (ImGui::Button("Back to Main Menu")) {
        setState(GameState::MAIN_MENU);
    }
    
    ImGui::End();
}

void Game::renderSettings() {
    ImGui::Begin("Settings");
    
    ImGui::Text("Graphics Quality");
    ImGui::RadioButton("Low", &graphicsQuality, 0);
    ImGui::RadioButton("Medium", &graphicsQuality, 1);
    ImGui::RadioButton("High", &graphicsQuality, 2);
    
    ImGui::Checkbox("Audio Enabled", &audioEnabled);
    ImGui::Checkbox("Commentary Enabled", &commentaryEnabled);
    
    static char apiKeyBuffer[256] = "";
    strcpy(apiKeyBuffer, apiKey.c_str());
    if (ImGui::InputText("API Key", apiKeyBuffer, sizeof(apiKeyBuffer))) {
        apiKey = apiKeyBuffer;
        if (dataManager) {
            dataManager->setApiKey(apiKey);
        }
    }
    
    if (ImGui::Button("Save Settings")) {
        saveSettings();
    }
    
    if (ImGui::Button("Back to Main Menu")) {
        setState(GameState::MAIN_MENU);
    }
    
    ImGui::End();
}

void Game::handleStateTransition() {
    if (currentState != previousState) {
        // Handle state transition logic
        previousState = currentState;
    }
}

void Game::loadDefaultData() {
    // Create some default teams
    Team india("India", TeamType::INTERNATIONAL);
    Team australia("Australia", TeamType::INTERNATIONAL);
    Team england("England", TeamType::INTERNATIONAL);
    
    teams.push_back(india);
    teams.push_back(australia);
    teams.push_back(england);
    
    // Create some default players
    Player kohli("Virat Kohli", 35, PlayerRole::BATSMAN);
    Player bumrah("Jasprit Bumrah", 30, PlayerRole::BOWLER);
    
    players.push_back(kohli);
    players.push_back(bumrah);
    
    // Create some default venues
    Venue wankhede("Wankhede Stadium", "Mumbai", VenueType::STADIUM);
    Venue mcg("MCG", "Melbourne", VenueType::STADIUM);
    Venue lords("Lord's", "London", VenueType::STADIUM);
    
    venues.push_back(wankhede);
    venues.push_back(mcg);
    venues.push_back(lords);
    
    std::cout << "Loaded default data: " << teams.size() << " teams, " 
              << players.size() << " players, " << venues.size() << " venues" << std::endl;
}

void Game::saveSettings() {
    std::ofstream file("settings.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&graphicsQuality), sizeof(graphicsQuality));
        file.write(reinterpret_cast<const char*>(&audioEnabled), sizeof(audioEnabled));
        file.write(reinterpret_cast<const char*>(&commentaryEnabled), sizeof(commentaryEnabled));
        
        // Save API key
        file.write(apiKey.c_str(), apiKey.length() + 1);
        
        file.close();
        std::cout << "Settings saved successfully" << std::endl;
    }
}

void Game::loadSettings() {
    std::ifstream file("settings.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&graphicsQuality), sizeof(graphicsQuality));
        file.read(reinterpret_cast<char*>(&audioEnabled), sizeof(audioEnabled));
        file.read(reinterpret_cast<char*>(&commentaryEnabled), sizeof(commentaryEnabled));
        
        // Load API key
        std::getline(file, apiKey, '\0');
        
        file.close();
        std::cout << "Settings loaded successfully" << std::endl;
    }
} 