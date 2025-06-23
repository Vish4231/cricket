#pragma once

#include "Player.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

enum class TeamType {
    INTERNATIONAL,
    DOMESTIC,
    FRANCHISE,
    CLUB
};

enum class TeamFormation {
    BALANCED,
    BATSMAN_HEAVY,
    BOWLER_HEAVY,
    ALL_ROUNDER_HEAVY,
    AGGRESSIVE,
    DEFENSIVE
};

struct TeamStats {
    int matchesPlayed;
    int matchesWon;
    int matchesLost;
    int matchesDrawn;
    int matchesTied;
    double winPercentage;
    int totalRuns;
    int totalWickets;
    double netRunRate;
    int points;
};

struct TeamFinances {
    int budget;
    int currentSpending;
    int sponsorship;
    int ticketSales;
    int merchandise;
    int totalRevenue;
    int totalExpenses;
    int profit;
};

struct TeamMorale {
    int overallMorale;
    int teamChemistry;
    int playerSatisfaction;
    int managementRating;
    std::map<std::string, int> individualMorale;
};

class Team {
public:
    // Constructors
    Team(const std::string& name, TeamType type);
    Team(const Team& other); // Copy constructor
    Team(Team&& other) noexcept; // Move constructor
    Team& operator=(const Team& other); // Copy assignment
    Team& operator=(Team&& other) noexcept; // Move assignment
    ~Team();
    
    // Basic info
    const std::string& GetName() const { return name; }
    TeamType GetType() const { return type; }
    const std::string& GetHomeVenue() const { return homeVenue; }
    void SetHomeVenue(const std::string& venue) { homeVenue = venue; }
    
    // Squad management
    void AddPlayer(std::unique_ptr<Player> player);
    void RemovePlayer(const std::string& playerName);
    Player* GetPlayer(const std::string& playerName);
    const std::vector<std::unique_ptr<Player>>& GetSquad() const { return squad; }
    std::vector<Player*> GetAvailablePlayers() const;
    std::vector<Player*> GetInjuredPlayers() const;
    
    // Team composition
    void SetPlayingXI(const std::vector<std::string>& playerNames);
    const std::vector<Player*>& GetPlayingXI() const { return playingXI; }
    void SetCaptain(const std::string& playerName);
    Player* GetCaptain() const { return captain; }
    void SetViceCaptain(const std::string& playerName);
    Player* GetViceCaptain() const { return viceCaptain; }
    
    // Tactics and formation
    void SetFormation(TeamFormation formation) { currentFormation = formation; }
    TeamFormation GetFormation() const { return currentFormation; }
    void SetBattingOrder(const std::vector<std::string>& order);
    const std::vector<Player*>& GetBattingOrder() const { return battingOrder; }
    void SetBowlingOrder(const std::vector<std::string>& order);
    const std::vector<Player*>& GetBowlingOrder() const { return bowlingOrder; }
    
    // Fielding positions
    void SetFieldingPositions(const std::map<std::string, std::string>& positions);
    std::string GetPlayerFieldingPosition(const std::string& playerName) const;
    
    // Stats and performance
    const TeamStats& GetTeamStats() const { return stats; }
    void UpdateStats(const TeamStats& newStats);
    void CalculateTeamRating();
    int GetTeamRating() const { return teamRating; }
    
    // Finances
    const TeamFinances& GetFinances() const { return finances; }
    void UpdateFinances(const TeamFinances& newFinances);
    bool CanAffordPlayer(int playerSalary) const;
    void SpendMoney(int amount);
    void AddRevenue(int amount);
    
    // Morale and chemistry
    const TeamMorale& GetMorale() const { return morale; }
    void UpdateMorale();
    void UpdatePlayerMorale(const std::string& playerName, int change);
    
    // Staff and management
    void SetCoach(const std::string& coachName) { coach = coachName; }
    const std::string& GetCoach() const { return coach; }
    void SetManager(const std::string& managerName) { manager = managerName; }
    const std::string& GetManager() const { return manager; }
    
    // Scouting and youth
    void AddYouthPlayer(std::unique_ptr<Player> player);
    std::vector<Player*> GetYouthPlayers() const;
    void PromoteYouthPlayer(const std::string& playerName);
    
    // Contracts and transfers
    void RenewPlayerContract(const std::string& playerName, const Contract& newContract);
    void ReleasePlayer(const std::string& playerName);
    std::vector<Player*> GetPlayersWithExpiringContracts() const;
    
    // Match preparation
    void PrepareForMatch();
    void UpdatePlayerFitness();
    void SetMatchStrategy(const std::string& strategy);
    const std::string& GetMatchStrategy() const { return matchStrategy; }
    
    // JSON serialization
    std::string ToJSON() const;
    static std::unique_ptr<Team> FromJSON(const std::string& json);
    
    // Getters
    const std::string& GetCity() const { return city; }
    const std::string& GetOwner() const { return owner; }
    int GetFounded() const { return founded; }
    const std::string& GetColors() const { return colors; }
    const std::string& GetLogo() const { return logo; }
    float GetBudget() const { return budget; }
    int GetTrophyCount() const { return trophyCount; }
    const std::vector<std::string>& GetTrophies() const { return trophies; }
    const std::map<std::string, float>& GetStats() const { return statsMap; }
    const std::map<std::string, std::string>& GetSettings() const { return settings; }
    
private:
    // Basic info
    std::string name;
    TeamType type;
    std::string city;
    std::string owner;
    int founded;
    std::string colors;
    std::string logo;
    float budget;
    int trophyCount;
    std::vector<std::string> trophies;
    std::map<std::string, std::string> settings;
    
    // Stats and performance
    TeamStats stats;
    std::map<std::string, float> statsMap;
    int teamRating;
    
    // Squad and management
    std::vector<std::unique_ptr<Player>> squad;
    std::vector<Player*> playingXI;
    std::string coach;
    std::string manager;
    Player* captain;
    Player* viceCaptain;
    std::string homeVenue;
    
    // Tactics
    TeamFormation currentFormation;
    std::vector<Player*> battingOrder;
    std::vector<Player*> bowlingOrder;
    std::map<std::string, std::string> fieldingPositions;
    std::string matchStrategy;
    
    // Finances and morale
    TeamFinances finances;
    TeamMorale morale;
    
    // Youth and contracts
    std::vector<std::unique_ptr<Player>> youthPlayers;
    
    // Helper methods
    void InitializeStats();
    void CalculateTeamChemistry();
    bool IsPlayerInSquad(const std::string& playerName) const;
}; 