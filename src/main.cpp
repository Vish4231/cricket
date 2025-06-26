#include <iostream>
#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <limits>
#include <regex>

using json = nlohmann::json;

// Forward declarations for IPL Cricket Manager components
class IPLPlayer {
public:
    std::string name;
    std::string team;
    std::string role; // Batsman, Bowler, All-rounder, Wicket-keeper
    std::string nationality; // Indian, Overseas
    float battingRating;
    float bowlingRating;
    float fieldingRating;
    float price;
    int age;
    std::string speciality; // Power hitter, Death bowler, etc.
    
    IPLPlayer() : battingRating(0), bowlingRating(0), fieldingRating(0), price(0), age(0) {}
};

class IPLTeam {
public:
    std::string name;
    std::string city;
    std::string homeGround;
    std::string captain;
    std::vector<IPLPlayer> players;
    float budget;
    int points;
    float netRunRate;
    int wins;
    int losses;
    int ties;
    
    IPLTeam() : budget(120.0f), points(0), netRunRate(0.0f), wins(0), losses(0), ties(0) {}
};

// Add AI strategy enum and AI team struct
enum class AIStrategy { AGGRESSIVE, BALANCED, CONSERVATIVE, WILDCARD };

struct AITeam {
    IPLTeam team;
    float budget;
    int overseasCount;
    AIStrategy strategy;
    std::vector<IPLPlayer> squad;
};

class IPLManager {
public:
    IPLManager();
    ~IPLManager();
    
    bool initialize();
    void run();
    void cleanup();
    
private:
    bool isRunning;
    void handleInput();
    void update();
    void render();
    
    // Game States
    enum class GameState {
        MAIN_MENU,
        AVATAR_CUSTOMIZATION,
        TEAM_SELECTION,
        AUCTION,
        SEASON_CALENDAR,
        MATCH_SIMULATION,
        LEAGUE_TABLE,
        PLAYOFFS,
        CAREER_SUMMARY,
        SETTINGS,
        EXIT
    };
    
    GameState currentState;
    
    // Career Data
    struct ManagerProfile {
        std::string name;
        std::string avatar;
        std::string selectedTeam;
        int careerYear;
        int totalMatches;
        int totalWins;
        int totalLosses;
        int championships;
        float winPercentage;
    };
    
    ManagerProfile managerProfile;
    
    // IPL Data
    std::vector<IPLTeam> iplTeams;
    std::vector<IPLPlayer> availablePlayers;
    std::vector<IPLPlayer> auctionPlayers;
    
    // Season Data
    struct Match {
        std::string team1;
        std::string team2;
        std::string venue;
        std::string date;
        bool isPlayed;
        std::string winner;
        int team1Score;
        int team2Score;
    };
    
    std::vector<Match> seasonFixtures;
    int currentMatchIndex;
    bool seasonInProgress;
    
    // AI Data
    std::vector<AITeam> aiTeams;
    
    // UI Methods
    void showMainMenu();
    void showAvatarCustomization();
    void showTeamSelection();
    void showAuction();
    void showSeasonCalendar();
    void showMatchSimulation();
    void showLeagueTable();
    void showPlayoffs();
    void showCareerSummary();
    void showSettings();
    
    // Game Logic Methods
    void initializeIPLTeams();
    void loadPlayersFromAPI();
    void generateSeasonFixtures();
    void simulateMatch(Match& match);
    void updateLeagueTable();
    void calculatePlayoffs();
    
    // Utility Methods
    void clearScreen();
    void printHeader();
    void printFooter();
    void printCentered(const std::string& text);
    void printBanner(const std::string& title);
    void waitForInput();
    std::string getInput();
    
    // API Integration
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    std::string fetchFromAPI(const std::string& url);
    void parsePlayerData(const json& data);
    
    // Add new method for auction simulation
    void simulateAuction();
    
    void showAvailablePlayers();
    void showCurrentSquad();
    void manualAuction();  // Add manual auction method
};

// Constructor
IPLManager::IPLManager() 
    : isRunning(false)
    , currentState(GameState::MAIN_MENU)
    , currentMatchIndex(0)
    , seasonInProgress(false) {
    
    // Initialize manager profile
    managerProfile.name = "";
    managerProfile.avatar = "Default";
    managerProfile.selectedTeam = "";
    managerProfile.careerYear = 1;
    managerProfile.totalMatches = 0;
    managerProfile.totalWins = 0;
    managerProfile.totalLosses = 0;
    managerProfile.championships = 0;
    managerProfile.winPercentage = 0.0f;
}

