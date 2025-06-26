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
    std::string battingApproach; // Aggressive, Defensive, Balanced, Attacking
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
    
    // Auction state
    bool auctionComplete = false;
    
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
    
    // Avatar and Logo helpers
    std::string getTeamLogo(const std::string& teamName);
    std::string getManagerAvatar();
    
    // New method for simulating entire season
    void simulateEntireSeason();
    
    // Simulate a Super Over between two teams
    std::string simulateSuperOver(const std::string& team1, const std::string& team2);
    
    // Helper: Get available bowlers for a team (max 4 overs per bowler)
    std::vector<IPLPlayer*> getAvailableBowlers(AITeam& team, std::map<std::string, int>& bowlerOvers);
    
    // Helper: Create batting order based on batting approaches
    std::vector<IPLPlayer*> createBattingOrder(AITeam& team);
    
    // Helper: Auto-simulate matches not involving manager's team
    void autoSimulateOtherMatches();
    
    // Helper: Show squad with detailed information
    void showDetailedSquad();
    
    // Helper: Select player using arrow keys
    IPLPlayer* selectPlayerWithArrows(const std::vector<IPLPlayer*>& players, const std::string& title);
    
    // Helper: Get arrow key input
    int getArrowKeyInput();
    
    // Helper: Validate team squad requirements
    bool validateSquadRequirements(const AITeam& team);
    
    // Helper: Get team squad statistics
    struct SquadStats {
        int totalPlayers;
        int wicketKeepers;
        int allRounders;
        int bowlers;
        int batsmen;
        int indians;
        int overseas;
    };
    SquadStats getSquadStats(const AITeam& team);
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
            if (input == "1") {
                currentState = GameState::AVATAR_CUSTOMIZATION;
            } else if (input == "2") {
                currentState = GameState::SETTINGS;
            } else if (input == "3") {
                currentState = GameState::EXIT;
                isRunning = false;
            }
            break;
            
        case GameState::AVATAR_CUSTOMIZATION:
            if (input == "1") {
                std::cout << "Enter your manager name: ";
                std::getline(std::cin, managerProfile.name);
            } else if (input == "2") {
                std::cout << "Select avatar (1-5): ";
                std::string avatarChoice = getInput();
                int choice = std::stoi(avatarChoice);
                if (choice >= 1 && choice <= 5) {
                    managerProfile.avatar = "Avatar_" + std::to_string(choice);
                }
            } else if (input == "3") {
                if (!managerProfile.name.empty()) {
                    currentState = GameState::TEAM_SELECTION;
                } else {
                    std::cout << "Please enter your name first!" << std::endl;
                    waitForInput();
                }
            } else if (input == "0") {
                currentState = GameState::MAIN_MENU;
            }
            break;
            
        case GameState::TEAM_SELECTION:
            if (input == "0") {
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
            if (input == "0") {
                currentState = GameState::TEAM_SELECTION;
            } else if (input == "5") {
                if (auctionComplete) {
                    currentState = GameState::SEASON_CALENDAR;
                    generateSeasonFixtures();
                    seasonInProgress = true;
                } else {
                    std::cout << "Please complete the auction first!" << std::endl;
                    waitForInput();
                }
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
            } else if (input == "6") {
                simulateEntireSeason();
            }
            break;
            
        case GameState::SEASON_CALENDAR:
            if (input == "0") {
                currentState = GameState::AUCTION;
            } else if (input == "1") {
                currentState = GameState::LEAGUE_TABLE;
            } else if (input == "2") {
                currentState = GameState::MATCH_SIMULATION;
            } else if (input == "3") {
                showDetailedSquad();
            } else if (input == "4") {
                if (currentMatchIndex < seasonFixtures.size()) {
                    currentState = GameState::MATCH_SIMULATION;
                } else {
                    currentState = GameState::PLAYOFFS;
                }
            }
            break;
            
        case GameState::MATCH_SIMULATION:
            if (input == "0") {
                currentState = GameState::SEASON_CALENDAR;
            } else if (input == "1") {
                if (currentMatchIndex < seasonFixtures.size()) {
                    simulateMatch(seasonFixtures[currentMatchIndex]);
                    currentMatchIndex++;
                    updateLeagueTable();
                    
                    // Auto-simulate other matches that don't involve the manager's team
                    autoSimulateOtherMatches();
                }
            }
            break;
            
        case GameState::LEAGUE_TABLE:
            if (input == "0") {
                currentState = GameState::SEASON_CALENDAR;
            }
            break;
            
        case GameState::PLAYOFFS:
            if (input == "0") {
                currentState = GameState::SEASON_CALENDAR;
            } else if (input == "1") {
                currentState = GameState::CAREER_SUMMARY;
            }
            break;
            
        case GameState::CAREER_SUMMARY:
            if (input == "0") {
                currentState = GameState::MAIN_MENU;
            } else if (input == "1") {
                // Start new season
                managerProfile.careerYear++;
                currentMatchIndex = 0;
                seasonInProgress = false;
                currentState = GameState::SEASON_CALENDAR;
            }
            break;
            
        case GameState::SETTINGS:
            if (input == "0") {
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
    std::cout << "║  1. Start Career                                             ║\n";
    std::cout << "║  2. Settings                                                 ║\n";
    std::cout << "║  3. Quit Game                                                ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice (1-3): ";
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
    std::cout << "║  1. Set Name                                                ║\n";
    std::cout << "║  2. Select Avatar                                           ║\n";
    std::cout << "║  3. Continue to Team Selection                              ║\n";
    std::cout << "║  0. Go Back                                                 ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice (0-3): ";
}

void IPLManager::showTeamSelection() {
    printBanner("🏏 TEAM SELECTION");
    std::cout << "\n";
    printCentered("Choose your IPL team for the 2025 season!\n");
    
    // Display team logos and information
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Available Teams:                                            ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    for (size_t i = 0; i < iplTeams.size(); i++) {
        std::string logo = getTeamLogo(iplTeams[i].name);
        std::string marker = (managerProfile.selectedTeam == iplTeams[i].name) ? "▶ " : "  ";
        
        std::cout << "║  " << marker << std::left << std::setw(2) << (i + 1) << ". " << std::setw(25) << iplTeams[i].name;
        std::cout << " | " << std::setw(15) << iplTeams[i].city;
        std::cout << " | " << std::setw(8) << std::fixed << std::setprecision(1) << iplTeams[i].budget << "Cr ║\n";
        
        // Display team logo
        if (!logo.empty()) {
            std::cout << "║     " << logo << std::setw(45 - logo.length()) << " ║\n";
        }
        
        if (i < iplTeams.size() - 1) {
            std::cout << "║                                                                  ║\n";
        }
    }
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\n";
    std::cout << "Enter team number (1-" << iplTeams.size() << ") or 0 to return: ";
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
    std::cout << "║  1. Manual Auction (Bid for each player)                     ║\n";
    std::cout << "║  2. Simulate Auction (Auto-build all teams)                  ║\n";
    std::cout << "║  3. View Available Players                                   ║\n";
    std::cout << "║  4. View Current Squad                                       ║\n";
    std::cout << "║  5. Continue to Season                                       ║\n";
    std::cout << "║  0. Go Back                                                  ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  6. Simulate Entire Season (Show only final scores)          ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice (0-6): ";
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
    std::cout << "║  1. View League Table                                        ║\n";
    std::cout << "║  2. Simulate Next Match                                      ║\n";
    std::cout << "║  3. View Detailed Squad                                      ║\n";
    std::cout << "║  4. Continue Season                                          ║\n";
    std::cout << "║  0. Go Back                                                  ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Enter your choice (0-4): ";
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
        std::cout << "║  1. Continue to Playoffs                                    ║\n";
        std::cout << "║  0. Go Back                                                 ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    }
    
    std::cout << "\n";
    std::cout << "Enter your choice (0-1): ";
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
    // IPL teams
    std::vector<std::string> teams = {
        "Mumbai Indians", "Chennai Super Kings", "Royal Challengers Bangalore", "Kolkata Knight Riders", "Delhi Capitals", "Punjab Kings", "Rajasthan Royals", "Sunrisers Hyderabad", "Gujarat Titans", "Lucknow Super Giants"
    };
    // Example Indian and overseas names (expand as needed)
    std::vector<std::string> indianNames = {
        "Rohit Sharma", "Ishan Kishan", "Suryakumar Yadav", "Tilak Varma", "Nehal Wadhera", "Shivam Dube", "Ruturaj Gaikwad", "Ajinkya Rahane", "Prithvi Shaw", "Shubman Gill", "KL Rahul", "Mayank Agarwal", "Rinku Singh", "Nitish Rana", "Venkatesh Iyer", "Devdutt Padikkal", "Rajat Patidar", "Abhishek Sharma", "Yashasvi Jaiswal", "Sanju Samson", "Ravindra Jadeja", "Deepak Chahar", "Jasprit Bumrah", "Harshal Patel", "Mohammed Siraj", "Umesh Yadav", "Tushar Deshpande", "Arshdeep Singh", "Avesh Khan", "Mukesh Kumar", "Yash Dayal", "Varun Chakravarthy", "Yuzvendra Chahal", "Krunal Pandya", "Washington Sundar", "Navdeep Saini", "Shahbaz Ahmed", "Kuldeep Yadav", "Axar Patel", "Simarjeet Singh", "Rajvardhan Hangargekar", "Prashant Solanki", "Ajay Mandal", "Bhagath Varma", "Nishant Sindhu", "Shaik Rasheed", "Subhranshu Senapati", "Kumar Kartikeya", "Piyush Chawla", "Arjun Tendulkar", "Raghav Goyal"
    };
    std::vector<std::string> overseasNames = {
        "Tim David", "Cameron Green", "Dewald Brevis", "Tristan Stubbs", "Devon Conway", "Ben Stokes", "Moeen Ali", "Mitchell Santner", "Kyle Jamieson", "Faf du Plessis", "Glenn Maxwell", "Josh Hazlewood", "Wanindu Hasaranga", "David Warner", "Mitchell Marsh", "Rilee Rossouw", "Phil Salt", "Liam Livingstone", "Sam Curran", "Kagiso Rabada", "Jonny Bairstow", "Jos Buttler", "Trent Boult", "Shimron Hetmyer", "Obed McCoy", "Rashid Khan", "David Miller", "Matthew Wade", "Alzarri Joseph", "Lockie Ferguson", "Kane Williamson", "Nicholas Pooran", "Quinton de Kock", "Marcus Stoinis", "Mark Wood", "Kyle Mayers", "Aiden Markram", "Heinrich Klaasen", "Marco Jansen", "Pat Cummins", "Harry Brook", "Rahmanullah Gurbaz", "Andre Russell", "Sunil Narine", "Tim Southee", "Jason Roy", "Dwaine Pretorius", "Sisanda Magala", "Chris Jordan", "Jofra Archer", "Riley Meredith"
    };
    std::vector<std::string> roles = {"Batsman", "Bowler", "All-rounder", "Wicket-keeper"};
    std::vector<std::string> nationalities = {"Indian", "Overseas"};
    std::vector<std::string> battingApproaches = {"Aggressive", "Defensive", "Balanced", "Attacking"};
    
    // Clear previous
    availablePlayers.clear();
    srand(42); // For reproducibility
    int indianIdx = 0, overseasIdx = 0;
    for (const auto& team : teams) {
        // 14 Indian players
        for (int i = 0; i < 14; ++i) {
            IPLPlayer player;
            player.name = indianNames[indianIdx % indianNames.size()] + " " + std::to_string(i+1);
            player.team = team;
            player.role = roles[i % roles.size()];
            player.nationality = "Indian";
            player.battingRating = 55 + rand() % 45; // 55-99
            player.bowlingRating = 40 + rand() % 60; // 40-99
            player.fieldingRating = 50 + rand() % 50; // 50-99
            player.price = 4 + (rand() % 25); // 4-28 crore
            player.age = 20 + (rand() % 17); // 20-36
            
            // Assign batting approach based on role and batting rating
            if (player.role == "Batsman") {
                if (player.battingRating > 85) {
                    player.battingApproach = "Aggressive";
                } else if (player.battingRating > 75) {
                    player.battingApproach = "Attacking";
                } else if (player.battingRating > 65) {
                    player.battingApproach = "Balanced";
                } else {
                    player.battingApproach = "Defensive";
                }
            } else if (player.role == "All-rounder") {
                if (player.battingRating > 80) {
                    player.battingApproach = "Attacking";
                } else if (player.battingRating > 70) {
                    player.battingApproach = "Balanced";
                } else {
                    player.battingApproach = "Defensive";
                }
            } else if (player.role == "Wicket-keeper") {
                if (player.battingRating > 80) {
                    player.battingApproach = "Aggressive";
                } else if (player.battingRating > 70) {
                    player.battingApproach = "Balanced";
                } else {
                    player.battingApproach = "Defensive";
                }
            } else { // Bowler
                player.battingApproach = "Defensive";
            }
            
            availablePlayers.push_back(player);
            ++indianIdx;
        }
        // 8 Overseas players
        for (int i = 0; i < 8; ++i) {
            IPLPlayer player;
            player.name = overseasNames[overseasIdx % overseasNames.size()] + " " + std::to_string(i+1);
            player.team = team;
            player.role = roles[(i+1) % roles.size()];
            player.nationality = "Overseas";
            player.battingRating = 60 + rand() % 40; // 60-99
            player.bowlingRating = 45 + rand() % 55; // 45-99
            player.fieldingRating = 55 + rand() % 45; // 55-99
            player.price = 6 + (rand() % 25); // 6-30 crore
            player.age = 22 + (rand() % 15); // 22-36
            
            // Assign batting approach based on role and batting rating
            if (player.role == "Batsman") {
                if (player.battingRating > 85) {
                    player.battingApproach = "Aggressive";
                } else if (player.battingRating > 75) {
                    player.battingApproach = "Attacking";
                } else if (player.battingRating > 65) {
                    player.battingApproach = "Balanced";
                } else {
                    player.battingApproach = "Defensive";
                }
            } else if (player.role == "All-rounder") {
                if (player.battingRating > 80) {
                    player.battingApproach = "Attacking";
                } else if (player.battingRating > 70) {
                    player.battingApproach = "Balanced";
                } else {
                    player.battingApproach = "Defensive";
                }
            } else if (player.role == "Wicket-keeper") {
                if (player.battingRating > 80) {
                    player.battingApproach = "Aggressive";
                } else if (player.battingRating > 70) {
                    player.battingApproach = "Balanced";
                } else {
                    player.battingApproach = "Defensive";
                }
            } else { // Bowler
                player.battingApproach = "Defensive";
            }
            
            availablePlayers.push_back(player);
            ++overseasIdx;
        }
    }
    std::cout << "Loaded " << availablePlayers.size() << " players into auction pool.\n";
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
    // Over-by-over simulation with bowler selection for manager
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> runsDist(0, 6); // 0-6 runs per ball
    std::uniform_int_distribution<> wicketDist(0, 19); // 0 = wicket (5% chance)
    int overs = 20;
    int ballsPerOver = 6;
    int team1Score = 0, team2Score = 0;
    int team1Wickets = 0, team2Wickets = 0;
    std::vector<std::string> commentary;
    
    // Find AITeam objects
    AITeam* team1AI = nullptr;
    AITeam* team2AI = nullptr;
    for (auto& ai : aiTeams) {
        if (ai.team.name == match.team1) team1AI = &ai;
        if (ai.team.name == match.team2) team2AI = &ai;
    }
    
    // Create batting orders based on batting approaches
    std::vector<IPLPlayer*> team1BattingOrder;
    std::vector<IPLPlayer*> team2BattingOrder;
    if (team1AI) team1BattingOrder = createBattingOrder(*team1AI);
    if (team2AI) team2BattingOrder = createBattingOrder(*team2AI);
    
    // Track overs per bowler
    std::map<std::string, int> team1BowlerOvers, team2BowlerOvers;
    
    // Team 1 batting
    std::cout << "\n" << match.team1 << " Innings:\n";
    int team1BatterIndex = 0;
    for (int over = 1; over <= overs && team1Wickets < 10; ++over) {
        IPLPlayer* bowler = nullptr;
        // Bowler selection: if manager is bowling
        if (match.team2 == managerProfile.selectedTeam && team2AI) {
            auto bowlers = getAvailableBowlers(*team2AI, team2BowlerOvers);
            bowler = selectPlayerWithArrows(bowlers, "Select Bowler for Over " + std::to_string(over));
            if (bowler) {
                team2BowlerOvers[bowler->name]++;
            } else {
                // Default to first available bowler if selection cancelled
                if (!bowlers.empty()) {
                    bowler = bowlers[0];
                    team2BowlerOvers[bowler->name]++;
                }
            }
        } else if (team2AI) {
            // AI selects bowler
            auto bowlers = getAvailableBowlers(*team2AI, team2BowlerOvers);
            if (!bowlers.empty()) {
                bowler = bowlers[over % bowlers.size()];
                team2BowlerOvers[bowler->name]++;
            }
        }
        
        int overRuns = 0, overWickets = 0;
        for (int ball = 1; ball <= ballsPerOver && team1Wickets < 10; ++ball) {
            // Get current batter
            IPLPlayer* currentBatter = nullptr;
            if (team1BatterIndex < team1BattingOrder.size()) {
                currentBatter = team1BattingOrder[team1BatterIndex];
            }
            
            int runs = runsDist(gen);
            bool wicket = (wicketDist(gen) == 0);
            
            // Adjust runs based on batting approach
            if (currentBatter) {
                if (currentBatter->battingApproach == "Aggressive") {
                    // Aggressive batters are more likely to hit boundaries but also get out
                    if (runs < 4) runs = std::max(0, runs - 1);
                    if (runs >= 4) runs = std::min(6, runs + 1);
                    if (wicketDist(gen) < 3) wicket = true; // Higher chance of getting out
                } else if (currentBatter->battingApproach == "Attacking") {
                    // Attacking batters are good at rotating strike and hitting boundaries
                    if (runs == 0) runs = 1; // Less likely to get dot balls
                    if (runs >= 4) runs = std::min(6, runs + 1);
                } else if (currentBatter->battingApproach == "Balanced") {
                    // Balanced batters are consistent
                    // No special adjustments
                } else if (currentBatter->battingApproach == "Defensive") {
                    // Defensive batters are less likely to get out but score fewer runs
                    if (runs > 4) runs = std::max(1, runs - 2);
                    if (wicketDist(gen) > 15) wicket = false; // Lower chance of getting out
                }
                
                // Adjust based on batting rating
                if (currentBatter->battingRating > 80 && runs > 0) runs += 1;
                if (currentBatter->battingRating < 60 && runs > 2) runs -= 1;
            }
            
            // Bowler skill: reduce runs, increase wicket chance
            if (bowler) {
                if (bowler->bowlingRating > 80 && runs > 0) runs -= 1;
                if (bowler->bowlingRating > 90 && runs > 0) runs -= 1;
                if (bowler->bowlingRating > 80 && wicketDist(gen) < 2) wicket = true;
            }
            
            if (runs < 0) runs = 0;
            overRuns += runs;
            team1Score += runs;
            
            std::string event = "";
            if (wicket) {
                team1Wickets++;
                overWickets++;
                team1BatterIndex++; // Next batter
                if (currentBatter) {
                    std::vector<std::string> wicketCommentary = {
                        "WICKET! " + currentBatter->name + " is clean bowled!",
                        "WICKET! " + currentBatter->name + " edges it to the keeper!",
                        "WICKET! " + currentBatter->name + " caught at mid-wicket!",
                        "WICKET! " + currentBatter->name + " LBW! That's plumb!",
                        "WICKET! " + currentBatter->name + " stumped! Brilliant work by the keeper!",
                        "WICKET! " + currentBatter->name + " run out! What a mix-up!",
                        "WICKET! " + currentBatter->name + " caught at deep mid-wicket!",
                        "WICKET! " + currentBatter->name + " bowled around the legs!"
                    };
                    event = wicketCommentary[rand() % wicketCommentary.size()] + " ";
                } else {
                    event = "WICKET! ";
                }
            }
            if (runs == 4) {
                if (currentBatter) {
                    std::vector<std::string> fourCommentary = {
                        "FOUR! " + currentBatter->name + " drives it beautifully through the covers!",
                        "FOUR! " + currentBatter->name + " cuts it past point for a boundary!",
                        "FOUR! " + currentBatter->name + " pulls it to the mid-wicket boundary!",
                        "FOUR! " + currentBatter->name + " flicks it fine for four!",
                        "FOUR! " + currentBatter->name + " square drives it to the boundary!",
                        "FOUR! " + currentBatter->name + " plays a lovely shot through extra cover!",
                        "FOUR! " + currentBatter->name + " hits it over the bowler's head!",
                        "FOUR! " + currentBatter->name + " guides it past third man!"
                    };
                    event += fourCommentary[rand() % fourCommentary.size()] + " ";
                } else {
                    event += "FOUR! ";
                }
            }
            if (runs == 6) {
                if (currentBatter) {
                    std::vector<std::string> sixCommentary = {
                        "SIX! " + currentBatter->name + " launches it over long-on for a maximum!",
                        "SIX! " + currentBatter->name + " smashes it over mid-wicket!",
                        "SIX! " + currentBatter->name + " hits it over the covers for six!",
                        "SIX! " + currentBatter->name + " pulls it over square leg!",
                        "SIX! " + currentBatter->name + " drives it straight down the ground!",
                        "SIX! " + currentBatter->name + " scoops it over fine leg!",
                        "SIX! " + currentBatter->name + " reverse sweeps it over point!",
                        "SIX! " + currentBatter->name + " hits it over long-off!"
                    };
                    event += sixCommentary[rand() % sixCommentary.size()] + " ";
                } else {
                    event += "SIX! ";
                }
            }
            if (runs == 1) {
                if (currentBatter) {
                    std::vector<std::string> singleCommentary = {
                        currentBatter->name + " takes a quick single.",
                        currentBatter->name + " works it to mid-wicket for one.",
                        currentBatter->name + " pushes it to cover for a single.",
                        currentBatter->name + " taps it to point for one run."
                    };
                    if (rand() % 10 < 3) { // 30% chance for single commentary
                        event += singleCommentary[rand() % singleCommentary.size()] + " ";
                    }
                }
            }
            if (runs == 2) {
                if (currentBatter) {
                    std::vector<std::string> twoCommentary = {
                        currentBatter->name + " runs hard for two.",
                        currentBatter->name + " places it in the gap for a couple.",
                        currentBatter->name + " works it to deep square leg for two."
                    };
                    if (rand() % 10 < 4) { // 40% chance for two commentary
                        event += twoCommentary[rand() % twoCommentary.size()] + " ";
                    }
                }
            }
            if (runs == 3) {
                if (currentBatter) {
                    std::vector<std::string> threeCommentary = {
                        currentBatter->name + " runs three! Good running between the wickets.",
                        currentBatter->name + " places it perfectly for three runs."
                    };
                    if (rand() % 10 < 5) { // 50% chance for three commentary
                        event += threeCommentary[rand() % threeCommentary.size()] + " ";
                    }
                }
            }
            if (runs == 0) {
                if (currentBatter) {
                    std::vector<std::string> dotCommentary = {
                        currentBatter->name + " defends it solidly.",
                        currentBatter->name + " leaves it alone.",
                        currentBatter->name + " blocks it back to the bowler.",
                        currentBatter->name + " plays it to mid-off.",
                        currentBatter->name + " lets it go through to the keeper."
                    };
                    if (rand() % 10 < 2) { // 20% chance for dot commentary
                        event += dotCommentary[rand() % dotCommentary.size()] + " ";
                    }
                }
            }
            if (!event.empty()) {
                commentary.push_back("Over " + std::to_string(over) + "." + std::to_string(ball) + ": " + event);
            }
        }
        std::cout << "Over " << over << ": " << overRuns << " runs, " << overWickets << " wickets. Total: " << team1Score << "/" << team1Wickets << "\n";
    }
    std::cout << "End of Innings: " << match.team1 << " " << team1Score << "/" << team1Wickets << "\n";
    
    // Team 2 batting
    std::cout << "\n" << match.team2 << " Innings:\n";
    int team2BatterIndex = 0;
    for (int over = 1; over <= overs && team2Wickets < 10 && team2Score <= team1Score; ++over) {
        IPLPlayer* bowler = nullptr;
        // Bowler selection: if manager is bowling
        if (match.team1 == managerProfile.selectedTeam && team1AI) {
            auto bowlers = getAvailableBowlers(*team1AI, team1BowlerOvers);
            bowler = selectPlayerWithArrows(bowlers, "Select Bowler for Over " + std::to_string(over));
            if (bowler) {
                team1BowlerOvers[bowler->name]++;
            } else {
                // Default to first available bowler if selection cancelled
                if (!bowlers.empty()) {
                    bowler = bowlers[0];
                    team1BowlerOvers[bowler->name]++;
                }
            }
        } else if (team1AI) {
            // AI selects bowler
            auto bowlers = getAvailableBowlers(*team1AI, team1BowlerOvers);
            if (!bowlers.empty()) {
                bowler = bowlers[over % bowlers.size()];
                team1BowlerOvers[bowler->name]++;
            }
        }
        
        int overRuns = 0, overWickets = 0;
        for (int ball = 1; ball <= ballsPerOver && team2Wickets < 10 && team2Score <= team1Score; ++ball) {
            // Get current batter
            IPLPlayer* currentBatter = nullptr;
            if (team2BatterIndex < team2BattingOrder.size()) {
                currentBatter = team2BattingOrder[team2BatterIndex];
            }
            
            int runs = runsDist(gen);
            bool wicket = (wicketDist(gen) == 0);
            
            // Adjust runs based on batting approach
            if (currentBatter) {
                if (currentBatter->battingApproach == "Aggressive") {
                    // Aggressive batters are more likely to hit boundaries but also get out
                    if (runs < 4) runs = std::max(0, runs - 1);
                    if (runs >= 4) runs = std::min(6, runs + 1);
                    if (wicketDist(gen) < 3) wicket = true; // Higher chance of getting out
                } else if (currentBatter->battingApproach == "Attacking") {
                    // Attacking batters are good at rotating strike and hitting boundaries
                    if (runs == 0) runs = 1; // Less likely to get dot balls
                    if (runs >= 4) runs = std::min(6, runs + 1);
                } else if (currentBatter->battingApproach == "Balanced") {
                    // Balanced batters are consistent
                    // No special adjustments
                } else if (currentBatter->battingApproach == "Defensive") {
                    // Defensive batters are less likely to get out but score fewer runs
                    if (runs > 4) runs = std::max(1, runs - 2);
                    if (wicketDist(gen) > 15) wicket = false; // Lower chance of getting out
                }
                
                // Adjust based on batting rating
                if (currentBatter->battingRating > 80 && runs > 0) runs += 1;
                if (currentBatter->battingRating < 60 && runs > 2) runs -= 1;
            }
            
            // Bowler skill: reduce runs, increase wicket chance
            if (bowler) {
                if (bowler->bowlingRating > 80 && runs > 0) runs -= 1;
                if (bowler->bowlingRating > 90 && runs > 0) runs -= 1;
                if (bowler->bowlingRating > 80 && wicketDist(gen) < 2) wicket = true;
            }
            
            if (runs < 0) runs = 0;
            overRuns += runs;
            team2Score += runs;
            
            std::string event = "";
            if (wicket) {
                team2Wickets++;
                overWickets++;
                team2BatterIndex++; // Next batter
                if (currentBatter) {
                    std::vector<std::string> wicketCommentary = {
                        "WICKET! " + currentBatter->name + " is clean bowled!",
                        "WICKET! " + currentBatter->name + " edges it to the keeper!",
                        "WICKET! " + currentBatter->name + " caught at mid-wicket!",
                        "WICKET! " + currentBatter->name + " LBW! That's plumb!",
                        "WICKET! " + currentBatter->name + " stumped! Brilliant work by the keeper!",
                        "WICKET! " + currentBatter->name + " run out! What a mix-up!",
                        "WICKET! " + currentBatter->name + " caught at deep mid-wicket!",
                        "WICKET! " + currentBatter->name + " bowled around the legs!"
                    };
                    event = wicketCommentary[rand() % wicketCommentary.size()] + " ";
                } else {
                    event = "WICKET! ";
                }
            }
            if (runs == 4) {
                if (currentBatter) {
                    std::vector<std::string> fourCommentary = {
                        "FOUR! " + currentBatter->name + " drives it beautifully through the covers!",
                        "FOUR! " + currentBatter->name + " cuts it past point for a boundary!",
                        "FOUR! " + currentBatter->name + " pulls it to the mid-wicket boundary!",
                        "FOUR! " + currentBatter->name + " flicks it fine for four!",
                        "FOUR! " + currentBatter->name + " square drives it to the boundary!",
                        "FOUR! " + currentBatter->name + " plays a lovely shot through extra cover!",
                        "FOUR! " + currentBatter->name + " hits it over the bowler's head!",
                        "FOUR! " + currentBatter->name + " guides it past third man!"
                    };
                    event += fourCommentary[rand() % fourCommentary.size()] + " ";
                } else {
                    event += "FOUR! ";
                }
            }
            if (runs == 6) {
                if (currentBatter) {
                    std::vector<std::string> sixCommentary = {
                        "SIX! " + currentBatter->name + " launches it over long-on for a maximum!",
                        "SIX! " + currentBatter->name + " smashes it over mid-wicket!",
                        "SIX! " + currentBatter->name + " hits it over the covers for six!",
                        "SIX! " + currentBatter->name + " pulls it over square leg!",
                        "SIX! " + currentBatter->name + " drives it straight down the ground!",
                        "SIX! " + currentBatter->name + " scoops it over fine leg!",
                        "SIX! " + currentBatter->name + " reverse sweeps it over point!",
                        "SIX! " + currentBatter->name + " hits it over long-off!"
                    };
                    event += sixCommentary[rand() % sixCommentary.size()] + " ";
                } else {
                    event += "SIX! ";
                }
            }
            if (runs == 1) {
                if (currentBatter) {
                    std::vector<std::string> singleCommentary = {
                        currentBatter->name + " takes a quick single.",
                        currentBatter->name + " works it to mid-wicket for one.",
                        currentBatter->name + " pushes it to cover for a single.",
                        currentBatter->name + " taps it to point for one run."
                    };
                    if (rand() % 10 < 3) { // 30% chance for single commentary
                        event += singleCommentary[rand() % singleCommentary.size()] + " ";
                    }
                }
            }
            if (runs == 2) {
                if (currentBatter) {
                    std::vector<std::string> twoCommentary = {
                        currentBatter->name + " runs hard for two.",
                        currentBatter->name + " places it in the gap for a couple.",
                        currentBatter->name + " works it to deep square leg for two."
                    };
                    if (rand() % 10 < 4) { // 40% chance for two commentary
                        event += twoCommentary[rand() % twoCommentary.size()] + " ";
                    }
                }
            }
            if (runs == 3) {
                if (currentBatter) {
                    std::vector<std::string> threeCommentary = {
                        currentBatter->name + " runs three! Good running between the wickets.",
                        currentBatter->name + " places it perfectly for three runs."
                    };
                    if (rand() % 10 < 5) { // 50% chance for three commentary
                        event += threeCommentary[rand() % threeCommentary.size()] + " ";
                    }
                }
            }
            if (runs == 0) {
                if (currentBatter) {
                    std::vector<std::string> dotCommentary = {
                        currentBatter->name + " defends it solidly.",
                        currentBatter->name + " leaves it alone.",
                        currentBatter->name + " blocks it back to the bowler.",
                        currentBatter->name + " plays it to mid-off.",
                        currentBatter->name + " lets it go through to the keeper."
                    };
                    if (rand() % 10 < 2) { // 20% chance for dot commentary
                        event += dotCommentary[rand() % dotCommentary.size()] + " ";
                    }
                }
            }
            if (!event.empty()) {
                commentary.push_back("Over " + std::to_string(over) + "." + std::to_string(ball) + ": " + event);
            }
        }
        std::cout << "Over " << over << ": " << overRuns << " runs, " << overWickets << " wickets. Total: " << team2Score << "/" << team2Wickets << "\n";
        if (team2Score > team1Score) break;
    }
    std::cout << "End of Innings: " << match.team2 << " " << team2Score << "/" << team2Wickets << "\n";
    
    // Print key commentary
    std::cout << "\nKey Moments:\n";
    for (const auto& line : commentary) {
        std::cout << line << "\n";
    }
    
    match.team1Score = team1Score;
    match.team2Score = team2Score;
    match.isPlayed = true;
    if (team1Score > team2Score) {
        match.winner = match.team1;
    } else if (team2Score > team1Score) {
        match.winner = match.team2;
    } else {
        // Tie! Super Over time
        std::cout << "\nMatch tied! A Super Over will decide the winner.\n";
        std::string superOverWinner;
        do {
            superOverWinner = simulateSuperOver(match.team1, match.team2);
            if (superOverWinner == "Tie") {
                std::cout << "Super Over tied! Another Super Over will be played.\n";
            }
        } while (superOverWinner == "Tie");
        match.winner = superOverWinner;
        std::cout << "Super Over Winner: " << superOverWinner << "!\n";
    }
    // Update team and manager stats (unchanged)
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

// Simulate a Super Over between two teams
std::string IPLManager::simulateSuperOver(const std::string& team1, const std::string& team2) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> runDist(0, 6); // Each ball: 0-6 runs
    std::uniform_int_distribution<> wicketDist(0, 9); // 0-1 wicket (10% chance)
    int team1Runs = 0, team2Runs = 0;
    int team1Wickets = 0, team2Wickets = 0;
    std::cout << "\nSuper Over: " << team1 << " batting\n";
    for (int ball = 1; ball <= 6 && team1Wickets < 2; ++ball) {
        int runs = runDist(gen);
        bool wicket = (wicketDist(gen) == 0);
        team1Runs += runs;
        std::cout << "Ball " << ball << ": " << runs << " run(s)";
        if (wicket) {
            team1Wickets++;
            std::cout << " - WICKET!";
        }
        std::cout << "\n";
        if (team1Wickets == 2) break;
    }
    std::cout << "Total: " << team1Runs << "/" << team1Wickets << "\n";
    std::cout << "Super Over: " << team2 << " batting\n";
    for (int ball = 1; ball <= 6 && team2Wickets < 2; ++ball) {
        int runs = runDist(gen);
        bool wicket = (wicketDist(gen) == 0);
        team2Runs += runs;
        std::cout << "Ball " << ball << ": " << runs << " run(s)";
        if (wicket) {
            team2Wickets++;
            std::cout << " - WICKET!";
        }
        std::cout << "\n";
        if (team2Wickets == 2) break;
    }
    std::cout << "Total: " << team2Runs << "/" << team2Wickets << "\n";
    if (team1Runs > team2Runs) return team1;
    if (team2Runs > team1Runs) return team2;
    return "Tie";
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
        // Find eligible teams (budget, squad size, overseas limits, and role requirements)
        std::vector<AITeam*> eligibleTeams;
        for (auto& ai : aiTeams) {
            // Basic eligibility checks
            if (ai.budget < player.price || ai.squad.size() >= 25) {
                continue;
            }
            
            // Overseas player limit check
            if (player.nationality == "Overseas" && ai.overseasCount >= 8) {
                continue;
            }
            
            // Check if team needs this role type
            SquadStats stats = getSquadStats(ai);
            bool needsThisRole = false;
            
            if (player.role == "Wicket-keeper" && stats.wicketKeepers < 1) {
                needsThisRole = true;
            } else if (player.role == "All-rounder" && stats.allRounders < 3) {
                needsThisRole = true;
            } else if (player.role == "Bowler" && stats.bowlers < 5) {
                needsThisRole = true;
            } else if (player.role == "Batsman" && stats.batsmen < 5) {
                needsThisRole = true;
            } else if (stats.totalPlayers < 18) {
                // If team has less than 18 players, they need any player
                needsThisRole = true;
            }
            
            if (needsThisRole) {
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
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Team                Squad  Overseas  Budget    Strategy  WK  AR  Bowl  Bat  Valid  ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    for (const auto& ai : aiTeams) {
        std::string strategyName;
        switch (ai.strategy) {
            case AIStrategy::AGGRESSIVE: strategyName = "Aggressive"; break;
            case AIStrategy::BALANCED: strategyName = "Balanced"; break;
            case AIStrategy::CONSERVATIVE: strategyName = "Conservative"; break;
            case AIStrategy::WILDCARD: strategyName = "Wildcard"; break;
        }
        
        SquadStats stats = getSquadStats(ai);
        bool isValid = validateSquadRequirements(ai);
        std::string validStatus = isValid ? "✓" : "✗";
        
        std::string marker = (ai.team.name == managerProfile.selectedTeam) ? "▶ " : "  ";
        std::cout << "║  " << marker << std::left << std::setw(18) << ai.team.name
                  << std::setw(7) << ai.squad.size()
                  << std::setw(10) << ai.overseasCount
                  << std::setw(10) << std::fixed << std::setprecision(1) << ai.budget
                  << std::setw(12) << strategyName
                  << std::setw(4) << stats.wicketKeepers
                  << std::setw(4) << stats.allRounders
                  << std::setw(6) << stats.bowlers
                  << std::setw(5) << stats.batsmen
                  << std::setw(6) << validStatus << " ║\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    // Check if all teams meet requirements
    bool allTeamsValid = true;
    for (const auto& ai : aiTeams) {
        if (!validateSquadRequirements(ai)) {
            allTeamsValid = false;
            break;
        }
    }
    
    if (allTeamsValid) {
        std::cout << "\n✅ All teams meet the minimum squad requirements!\n";
    } else {
        std::cout << "\n⚠️  Some teams do not meet the minimum squad requirements.\n";
        std::cout << "Minimum requirements: 18+ players, 1+ WK, 3+ AR, 5+ Bowlers, 5+ Batsmen\n";
    }
    
    // Mark auction as complete
    auctionComplete = true;
    std::cout << "\nType 5 to proceed to the season." << std::endl;
    std::cout << "Enter your choice: ";
}

void IPLManager::showAvailablePlayers() {
    printBanner("📊 AVAILABLE PLAYERS");
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Name                Role      Nationality  Approach   Price   Rating          ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";
    
    for (const auto& player : availablePlayers) {
        float avgRating = (player.battingRating + player.bowlingRating + player.fieldingRating) / 3.0f;
        std::cout << "║  " << std::left << std::setw(20) << player.name
                  << std::setw(10) << player.role
                  << std::setw(13) << player.nationality
                  << std::setw(11) << player.battingApproach
                  << std::setw(8) << std::fixed << std::setprecision(1) << player.price
                  << std::setw(8) << std::fixed << std::setprecision(1) << avgRating << " ║\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════╝\n";
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
    
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << std::left << std::setw(50) << managerProfile.selectedTeam << "║\n";
    std::cout << "║  Budget: ₹" << std::fixed << std::setprecision(1) << std::setw(8) << userTeam->budget << " crore";
    std::cout << "  Squad: " << userTeam->squad.size() << "/25";
    std::cout << "  Overseas: " << userTeam->overseasCount << "/8" << std::setw(15) << " ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Name                Role      Nationality  Approach   Price   Rating          ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";
    
    for (const auto& player : userTeam->squad) {
        float avgRating = (player.battingRating + player.bowlingRating + player.fieldingRating) / 3.0f;
        std::cout << "║  " << std::left << std::setw(20) << player.name
                  << std::setw(10) << player.role
                  << std::setw(13) << player.nationality
                  << std::setw(11) << player.battingApproach
                  << std::setw(8) << std::fixed << std::setprecision(1) << player.price
                  << std::setw(8) << std::fixed << std::setprecision(1) << avgRating << " ║\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void IPLManager::manualAuction() {
    printBanner("🎯 MANUAL AUCTION");
    std::cout << "\n";
    
    // User's team
    AITeam* userTeam = nullptr;
    for (auto& ai : aiTeams) {
        if (ai.team.name == managerProfile.selectedTeam) {
            userTeam = &ai;
            break;
        }
    }
    if (!userTeam) return;
    
    // Auction pool (for demo, use availablePlayers)
    static size_t playerIndex = 0;
    if (playerIndex >= availablePlayers.size() || userTeam->squad.size() >= 25) {
        auctionComplete = true;
        std::cout << "Auction complete!\n";
        std::cout << "Type 'continue' to proceed to the season." << std::endl;
        std::cout << "Enter your choice: ";
        return;
    }
    IPLPlayer& player = availablePlayers[playerIndex];
    
    // Show player up for auction
    std::cout << "\nPlayer up for auction: " << player.name << " (" << player.role << ", " << player.nationality << ")\n";
    std::cout << "Batting Approach: " << player.battingApproach << "\n";
    std::cout << "Base Price: ₹" << player.price << " crore\n";
    
    // Bidding
    float currentBid = player.price;
    std::string currentBidder = "None";
    std::map<std::string, bool> activeBidders;
    const float MAX_BID = 30.0f; // Maximum bid limit of ₹30 crore
    
    for (auto& ai : aiTeams) {
        // Only teams with squad < 25 and budget > base price can bid
        if (ai.squad.size() < 25 && ai.budget >= player.price) {
            activeBidders[ai.team.name] = true;
        }
    }
    // User always active if eligible
    if (userTeam->squad.size() < 25 && userTeam->budget >= player.price) {
        activeBidders[userTeam->team.name] = true;
    }
    
    bool userPassed = false;
    int round = 0;
    while (true) {
        bool anyBid = false;
        for (auto& ai : aiTeams) {
            if (!activeBidders[ai.team.name]) continue;
            if (ai.team.name == userTeam->team.name) {
                // User's turn
                if (userPassed) continue;
                std::cout << "\nCurrent bid: ₹" << currentBid << " crore by " << currentBidder << std::endl;
                std::cout << "Your budget: ₹" << userTeam->budget << " crore, Squad: " << userTeam->squad.size() << "/25, Overseas: " << userTeam->overseasCount << "/8" << std::endl;
                
                // Show squad requirements
                SquadStats stats = getSquadStats(*userTeam);
                std::cout << "Squad: WK(" << stats.wicketKeepers << "/1) AR(" << stats.allRounders << "/3) Bowl(" << stats.bowlers << "/5) Bat(" << stats.batsmen << "/5)" << std::endl;
                
                // Check if next bid would exceed maximum
                if (currentBid + 0.5f > MAX_BID) {
                    std::cout << "Maximum bid limit of ₹" << MAX_BID << " crore reached!" << std::endl;
                    userPassed = true;
                    activeBidders[userTeam->team.name] = false;
                    continue;
                }
                
                std::cout << "Do you want to bid ₹" << (currentBid + 0.5f) << " crore? (1=bid, 0=pass): ";
                std::string input;
                std::getline(std::cin, input);
                if (input == "1" && userTeam->budget >= currentBid + 0.5f && userTeam->squad.size() < 25 && (player.nationality == "Indian" || userTeam->overseasCount < 8)) {
                    currentBid += 0.5f;
                    currentBidder = userTeam->team.name;
                    anyBid = true;
                } else {
                    userPassed = true;
                    activeBidders[userTeam->team.name] = false;
                }
            } else {
                // AI's turn
                if (ai.squad.size() >= 25 || ai.budget < currentBid + 0.5f) {
                    activeBidders[ai.team.name] = false;
                    continue;
                }
                
                // Check if next bid would exceed maximum
                if (currentBid + 0.5f > MAX_BID) {
                    activeBidders[ai.team.name] = false;
                    continue;
                }
                
                // AI logic based on strategy
                bool aiBid = false;
                float value = (player.battingRating + player.bowlingRating + player.fieldingRating) / 3.0f;
                float maxBid = player.price;
                switch (ai.strategy) {
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
                // AI only bids if under maxBid, has budget, and squad/overseas room
                if (currentBid + 0.5f <= maxBid && ai.budget >= currentBid + 0.5f && ai.squad.size() < 25 && (player.nationality == "Indian" || ai.overseasCount < 8)) {
                    if (rand() % 100 < 70) { // 70% chance to bid if eligible
                        currentBid += 0.5f;
                        currentBidder = ai.team.name;
                        anyBid = true;
                        std::cout << ai.team.name << " bids ₹" << currentBid << " crore!\n";
                    } else {
                        activeBidders[ai.team.name] = false;
                        std::cout << ai.team.name << " passes.\n";
                    }
                } else {
                    activeBidders[ai.team.name] = false;
                    std::cout << ai.team.name << " passes.\n";
                }
            }
        }
        // If only one bidder left, award player
        int biddersLeft = 0;
        std::string winner;
        for (const auto& [name, active] : activeBidders) {
            if (active) {
                biddersLeft++;
                winner = name;
            }
        }
        if (biddersLeft <= 1) {
            if (winner.empty()) winner = currentBidder;
            std::cout << "\nPlayer " << player.name << " sold to " << winner << " for ₹" << currentBid << " crore!\n";
            // Add player to winner's squad
            for (auto& ai : aiTeams) {
                if (ai.team.name == winner) {
                    ai.squad.push_back(player);
                    ai.budget -= currentBid;
                    if (player.nationality == "Overseas") ai.overseasCount++;
                    break;
                }
            }
            // If user won, update userTeam pointer
            if (winner == userTeam->team.name) {
                userTeam = nullptr;
                for (auto& ai : aiTeams) {
                    if (ai.team.name == managerProfile.selectedTeam) {
                        userTeam = &ai;
                        break;
                    }
                }
            }
            break;
        }
        if (!anyBid) break; // No more bids
    }
    playerIndex++;
    std::cout << "\nType 'continue' to auction next player or 'back' to exit auction." << std::endl;
    std::cout << "Enter your choice: ";
}

// Avatar and Logo helpers
std::string IPLManager::getTeamLogo(const std::string& teamName) {
    if (teamName == "Mumbai Indians") {
        return "🔵⚪ MI";
    } else if (teamName == "Chennai Super Kings") {
        return "🟡🔵 CSK";
    } else if (teamName == "Royal Challengers Bangalore") {
        return "🔴⚫ RCB";
    } else if (teamName == "Kolkata Knight Riders") {
        return "🟣🟡 KKR";
    } else if (teamName == "Delhi Capitals") {
        return "🔵🔴 DC";
    } else if (teamName == "Punjab Kings") {
        return "🔴⚪ PBKS";
    } else if (teamName == "Rajasthan Royals") {
        return "🔵🟡 RR";
    } else if (teamName == "Sunrisers Hyderabad") {
        return "🟠🔴 SRH";
    } else if (teamName == "Gujarat Titans") {
        return "🔵🟢 GT";
    } else if (teamName == "Lucknow Super Giants") {
        return "🔵🟢 LSG";
    }
    return "";
}

std::string IPLManager::getManagerAvatar() {
    if (managerProfile.avatar == "Coach") {
        return "👨‍💼";
    } else if (managerProfile.avatar == "Captain") {
        return "👨‍✈️";
    } else if (managerProfile.avatar == "Analyst") {
        return "📊";
    } else if (managerProfile.avatar == "Legend") {
        return "🏆";
    } else if (managerProfile.avatar == "Rookie") {
        return "🆕";
    } else {
        return "👤";
    }
}

void IPLManager::simulateEntireSeason() {
    printBanner("🏁 SIMULATING ENTIRE SEASON");
    std::cout << "\nSimulating all matches...\n\n";
    generateSeasonFixtures();
    for (auto& match : seasonFixtures) {
        simulateMatch(match);
    }
    updateLeagueTable();
    std::cout << "\nAll matches completed!\n\n";
    showLeagueTable();
    std::cout << "\nPress Enter to return to main menu...";
    std::cin.get();
    currentState = GameState::MAIN_MENU;
}

// Helper: Get available bowlers for a team (max 4 overs per bowler)
std::vector<IPLPlayer*> IPLManager::getAvailableBowlers(AITeam& team, std::map<std::string, int>& bowlerOvers) {
    std::vector<IPLPlayer*> bowlers;
    for (auto& player : team.squad) {
        if ((player.role == "Bowler" || player.role == "All-rounder") && bowlerOvers[player.name] < 4) {
            bowlers.push_back(&player);
        }
    }
    return bowlers;
}

// Helper: Create batting order based on batting approaches
std::vector<IPLPlayer*> IPLManager::createBattingOrder(AITeam& team) {
    std::vector<IPLPlayer*> battingOrder;
    for (auto& player : team.squad) {
        if (player.battingApproach == "Aggressive") {
            battingOrder.push_back(&player);
        }
    }
    for (auto& player : team.squad) {
        if (player.battingApproach == "Balanced") {
            battingOrder.push_back(&player);
        }
    }
    for (auto& player : team.squad) {
        if (player.battingApproach == "Defensive") {
            battingOrder.push_back(&player);
        }
    }
    for (auto& player : team.squad) {
        if (player.battingApproach == "Attacking") {
            battingOrder.push_back(&player);
        }
    }
    return battingOrder;
}

// Helper: Auto-simulate matches not involving manager's team
void IPLManager::autoSimulateOtherMatches() {
    std::cout << "\n🏏 Auto-simulating other matches...\n";
    
    for (auto& match : seasonFixtures) {
        if (!match.isPlayed && match.team1 != managerProfile.selectedTeam && match.team2 != managerProfile.selectedTeam) {
            std::cout << "Simulating: " << match.team1 << " vs " << match.team2 << "... ";
            
            // Quick simulation without detailed commentary
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> scoreDist(120, 200);
            std::uniform_int_distribution<> wicketDist(5, 10);
            
            match.team1Score = scoreDist(gen);
            match.team2Score = scoreDist(gen);
            match.isPlayed = true;
            
            if (match.team1Score > match.team2Score) {
                match.winner = match.team1;
                std::cout << match.team1 << " wins by " << (match.team1Score - match.team2Score) << " runs\n";
            } else if (match.team2Score > match.team1Score) {
                match.winner = match.team2;
                std::cout << match.team2 << " wins by " << (match.team2Score - match.team1Score) << " runs\n";
            } else {
                match.winner = "Tie";
                std::cout << "Match tied!\n";
            }
            
            // Update team stats
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
        }
    }
    std::cout << "Auto-simulation complete!\n\n";
}

// Helper: Show squad with detailed information
void IPLManager::showDetailedSquad() {
    printBanner("👥 DETAILED SQUAD VIEW");
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
    
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << std::left << std::setw(50) << managerProfile.selectedTeam << "║\n";
    std::cout << "║  Budget: ₹" << std::fixed << std::setprecision(1) << std::setw(8) << userTeam->budget << " crore";
    std::cout << "  Squad: " << userTeam->squad.size() << "/25";
    std::cout << "  Overseas: " << userTeam->overseasCount << "/8" << std::setw(15) << " ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Name                Role      Nationality  Approach   Age   Bat   Bowl  Field  Price   Overall Rating      ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    for (const auto& player : userTeam->squad) {
        float avgRating = (player.battingRating + player.bowlingRating + player.fieldingRating) / 3.0f;
        std::cout << "║  " << std::left << std::setw(20) << player.name
                  << std::setw(10) << player.role
                  << std::setw(13) << player.nationality
                  << std::setw(11) << player.battingApproach
                  << std::setw(6) << player.age
                  << std::setw(6) << std::fixed << std::setprecision(0) << player.battingRating
                  << std::setw(7) << std::fixed << std::setprecision(0) << player.bowlingRating
                  << std::setw(7) << std::fixed << std::setprecision(0) << player.fieldingRating
                  << std::setw(8) << std::fixed << std::setprecision(1) << player.price
                  << std::setw(8) << std::fixed << std::setprecision(1) << avgRating << " ║\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    // Show team statistics
    std::cout << "\n📊 TEAM STATISTICS:\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    
    // Count players by role
    int batsmen = 0, bowlers = 0, allRounders = 0, wicketKeepers = 0;
    int indians = 0, overseas = 0;
    float totalBattingRating = 0, totalBowlingRating = 0, totalFieldingRating = 0;
    
    for (const auto& player : userTeam->squad) {
        if (player.role == "Batsman") batsmen++;
        else if (player.role == "Bowler") bowlers++;
        else if (player.role == "All-rounder") allRounders++;
        else if (player.role == "Wicket-keeper") wicketKeepers++;
        
        if (player.nationality == "Indian") indians++;
        else overseas++;
        
        totalBattingRating += player.battingRating;
        totalBowlingRating += player.bowlingRating;
        totalFieldingRating += player.fieldingRating;
    }
    
    float avgBatting = totalBattingRating / userTeam->squad.size();
    float avgBowling = totalBowlingRating / userTeam->squad.size();
    float avgFielding = totalFieldingRating / userTeam->squad.size();
    
    std::cout << "║  Role Distribution:                                    ║\n";
    std::cout << "║    Batsmen: " << std::setw(3) << batsmen << "    Bowlers: " << std::setw(3) << bowlers 
              << "    All-rounders: " << std::setw(3) << allRounders << "    Wicket-keepers: " << std::setw(3) << wicketKeepers << " ║\n";
    std::cout << "║  Nationality: Indians " << std::setw(3) << indians << "    Overseas " << std::setw(3) << overseas << "                    ║\n";
    std::cout << "║  Average Ratings: Bat " << std::fixed << std::setprecision(1) << std::setw(5) << avgBatting 
              << "    Bowl " << std::setw(5) << avgBowling << "    Field " << std::setw(5) << avgFielding << "        ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // Check minimum requirements
    bool meetsRequirements = validateSquadRequirements(*userTeam);
    std::cout << "\n📋 MINIMUM REQUIREMENTS STATUS:\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Requirement                    Required  Current  Status    ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Total Players                  ≥18       " << std::setw(7) << userTeam->squad.size() 
              << "  " << (userTeam->squad.size() >= 18 ? "✓" : "✗") << "        ║\n";
    std::cout << "║  Wicket-keepers                 ≥1        " << std::setw(7) << wicketKeepers 
              << "  " << (wicketKeepers >= 1 ? "✓" : "✗") << "        ║\n";
    std::cout << "║  All-rounders                   ≥3        " << std::setw(7) << allRounders 
              << "  " << (allRounders >= 3 ? "✓" : "✗") << "        ║\n";
    std::cout << "║  Bowlers                        ≥5        " << std::setw(7) << bowlers 
              << "  " << (bowlers >= 5 ? "✓" : "✗") << "        ║\n";
    std::cout << "║  Batsmen                        ≥5        " << std::setw(7) << batsmen 
              << "  " << (batsmen >= 5 ? "✓" : "✗") << "        ║\n";
    std::cout << "║  Overseas Players               ≤8        " << std::setw(7) << overseas 
              << "  " << (overseas <= 8 ? "✓" : "✗") << "        ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    if (meetsRequirements) {
        std::cout << "\n✅ Squad meets all minimum requirements!\n";
    } else {
        std::cout << "\n⚠️  Squad does not meet all minimum requirements.\n";
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Helper: Select player using arrow keys
IPLPlayer* IPLManager::selectPlayerWithArrows(const std::vector<IPLPlayer*>& players, const std::string& title) {
    if (players.empty()) return nullptr;
    
    int selectedIndex = 0;
    bool selectionMade = false;
    
    while (!selectionMade) {
        clearScreen();
        printBanner(title);
        std::cout << "\n";
        
        std::cout << "Use ↑/↓ arrows to select, Enter to confirm, Esc to cancel\n\n";
        
        for (size_t i = 0; i < players.size(); ++i) {
            if (i == selectedIndex) {
                std::cout << "  ▶ "; // Selected indicator
            } else {
                std::cout << "    ";
            }
            
            const auto& player = players[i];
            std::cout << std::left << std::setw(25) << player->name
                      << std::setw(12) << player->role
                      << std::setw(10) << player->battingApproach
                      << std::setw(8) << std::fixed << std::setprecision(0) << player->battingRating
                      << std::setw(8) << std::fixed << std::setprecision(0) << player->bowlingRating
                      << std::setw(8) << std::fixed << std::setprecision(0) << player->fieldingRating << "\n";
        }
        
        int key = getArrowKeyInput();
        switch (key) {
            case 1: // Up arrow
                if (selectedIndex > 0) selectedIndex--;
                break;
            case 2: // Down arrow
                if (selectedIndex < players.size() - 1) selectedIndex++;
                break;
            case 5: // Enter
                selectionMade = true;
                break;
            case 6: // Escape
                return nullptr;
        }
    }
    
    return players[selectedIndex];
}

// Helper: Get arrow key input
int IPLManager::getArrowKeyInput() {
    char input;
    std::cin.get(input);
    
    // Check for arrow keys (ESC sequence)
    if (input == '\033') {
        std::cin.get(input); // Skip '['
        if (input == '[') {
            std::cin.get(input);
            switch (input) {
                case 'A': return 1; // Up arrow
                case 'B': return 2; // Down arrow
                case 'C': return 3; // Right arrow
                case 'D': return 4; // Left arrow
            }
        }
    }
    
    // Check for Enter key
    if (input == '\n' || input == '\r') {
        return 5; // Enter
    }
    
    // Check for Escape key
    if (input == 27) {
        return 6; // Escape
    }
    
    return 0; // No special key
}

// Helper: Validate team squad requirements
bool IPLManager::validateSquadRequirements(const AITeam& team) {
    SquadStats stats = getSquadStats(team);
    
    // Check minimum squad size
    if (stats.totalPlayers < 18) {
        return false;
    }
    
    // Check minimum role requirements
    if (stats.wicketKeepers < 1) {
        return false;
    }
    if (stats.allRounders < 3) {
        return false;
    }
    if (stats.bowlers < 5) {
        return false;
    }
    if (stats.batsmen < 5) {
        return false;
    }
    
    // Check overseas player limit
    if (stats.overseas > 8) {
        return false;
    }
    
    return true;
}

// Helper: Get team squad statistics
IPLManager::SquadStats IPLManager::getSquadStats(const AITeam& team) {
    SquadStats stats;
    stats.totalPlayers = team.squad.size();
    stats.wicketKeepers = 0;
    stats.allRounders = 0;
    stats.bowlers = 0;
    stats.batsmen = 0;
    stats.indians = 0;
    stats.overseas = 0;
    
    for (const auto& player : team.squad) {
        if (player.role == "Wicket-keeper") stats.wicketKeepers++;
        else if (player.role == "All-rounder") stats.allRounders++;
        else if (player.role == "Bowler") stats.bowlers++;
        else if (player.role == "Batsman") stats.batsmen++;
        
        if (player.nationality == "Indian") stats.indians++;
        else stats.overseas++;
    }
    
    return stats;
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