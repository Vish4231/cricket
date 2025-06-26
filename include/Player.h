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

enum class BattingApproach {
    AGGRESSIVE,
    DEFENSIVE,
    BALANCED,
    ATTACKING
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
    const std::string& GetNationality() const { return nationality; }
    PlayerRole GetPlayerRole() const { return role; }
    const std::string& GetRole() const { return roleString; }
    const std::string& GetBattingStyle() const { return battingStyleString; }
    const std::string& GetBowlingStyle() const { return bowlingStyleString; }
    BattingApproach GetBattingApproach() const { return battingApproach; }
    const std::string& GetBattingApproachString() const { return battingApproachString; }
    const std::string& GetTeam() const { return team; }
    const std::string& GetPhoto() const { return photo; }
    float GetBattingSkill() const { return battingSkill; }
    float GetBowlingSkill() const { return bowlingSkill; }
    float GetFieldingSkill() const { return fieldingSkill; }
    float GetExperience() const { return experience; }
    float GetFitness() const { return fitness; }
    int GetMorale() const { return morale; }
    float GetSalary() const { return salary; }
    float GetMarketValue() const { return marketValue; }
    const std::map<std::string, float>& GetStats() const { return statsMap; }
    const std::map<std::string, std::string>& GetAttributes() const { return attributesMap; }
    const std::vector<std::string>& GetSpecialties() const { return specialties; }
    const std::vector<std::string>& GetAchievements() const { return achievements; }
    int GetForm() const { return form; }
    const std::map<std::string, std::string>& GetPreferences() const { return preferences; }
    
    // Stats and attributes
    const PlayerStats& GetPlayerStats() const { return stats; }
    const PlayerAttributes& GetPlayerAttributes() const { return attributes; }
    PlayerAttributes& GetPlayerAttributes() { return attributes; }
    
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
    void AddSpecialtyString(const std::string& specialty);
    bool HasSpecialtyString(const std::string& specialty) const;
    
    // Career
    void AddAchievement(const std::string& achievement);
    
    // Rating calculation
    int GetOverallRating() const;
    int GetBattingRating() const;
    int GetBowlingRating() const;
    int GetFieldingRating() const;
    
    // Form and morale
    void UpdateForm(int formChange);
    void UpdateMorale(int moraleChange);
    
    // JSON serialization
    std::string ToJSON() const;
    static std::unique_ptr<Player> FromJSON(const std::string& json);
    
private:
    // Basic info
    std::string name;
    int age;
    std::string nationality;
    PlayerRole role;
    std::string roleString;
    BattingStyle battingStyle;
    std::string battingStyleString;
    BowlingStyle bowlingStyle;
    std::string bowlingStyleString;
    BattingApproach battingApproach;
    std::string battingApproachString;
    std::string team;
    std::string photo;
    
    // Skills and attributes
    float battingSkill;
    float bowlingSkill;
    float fieldingSkill;
    float experience;
    float fitness;
    int form;
    int morale;
    float salary;
    float marketValue;
    
    // Stats and attributes
    PlayerStats stats;
    PlayerAttributes attributes;
    std::map<std::string, float> statsMap;
    std::map<std::string, std::string> attributesMap;
    
    // Team and contract
    std::string currentTeam;
    Contract contract;
    
    // Injuries and fitness
    std::vector<Injury> injuries;
    
    // Specialties and achievements
    std::vector<PlayerSpecialty> playerSpecialties;
    std::vector<std::string> specialties;
    std::vector<std::string> achievements;
    
    // Preferences
    std::map<std::string, std::string> preferences;
    
    // Helper methods
    void InitializeAttributes();
    void CalculateRatings();
}; 