IPLManager::~IPLManager() {
    cleanup();
}

bool IPLManager::initialize() {
    clearScreen();
    printBanner("🏏 IPL CRICKET MANAGER 2025 🏏");
    std::cout << "\n";
    printCentered("Version: 1.0.0 - Professional Edition");
    printCentered("Platform: Console Mode");
    printCentered("Graphics: Professional ASCII UI");
    std::cout << "\n";
    printCentered("Loading IPL data and initializing game...");
    
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Initialize IPL teams
    initializeIPLTeams();
    
    // Load players from API
    loadPlayersFromAPI();
    
    isRunning = true;
    std::cout << "\n";
    printCentered("✅ Game initialized successfully!");
    std::cout << "\n";
    waitForInput();
    
    return true;
}

void IPLManager::run() {
    while (isRunning) {
        handleInput();
        update();
        render();
        
        // Simple frame rate control
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void IPLManager::cleanup() {
    std::cout << "Cleaning up IPL Manager..." << std::endl;
    curl_global_cleanup();
    isRunning = false;
}

void IPLManager::handleInput() {
    std::string input = getInput();
    
    switch (currentState) {
        case GameState::MAIN_MENU:
            if (input == "1" || input == "start") {
                currentState = GameState::AVATAR_CUSTOMIZATION;
            } else if (input == "2" || input == "settings") {
                currentState = GameState::SETTINGS;
            } else if (input == "3" || input == "quit") {
                currentState = GameState::EXIT;
                isRunning = false;
            }
            break;
            
        case GameState::AVATAR_CUSTOMIZATION:
            if (input == "1" || input == "name") {
                std::cout << "Enter your manager name: ";
                std::getline(std::cin, managerProfile.name);
            } else if (input == "2" || input == "avatar") {
                std::cout << "Select avatar (1-5): ";
                std::string avatarChoice = getInput();
                int choice = std::stoi(avatarChoice);
                if (choice >= 1 && choice <= 5) {
                    managerProfile.avatar = "Avatar_" + std::to_string(choice);
                }
            } else if (input == "3" || input == "continue") {
                if (!managerProfile.name.empty()) {
                    currentState = GameState::TEAM_SELECTION;
                } else {
                    std::cout << "Please enter your name first!" << std::endl;
                    waitForInput();
                }
            } else if (input == "back") {
                currentState = GameState::MAIN_MENU;
            }
            break;
            
        case GameState::TEAM_SELECTION:
            if (input == "back") {
                currentState = GameState::AVATAR_CUSTOMIZATION;
            } else {
                int teamChoice = std::stoi(input);
                if (teamChoice >= 1 && teamChoice <= iplTeams.size()) {
                    managerProfile.selectedTeam = iplTeams[teamChoice - 1].name;
                    currentState = GameState::AUCTION;
                }
            }
            break;
            
        case GameState::AUCTION:
            if (input == "back") {
                currentState = GameState::TEAM_SELECTION;
            } else if (input == "continue") {
                currentState = GameState::SEASON_CALENDAR;
                generateSeasonFixtures();
                seasonInProgress = true;
            } else if (input == "1") {
                // Manual Auction
                manualAuction();
            } else if (input == "2") {
                // Simulate Auction
                simulateAuction();
            } else if (input == "3") {
                // View Available Players
                showAvailablePlayers();
            } else if (input == "4") {
                // View Current Squad
                showCurrentSquad();
            }
            break;
            
        case GameState::SEASON_CALENDAR:
            if (input == "back") {
                currentState = GameState::AUCTION;
            } else if (input == "table") {
                currentState = GameState::LEAGUE_TABLE;
            } else if (input == "match") {
                currentState = GameState::MATCH_SIMULATION;
            } else if (input == "continue") {
                if (currentMatchIndex < seasonFixtures.size()) {
                    currentState = GameState::MATCH_SIMULATION;
                } else {
                    currentState = GameState::PLAYOFFS;
                }
            }
            break;
            
        case GameState::MATCH_SIMULATION:
            if (input == "back") {
                currentState = GameState::SEASON_CALENDAR;
            } else if (input == "simulate") {
                if (currentMatchIndex < seasonFixtures.size()) {
                    simulateMatch(seasonFixtures[currentMatchIndex]);
                    currentMatchIndex++;
                    updateLeagueTable();
                }
            }
            break;
            
        case GameState::LEAGUE_TABLE:
            if (input == "back") {
                currentState = GameState::SEASON_CALENDAR;
            }
            break;
            
        case GameState::PLAYOFFS:
            if (input == "back") {
                currentState = GameState::SEASON_CALENDAR;
            } else if (input == "continue") {
                currentState = GameState::CAREER_SUMMARY;
            }
            break;
            
        case GameState::CAREER_SUMMARY:
            if (input == "back") {
                currentState = GameState::MAIN_MENU;
            } else if (input == "continue") {
                // Start new season
                managerProfile.careerYear++;
                currentMatchIndex = 0;
                seasonInProgress = false;
                currentState = GameState::SEASON_CALENDAR;
            }
            break;
            
        case GameState::SETTINGS:
            if (input == "back") {
                currentState = GameState::MAIN_MENU;
            }
            break;
            
        case GameState::EXIT:
            isRunning = false;
            break;
    }
}

void IPLManager::update() {
    // Update game logic based on current state
    switch (currentState) {
        case GameState::MAIN_MENU:
            // Main menu logic
            break;
        case GameState::AVATAR_CUSTOMIZATION:
            // Avatar customization logic
            break;
        case GameState::TEAM_SELECTION:
            // Team selection logic
            break;
        case GameState::AUCTION:
            // Auction logic
            break;
        case GameState::SEASON_CALENDAR:
            // Season calendar logic
            break;
        case GameState::MATCH_SIMULATION:
            // Match simulation logic
            break;
        case GameState::LEAGUE_TABLE:
            // League table logic
            break;
        case GameState::PLAYOFFS:
            // Playoffs logic
            break;
        case GameState::CAREER_SUMMARY:
            // Career summary logic
            break;
        case GameState::SETTINGS:
            // Settings logic
            break;
        case GameState::EXIT:
            isRunning = false;
            break;
    }
}

void IPLManager::render() {
    clearScreen();
    printHeader();
    
    switch (currentState) {
        case GameState::MAIN_MENU:
            showMainMenu();
            break;
        case GameState::AVATAR_CUSTOMIZATION:
            showAvatarCustomization();
            break;
        case GameState::TEAM_SELECTION:
            showTeamSelection();
            break;
        case GameState::AUCTION:
            showAuction();
            break;
        case GameState::SEASON_CALENDAR:
            showSeasonCalendar();
            break;
        case GameState::MATCH_SIMULATION:
            showMatchSimulation();
            break;
        case GameState::LEAGUE_TABLE:
            showLeagueTable();
            break;
        case GameState::PLAYOFFS:
            showPlayoffs();
            break;
        case GameState::CAREER_SUMMARY:
            showCareerSummary();
            break;
        case GameState::SETTINGS:
            showSettings();
            break;
        case GameState::EXIT:
            std::cout << "Exiting IPL Cricket Manager..." << std::endl;
            break;
    }
    
    printFooter();
}

// UI Methods
void IPLManager::showMainMenu() {
    printBanner("🏠 MAIN MENU");
    std::cout << "\n";
    printCentered("Welcome to IPL Cricket Manager 2025!");
    std::cout << "\n";
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  🎮 1. Start Career                                         ║\n";
    std::cout << "║  ⚙️  2. Settings                                            ║\n";
    std::cout << "║  🚪 3. Quit Game                                            ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showAvatarCustomization() {
    printBanner("👤 AVATAR CUSTOMIZATION");
    std::cout << "\n";
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  Current Profile:                                           ║\n";
    std::cout << "║  Name: " << std::left << std::setw(50) << managerProfile.name << "║\n";
    std::cout << "║  Avatar: " << std::left << std::setw(47) << managerProfile.avatar << "║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  🏷️  1. Set Name                                            ║\n";
    std::cout << "║  🎭 2. Select Avatar                                        ║\n";
    std::cout << "║  ➡️  3. Continue to Team Selection                          ║\n";
    std::cout << "║  ⬅️  back. Go Back                                          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showTeamSelection() {
    printBanner("🏏 TEAM SELECTION");
    std::cout << "\n";
    printCentered("Choose your IPL team for the 2025 season:");
    std::cout << "\n";
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    
    for (size_t i = 0; i < iplTeams.size(); i++) {
        std::cout << "║  " << std::left << std::setw(2) << (i + 1) << ". " 
                  << std::setw(20) << iplTeams[i].name 
                  << " (" << std::setw(15) << iplTeams[i].city << ") ║\n";
    }
    
    std::cout << "║                                                              ║\n";
    std::cout << "║  ⬅️  back. Go Back                                          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showAuction() {
    printBanner("💰 PLAYER AUCTION");
    std::cout << "\n";
    printCentered("Build your squad for the 2025 IPL season!\n");
    
    // Only run auction if user squad < 18
    if (auctionComplete) {
        std::cout << "Auction complete!\n";
        std::cout << "Type 'continue' to proceed to the season." << std::endl;
        std::cout << "Enter your choice: ";
        return;
    }
    
    // User's team
    AITeam* userTeam = nullptr;
    for (auto& ai : aiTeams) {
        if (ai.team.name == managerProfile.selectedTeam) {
            userTeam = &ai;
            break;
        }
    }
    if (!userTeam) return;
    
    // Show auction options
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  Selected Team: " << std::left << std::setw(40) << managerProfile.selectedTeam << "║\n";
    std::cout << "║  Budget: ₹120 Crore                                         ║\n";
    std::cout << "║  Squad Size: 18-25 players                                  ║\n";
    std::cout << "║  Overseas Players: Max 8 in squad, Max 4 in XI              ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  🎯 1. Manual Auction (Bid for each player)                 ║\n";
    std::cout << "║  ⚡ 2. Simulate Auction (Auto-build all teams)              ║\n";
    std::cout << "║  📊 3. View Available Players                               ║\n";
    std::cout << "║  👥 4. View Current Squad                                   ║\n";
    std::cout << "║  ➡️  5. Continue to Season                                  ║\n";
    std::cout << "║  ⬅️  back. Go Back                                          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showSeasonCalendar() {
    printBanner("📅 SEASON CALENDAR");
    std::cout << "\n";
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  Season: " << std::left << std::setw(47) << managerProfile.careerYear << "║\n";
    std::cout << "║  Team: " << std::left << std::setw(50) << managerProfile.selectedTeam << "║\n";
    std::cout << "║  Matches Played: " << std::left << std::setw(41) << currentMatchIndex << "║\n";
    std::cout << "║  Total Matches: " << std::left << std::setw(41) << seasonFixtures.size() << "║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  📊 table. View League Table                                ║\n";
    std::cout << "║  🏏 match. Simulate Next Match                              ║\n";
    std::cout << "║  ➡️  continue. Continue Season                              ║\n";
    std::cout << "║  ⬅️  back. Go Back                                          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showMatchSimulation() {
    printBanner("🏏 MATCH SIMULATION");
    std::cout << "\n";
    
    if (currentMatchIndex < seasonFixtures.size()) {
        Match& currentMatch = seasonFixtures[currentMatchIndex];
        
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  Match " << (currentMatchIndex + 1) << " of " << seasonFixtures.size() << "                          ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  " << std::left << std::setw(25) << currentMatch.team1 << " vs " 
                  << std::setw(25) << currentMatch.team2 << " ║\n";
        std::cout << "║  Venue: " << std::left << std::setw(47) << currentMatch.venue << "║\n";
        std::cout << "║  Date: " << std::left << std::setw(49) << currentMatch.date << "║\n";
        std::cout << "║                                                              ║\n";
        
        if (currentMatch.isPlayed) {
            std::cout << "║  Result: " << std::left << std::setw(46) << currentMatch.winner << "║\n";
            std::cout << "║  Score: " << currentMatch.team1Score << " - " << currentMatch.team2Score << "                                    ║\n";
        } else {
            std::cout << "║  Status: Not Played                                        ║\n";
        }
        
        std::cout << "║                                                              ║\n";
        std::cout << "║  🏏 simulate. Simulate Match                                ║\n";
        std::cout << "║  ⬅️  back. Go Back                                          ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    } else {
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  All matches completed!                                     ║\n";
        std::cout << "║  Proceeding to playoffs...                                  ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  ➡️  continue. Continue to Playoffs                         ║\n";
        std::cout << "║  ⬅️  back. Go Back                                          ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    }
    
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showLeagueTable() {
    printBanner("📊 LEAGUE TABLE");
    std::cout << "\n";
    
    // Sort teams by points, then by NRR
    std::vector<IPLTeam> sortedTeams = iplTeams;
    std::sort(sortedTeams.begin(), sortedTeams.end(), 
              [](const IPLTeam& a, const IPLTeam& b) {
                  if (a.points != b.points) return a.points > b.points;
                  return a.netRunRate > b.netRunRate;
              });
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Pos  Team                P   W   L   T   NRR    Points     ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    for (size_t i = 0; i < sortedTeams.size(); i++) {
        const IPLTeam& team = sortedTeams[i];
        std::string marker = (team.name == managerProfile.selectedTeam) ? "▶ " : "  ";
        
        std::cout << "║  " << std::left << std::setw(3) << (i + 1) 
                  << marker << std::setw(18) << team.name
                  << std::setw(4) << (team.wins + team.losses + team.ties)
                  << std::setw(4) << team.wins
                  << std::setw(4) << team.losses
                  << std::setw(4) << team.ties
                  << std::setw(7) << std::fixed << std::setprecision(3) << team.netRunRate
                  << std::setw(10) << team.points << "     ║\n";
    }
    
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "⬅️  back. Go Back\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showPlayoffs() {
    printBanner("🏆 PLAYOFFS");
    std::cout << "\n";
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  🏆 IPL 2025 Playoffs                                       ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  Qualifier 1: 1st vs 2nd                                    ║\n";
    std::cout << "║  Eliminator: 3rd vs 4th                                     ║\n";
    std::cout << "║  Qualifier 2: Loser Q1 vs Winner Eliminator                 ║\n";
    std::cout << "║  Final: Winner Q1 vs Winner Q2                              ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  ➡️  continue. Simulate Playoffs                            ║\n";
    std::cout << "║  ⬅️  back. Go Back                                          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showCareerSummary() {
    printBanner("📈 CAREER SUMMARY");
    std::cout << "\n";
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  Manager: " << std::left << std::setw(46) << managerProfile.name << "║\n";
    std::cout << "║  Team: " << std::left << std::setw(49) << managerProfile.selectedTeam << "║\n";
    std::cout << "║  Season: " << std::left << std::setw(47) << managerProfile.careerYear << "║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  Season Statistics:                                         ║\n";
    std::cout << "║  Total Matches: " << std::left << std::setw(41) << managerProfile.totalMatches << "║\n";
    std::cout << "║  Wins: " << std::left << std::setw(49) << managerProfile.totalWins << "║\n";
    std::cout << "║  Losses: " << std::left << std::setw(47) << managerProfile.totalLosses << "║\n";
    std::cout << "║  Win Percentage: " << std::left << std::setw(41) << std::fixed << std::setprecision(1) << managerProfile.winPercentage << "% ║\n";
    std::cout << "║  Championships: " << std::left << std::setw(42) << managerProfile.championships << "║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  ➡️  continue. Start Next Season                             ║\n";
    std::cout << "║  ⬅️  back. Return to Main Menu                               ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

void IPLManager::showSettings() {
    printBanner("⚙️ SETTINGS");
    std::cout << "\n";
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  🎮 Game Settings                                           ║\n";
    std::cout << "║  📊 Display Settings                                        ║\n";
    std::cout << "║  🎵 Audio Settings                                          ║\n";
    std::cout << "║  💾 Save/Load Game                                          ║\n";
    std::cout << "║  ℹ️  About                                                   ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  ⬅️  back. Go Back                                          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice: ";
}

// Game Logic Methods
void IPLManager::initializeIPLTeams() {
    // Initialize IPL 2025 teams
    std::vector<std::tuple<std::string, std::string, std::string>> teamData = {
        {"Mumbai Indians", "Mumbai", "Wankhede Stadium"},
        {"Chennai Super Kings", "Chennai", "M. A. Chidambaram Stadium"},
        {"Kolkata Knight Riders", "Kolkata", "Eden Gardens"},
        {"Royal Challengers Bangalore", "Bangalore", "M. Chinnaswamy Stadium"},
        {"Delhi Capitals", "Delhi", "Arun Jaitley Stadium"},
        {"Punjab Kings", "Mohali", "IS Bindra Stadium"},
        {"Rajasthan Royals", "Jaipur", "Sawai Mansingh Stadium"},
        {"Sunrisers Hyderabad", "Hyderabad", "Rajiv Gandhi Stadium"},
        {"Gujarat Titans", "Ahmedabad", "Narendra Modi Stadium"},
        {"Lucknow Super Giants", "Lucknow", "BRSABV Ekana Stadium"}
    };
    
    for (const auto& [name, city, ground] : teamData) {
        IPLTeam team;
        team.name = name;
        team.city = city;
        team.homeGround = ground;
        iplTeams.push_back(team);
    }
    
    aiTeams.clear();
    std::vector<AIStrategy> strategies = {AIStrategy::AGGRESSIVE, AIStrategy::BALANCED, AIStrategy::CONSERVATIVE, AIStrategy::WILDCARD};
    std::random_device rd;
    std::mt19937 g(rd());
    for (size_t i = 0; i < iplTeams.size(); ++i) {
        AITeam ai;
        ai.team = iplTeams[i];
        ai.budget = 120.0f;
        ai.overseasCount = 0;
        ai.strategy = strategies[i % strategies.size()];
        ai.squad.clear();
        aiTeams.push_back(ai);
    }
}

void IPLManager::loadPlayersFromAPI() {
    // For now, create some sample players
    // In a real implementation, this would fetch from the API
    std::vector<std::tuple<std::string, std::string, std::string, std::string, float, float, float, float>> playerData = {
        {"Virat Kohli", "Royal Challengers Bangalore", "Batsman", "Indian", 95.0f, 30.0f, 85.0f, 20.0f},
        {"Rohit Sharma", "Mumbai Indians", "Batsman", "Indian", 92.0f, 25.0f, 80.0f, 18.0f},
        {"MS Dhoni", "Chennai Super Kings", "Wicket-keeper", "Indian", 88.0f, 20.0f, 90.0f, 15.0f},
        {"Jasprit Bumrah", "Mumbai Indians", "Bowler", "Indian", 30.0f, 95.0f, 85.0f, 22.0f},
        {"Rashid Khan", "Gujarat Titans", "Bowler", "Overseas", 40.0f, 92.0f, 88.0f, 18.0f},
        {"Andre Russell", "Kolkata Knight Riders", "All-rounder", "Overseas", 85.0f, 88.0f, 82.0f, 16.0f},
        {"Ben Stokes", "Chennai Super Kings", "All-rounder", "Overseas", 82.0f, 85.0f, 80.0f, 16.5f},
        {"KL Rahul", "Lucknow Super Giants", "Batsman", "Indian", 90.0f, 25.0f, 75.0f, 17.0f},
        {"Rishabh Pant", "Delhi Capitals", "Wicket-keeper", "Indian", 85.0f, 20.0f, 85.0f, 16.0f},
        {"Hardik Pandya", "Gujarat Titans", "All-rounder", "Indian", 80.0f, 75.0f, 78.0f, 15.0f}
    };
    
    for (const auto& [name, team, role, nationality, batting, bowling, fielding, price] : playerData) {
        IPLPlayer player;
        player.name = name;
        player.team = team;
        player.role = role;
        player.nationality = nationality;
        player.battingRating = batting;
        player.bowlingRating = bowling;
        player.fieldingRating = fielding;
        player.price = price;
        player.age = 25 + (rand() % 15); // Random age between 25-40
        availablePlayers.push_back(player);
    }
}

void IPLManager::generateSeasonFixtures() {
    seasonFixtures.clear();
    
    // Generate round-robin fixtures
    for (size_t i = 0; i < iplTeams.size(); i++) {
        for (size_t j = i + 1; j < iplTeams.size(); j++) {
            // Home match
            Match match1;
            match1.team1 = iplTeams[i].name;
            match1.team2 = iplTeams[j].name;
            match1.venue = iplTeams[i].homeGround;
            match1.date = "Match " + std::to_string(seasonFixtures.size() + 1);
            match1.isPlayed = false;
            seasonFixtures.push_back(match1);
            
            // Away match
            Match match2;
            match2.team1 = iplTeams[j].name;
            match2.team2 = iplTeams[i].name;
            match2.venue = iplTeams[j].homeGround;
            match2.date = "Match " + std::to_string(seasonFixtures.size() + 1);
            match2.isPlayed = false;
            seasonFixtures.push_back(match2);
        }
    }
    
    // Shuffle fixtures for randomness
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(seasonFixtures.begin(), seasonFixtures.end(), g);
}

void IPLManager::simulateMatch(Match& match) {
    // Simple match simulation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> scoreDist(120, 200);
    std::uniform_int_distribution<> resultDist(0, 1);
    
    match.team1Score = scoreDist(gen);
    match.team2Score = scoreDist(gen);
    match.isPlayed = true;
    
    if (match.team1Score > match.team2Score) {
        match.winner = match.team1;
    } else if (match.team2Score > match.team1Score) {
        match.winner = match.team2;
    } else {
        match.winner = "Tie";
    }
    
    // Update team statistics
    for (auto& team : iplTeams) {
        if (team.name == match.team1) {
            if (match.winner == match.team1) {
                team.wins++;
                team.points += 2;
            } else if (match.winner == "Tie") {
                team.ties++;
                team.points += 1;
            } else {
                team.losses++;
            }
        } else if (team.name == match.team2) {
            if (match.winner == match.team2) {
                team.wins++;
                team.points += 2;
            } else if (match.winner == "Tie") {
                team.ties++;
                team.points += 1;
            } else {
                team.losses++;
            }
        }
    }
    
    // Update manager statistics
    if (match.team1 == managerProfile.selectedTeam || match.team2 == managerProfile.selectedTeam) {
        managerProfile.totalMatches++;
        if (match.winner == managerProfile.selectedTeam) {
            managerProfile.totalWins++;
        } else if (match.winner != "Tie") {
            managerProfile.totalLosses++;
        }
        managerProfile.winPercentage = (float)managerProfile.totalWins / managerProfile.totalMatches * 100.0f;
    }
}

void IPLManager::updateLeagueTable() {
    // Calculate NRR (simplified)
    for (auto& team : iplTeams) {
        int totalMatches = team.wins + team.losses + team.ties;
        if (totalMatches > 0) {
            team.netRunRate = (float)(team.wins - team.losses) / totalMatches;
        }
    }
}

void IPLManager::calculatePlayoffs() {
    // Sort teams for playoffs
    std::sort(iplTeams.begin(), iplTeams.end(), 
              [](const IPLTeam& a, const IPLTeam& b) {
                  if (a.points != b.points) return a.points > b.points;
                  return a.netRunRate > b.netRunRate;
              });
}

// Utility Methods
void IPLManager::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void IPLManager::printHeader() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    IPL CRICKET MANAGER 2025                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

void IPLManager::printFooter() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Press Enter to continue...                                  ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

void IPLManager::printCentered(const std::string& text) {
    int width = 60;
    int padding = (width - text.length()) / 2;
    std::cout << std::string(padding, ' ') << text << std::endl;
}

void IPLManager::printBanner(const std::string& title) {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║" << std::left << std::setw(58) << title << "║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

void IPLManager::waitForInput() {
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

std::string IPLManager::getInput() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

// API Integration
size_t IPLManager::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string IPLManager::fetchFromAPI(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            return "";
        }
    }
    
    return readBuffer;
}

void IPLManager::parsePlayerData(const json& data) {
    // Parse player data from API response
    // This would be implemented based on the actual API response format
}

// Add new method for auction simulation
void IPLManager::simulateAuction() {
    std::cout << "\n🎯 Simulating auction for all teams...\n";
    std::cout << "This may take a moment...\n\n";
    
    // Create a copy of available players for auction
    std::vector<IPLPlayer> auctionPool = availablePlayers;
    
    // Shuffle players for randomness
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(auctionPool.begin(), auctionPool.end(), g);
    
    // Simulate auction for each player
    for (const auto& player : auctionPool) {
        // Find eligible teams (budget, squad size, overseas limits)
        std::vector<AITeam*> eligibleTeams;
        for (auto& ai : aiTeams) {
            if (ai.budget >= player.price && ai.squad.size() < 25 && 
                (player.nationality == "Indian" || ai.overseasCount < 8)) {
                eligibleTeams.push_back(&ai);
            }
        }
        
        if (eligibleTeams.empty()) {
            std::cout << "No eligible teams for " << player.name << " - Player unsold\n";
            continue;
        }
        
        // Simulate bidding
        float currentBid = player.price;
        AITeam* winner = nullptr;
        const float MAX_BID = 30.0f;
        
        while (currentBid < MAX_BID && eligibleTeams.size() > 1) {
            // Remove teams that can't afford next bid
            eligibleTeams.erase(
                std::remove_if(eligibleTeams.begin(), eligibleTeams.end(),
                    [currentBid](AITeam* team) {
                        return team->budget < currentBid + 0.5f;
                    }),
                eligibleTeams.end()
            );
            
            if (eligibleTeams.size() <= 1) break;
            
            // Each eligible team has a chance to bid based on their strategy
            std::vector<AITeam*> bidders;
            for (auto* team : eligibleTeams) {
                float value = (player.battingRating + player.bowlingRating + player.fieldingRating) / 3.0f;
                float maxBid = player.price;
                
                switch (team->strategy) {
                    case AIStrategy::AGGRESSIVE:
                        maxBid = std::min(value * 1.5f, MAX_BID);
                        break;
                    case AIStrategy::BALANCED:
                        maxBid = std::min(value * 1.2f, MAX_BID);
                        break;
                    case AIStrategy::CONSERVATIVE:
                        maxBid = std::min(value * 1.0f, MAX_BID);
                        break;
                    case AIStrategy::WILDCARD:
                        maxBid = std::min(value * (1.0f + (rand() % 100) / 200.0f), MAX_BID);
                        break;
                }
                
                if (currentBid + 0.5f <= maxBid && team->budget >= currentBid + 0.5f) {
                    if (rand() % 100 < 70) { // 70% chance to bid
                        bidders.push_back(team);
                    }
                }
            }
            
            if (bidders.empty()) break;
            
            // Randomly select a bidder
            std::uniform_int_distribution<> dis(0, bidders.size() - 1);
            winner = bidders[dis(g)];
            currentBid += 0.5f;
            
            // Remove other teams that didn't bid
            eligibleTeams = bidders;
        }
        
        // Award player to winner
        if (winner) {
            winner->squad.push_back(player);
            winner->budget -= currentBid;
            if (player.nationality == "Overseas") winner->overseasCount++;
            
            std::cout << player.name << " → " << winner->team.name << " (₹" << currentBid << " crore)\n";
        }
    }
    
    // Display final squads
    std::cout << "\n🏆 Auction Complete! Final Squads:\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Team                Squad  Overseas  Budget    Strategy     ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    for (const auto& ai : aiTeams) {
        std::string strategyName;
        switch (ai.strategy) {
            case AIStrategy::AGGRESSIVE: strategyName = "Aggressive"; break;
            case AIStrategy::BALANCED: strategyName = "Balanced"; break;
            case AIStrategy::CONSERVATIVE: strategyName = "Conservative"; break;
            case AIStrategy::WILDCARD: strategyName = "Wildcard"; break;
        }
        
        std::string marker = (ai.team.name == managerProfile.selectedTeam) ? "▶ " : "  ";
        std::cout << "║  " << marker << std::left << std::setw(18) << ai.team.name
                  << std::setw(7) << ai.squad.size()
                  << std::setw(10) << ai.overseasCount
                  << std::setw(10) << std::fixed << std::setprecision(1) << ai.budget
                  << std::setw(12) << strategyName << " ║\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // Mark auction as complete
    auctionComplete = true;
    std::cout << "\nType 'continue' to proceed to the season." << std::endl;
    std::cout << "Enter your choice: ";
}

void IPLManager::showAvailablePlayers() {
    printBanner("📊 AVAILABLE PLAYERS");
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Name                Role      Nationality  Price   Rating  ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    for (const auto& player : availablePlayers) {
        float avgRating = (player.battingRating + player.bowlingRating + player.fieldingRating) / 3.0f;
        std::cout << "║  " << std::left << std::setw(20) << player.name
                  << std::setw(10) << player.role
                  << std::setw(13) << player.nationality
                  << std::setw(8) << std::fixed << std::setprecision(1) << player.price
                  << std::setw(8) << std::fixed << std::setprecision(1) << avgRating << " ║\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\nTotal available players: " << availablePlayers.size() << "\n";
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void IPLManager::showCurrentSquad() {
    printBanner("👥 CURRENT SQUAD");
    std::cout << "\n";
    
    // Find user's team
    AITeam* userTeam = nullptr;
    for (auto& ai : aiTeams) {
        if (ai.team.name == managerProfile.selectedTeam) {
            userTeam = &ai;
            break;
        }
    }
    
    if (!userTeam) {
        std::cout << "Team not found!\n";
        return;
    }
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << std::left << std::setw(50) << managerProfile.selectedTeam << "║\n";
    std::cout << "║  Budget: ₹" << std::fixed << std::setprecision(1) << std::setw(8) << userTeam->budget << " crore";
    std::cout << "  Squad: " << userTeam->squad.size() << "/25";
    std::cout << "  Overseas: " << userTeam->overseasCount << "/8" << std::setw(15) << " ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Name                Role      Nationality  Price   Rating  ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    for (const auto& player : userTeam->squad) {
        float avgRating = (player.battingRating + player.bowlingRating + player.fieldingRating) / 3.0f;
        std::cout << "║  " << std::left << std::setw(20) << player.name
                  << std::setw(10) << player.role
                  << std::setw(13) << player.nationality
                  << std::setw(8) << std::fixed << std::setprecision(1) << player.price
                  << std::setw(8) << std::fixed << std::setprecision(1) << avgRating << " ║\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void IPLManager::manualAuction() {
    // Implementation of manual auction functionality
    std::cout << "Manual auction functionality not implemented yet.\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

int main(int argc, char* argv[]) {
    try {
        // Create and initialize the IPL Manager
        IPLManager game;
        
        if (!game.initialize()) {
            std::cerr << "Failed to initialize IPL Cricket Manager!" << std::endl;
            return -1;
        }
        
        // Run the game
        game.run();
        
        std::cout << "IPL Cricket Manager exited successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return -1;
    }
} 