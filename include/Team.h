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
    Team(const std::string& name, TeamType type);
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
    const TeamStats& GetStats() const { return stats; }
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
    
private:
    // Basic info
    std::string name;
    TeamType type;
    std::string homeVenue;
    
    // Squad and playing XI
    std::vector<std::unique_ptr<Player>> squad;
    std::vector<Player*> playingXI;
    std::vector<Player*> battingOrder;
    std::vector<Player*> bowlingOrder;
    
    // Leadership
    Player* captain;
    Player* viceCaptain;
    
    // Tactics
    TeamFormation currentFormation;
    std::map<std::string, std::string> fieldingPositions;
    std::string matchStrategy;
    
    // Stats and performance
    TeamStats stats;
    int teamRating;
    
    // Finances
    TeamFinances finances;
    
    // Morale
    TeamMorale morale;
    
    // Staff
    std::string coach;
    std::string manager;
    
    // Youth development
    std::vector<std::unique_ptr<Player>> youthPlayers;
    
    // Helper methods
    void CalculateTeamChemistry();
    void UpdateTeamMorale();
    bool IsPlayerInSquad(const std::string& playerName) const;
}; 