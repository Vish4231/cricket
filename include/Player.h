#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

enum class PlayerRole {
    BATSMAN,
    BOWLER,
    ALL_ROUNDER,
    WICKET_KEEPER,
    CAPTAIN
};

enum class BattingStyle {
    RIGHT_HANDED,
    LEFT_HANDED
};

enum class BowlingStyle {
    FAST,
    MEDIUM_FAST,
    MEDIUM,
    SPIN_OFF_BREAK,
    SPIN_LEG_BREAK,
    SPIN_LEFT_ARM_ORTHODOX,
    SPIN_LEFT_ARM_UNORTHODOX
};

enum class PlayerSpecialty {
    OPENING_BATSMAN,
    MIDDLE_ORDER,
    FINISHER,
    POWER_HITTER,
    ANCHOR,
    FAST_BOWLER,
    SPIN_BOWLER,
    WICKET_KEEPER,
    ALL_ROUNDER
};

struct PlayerStats {
    // Batting stats
    int matches;
    int innings;
    int runs;
    int highestScore;
    double average;
    double strikeRate;
    int fifties;
    int hundreds;
    int fours;
    int sixes;
    
    // Bowling stats
    int overs;
    int wickets;
    double bowlingAverage;
    double economyRate;
    double bowlingStrikeRate;
    int fiveWickets;
    int tenWickets;
    int bestBowling;
    
    // Fielding stats
    int catches;
    int stumpings;
    int runOuts;
};

struct PlayerAttributes {
    // Batting attributes (1-100)
    int battingTechnique;
    int battingPower;
    int battingTiming;
    int battingConcentration;
    int battingAggression;
    
    // Bowling attributes (1-100)
    int bowlingAccuracy;
    int bowlingPace;
    int bowlingSpin;
    int bowlingVariation;
    int bowlingStamina;
    
    // Fielding attributes (1-100)
    int fielding;
    int throwing;
    int catching;
    int wicketKeeping;
    
    // Mental attributes (1-100)
    int leadership;
    int temperament;
    int pressureHandling;
    int teamwork;
    int experience;
    
    // Physical attributes (1-100)
    int fitness;
    int stamina;
    int speed;
    int strength;
    int agility;
};

struct Injury {
    std::string type;
    int severity; // 1-10
    int recoveryDays;
    bool isRecovered;
};

struct Contract {
    std::string teamName;
    int salary;
    int contractYears;
    std::string startDate;
    std::string endDate;
};

class Player {
public:
    Player(const std::string& name, int age, PlayerRole role);
    ~Player();
    
    // Basic info
    const std::string& GetName() const { return name; }
    int GetAge() const { return age; }
    PlayerRole GetRole() const { return role; }
    BattingStyle GetBattingStyle() const { return battingStyle; }
    BowlingStyle GetBowlingStyle() const { return bowlingStyle; }
    
    // Stats and attributes
    const PlayerStats& GetStats() const { return stats; }
    const PlayerAttributes& GetAttributes() const { return attributes; }
    PlayerAttributes& GetAttributes() { return attributes; }
    
    // Team and contract
    const std::string& GetCurrentTeam() const { return currentTeam; }
    void SetCurrentTeam(const std::string& team) { currentTeam = team; }
    const Contract& GetContract() const { return contract; }
    void SetContract(const Contract& newContract) { contract = newContract; }
    
    // Injuries and fitness
    bool IsInjured() const { return !injuries.empty() && !injuries.back().isRecovered; }
    void AddInjury(const Injury& injury);
    void UpdateInjuries();
    int GetFitnessLevel() const { return attributes.fitness; }
    
    // Performance
    void UpdateStats(const PlayerStats& newStats);
    void SimulatePerformance();
    
    // Specialties
    void AddSpecialty(PlayerSpecialty specialty);
    bool HasSpecialty(PlayerSpecialty specialty) const;
    
    // Career
    void AddAchievement(const std::string& achievement);
    const std::vector<std::string>& GetAchievements() const { return achievements; }
    
    // Rating calculation
    int GetOverallRating() const;
    int GetBattingRating() const;
    int GetBowlingRating() const;
    int GetFieldingRating() const;
    
    // Form and morale
    void UpdateForm(int formChange);
    int GetForm() const { return form; }
    void UpdateMorale(int moraleChange);
    int GetMorale() const { return morale; }
    
    // JSON serialization
    std::string ToJSON() const;
    static std::unique_ptr<Player> FromJSON(const std::string& json);
    
private:
    // Basic info
    std::string name;
    int age;
    PlayerRole role;
    BattingStyle battingStyle;
    BowlingStyle bowlingStyle;
    
    // Stats and attributes
    PlayerStats stats;
    PlayerAttributes attributes;
    
    // Team and contract
    std::string currentTeam;
    Contract contract;
    
    // Injuries and fitness
    std::vector<Injury> injuries;
    
    // Specialties and achievements
    std::vector<PlayerSpecialty> specialties;
    std::vector<std::string> achievements;
    
    // Form and morale (1-100)
    int form;
    int morale;
    
    // Helper methods
    void InitializeAttributes();
    void CalculateRatings();
}; 