#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <memory>
#include <string>
#include <vector>
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

class Game {
public:
    Game();
    ~Game();
    
    // Initialization
    bool initialize(const std::string& windowTitle = "Cricket Manager", 
                   int width = 1280, int height = 720);
    void cleanup();
    
    // Main game loop
    void run();
    void update(float deltaTime);
    void render();
    
    // Event handling
    void handleEvents();
    void processInput();
    
    // State management
    enum class MatchFormat {
        TEST,
        ODI,
        T20
    };

    enum class GameState {
        MENU,
        MAIN_MENU,
        TEAM_MANAGEMENT,
        MATCH_SETUP,
        MATCH_IN_PROGRESS,
        AUCTION,
        CAREER,
        SETTINGS,
        PLAYING,
        PAUSED,
        EXIT
    };

    enum class VisualQuality {
        LOW,
        MEDIUM,
        HIGH,
        ULTRA
    };
    
    void setState(GameState newState);
    GameState getState() const;
    
    // Data management
    void loadGameData();
    void saveGameData();
    
    // API integration
    void initializeDataManager(const std::string& apiKey = "");
    void refreshLiveData();
    
    // Getters for subsystems
    MatchEngine* getMatchEngine() { return matchEngine.get(); }
    AuctionManager* getAuctionManager() { return auctionManager.get(); }
    CommentaryManager* getCommentaryManager() { return commentaryManager.get(); }
    GUIManager* getGUIManager() { return guiManager.get(); }
    AnimationHandler* getAnimationHandler() { return animationHandler.get(); }
    DataManager* getDataManager() { return dataManager.get(); }
    MatchVisualizer* getMatchVisualizer() { return matchVisualizer.get(); }
    
    // Team and player management
    std::vector<Team>& getTeams() { return teams; }
    std::vector<Player>& getPlayers() { return players; }
    std::vector<Venue>& getVenues() { return venues; }
    
    // Match management
    void startMatch(Team& team1, Team& team2, Venue& venue, MatchFormat format);
    void pauseMatch();
    void resumeMatch();
    void endMatch();
    
    // Tournament management
    void startTournament(const std::string& tournamentName, 
                        const std::vector<Team>& participatingTeams);
    void advanceTournament();
    
    // Career mode
    void startCareer(const std::string& playerName, Team& startingTeam);
    void advanceCareer();
    
    // Settings
    void setGraphicsQuality(int quality); // 0=Low, 1=Medium, 2=High
    void setAudioEnabled(bool enabled);
    void setCommentaryEnabled(bool enabled);
    void setApiKey(const std::string& apiKey);
    
    // Statistics
    struct GameStats {
        int matchesPlayed = 0;
        int tournamentsWon = 0;
        int playersManaged = 0;
        int totalRuns = 0;
        int totalWickets = 0;
        float winPercentage = 0.0f;
    };
    
    GameStats getStats() const { return stats; }
    void exportStats(const std::string& filename);

    // Getters
    SDL_Window* GetWindow() const { return window; }
    SDL_GLContext GetGLContext() const { return glContext; }
    int GetWindowWidth() const { return windowWidth; }
    int GetWindowHeight() const { return windowHeight; }
    GameState GetCurrentState() const { return currentState; }
    bool IsRunning() const { return isRunning; }

private:
    // SDL and OpenGL
    SDL_Window* window;
    SDL_GLContext glContext;
    int windowWidth, windowHeight;
    bool isRunning;
    
    // Game state
    GameState currentState;
    GameState previousState;
    
    // Subsystems
    std::unique_ptr<MatchEngine> matchEngine;
    std::unique_ptr<AuctionManager> auctionManager;
    std::unique_ptr<CommentaryManager> commentaryManager;
    std::unique_ptr<GUIManager> guiManager;
    std::unique_ptr<AnimationHandler> animationHandler;
    std::unique_ptr<DataManager> dataManager;
    std::unique_ptr<MatchVisualizer> matchVisualizer;
    
    // Game data
    std::vector<Team> teams;
    std::vector<Player> players;
    std::vector<Venue> venues;
    
    // Current match/tournament
    Team* currentTeam1;
    Team* currentTeam2;
    Venue* currentVenue;
    MatchFormat currentMatchFormat;
    bool matchInProgress;
    
    // Career mode
    std::string careerPlayerName;
    Team* careerTeam;
    int careerYear;
    int careerMatches;
    
    // Settings
    int graphicsQuality;
    bool audioEnabled;
    bool commentaryEnabled;
    std::string apiKey;
    
    // Statistics
    GameStats stats;
    
    // Timing
    Uint32 lastFrameTime;
    float deltaTime;
    
    // Helper methods
    bool initializeSDL();
    bool initializeOpenGL();
    bool initializeImGui();
    void updateDeltaTime();
    void UpdateGameState(float deltaTime);
    void renderMainMenu();
    void renderTeamManagement();
    void renderMatchSimulation();
    void renderAuction();
    void renderCareer();
    void renderSettings();
    void handleStateTransition();
    void loadDefaultData();
    void saveSettings();
    void loadSettings();
}; 