#include "Player.h"
#include <algorithm>
#include <random>
#include <sstream>

Player::Player(const std::string& name, int age, PlayerRole role)
    : name(name)
    , age(age)
    , role(role)
    , battingStyle(BattingStyle::RIGHT_HANDED)
    , bowlingStyle(BowlingStyle::MEDIUM)
    , form(75)
    , morale(80)
{
    InitializeAttributes();
}

Player::~Player() {
}

void Player::InitializeAttributes() {
    // Initialize all attributes to base values
    attributes.battingTechnique = 50;
    attributes.battingPower = 50;
    attributes.battingTiming = 50;
    attributes.battingConcentration = 50;
    attributes.battingAggression = 50;
    
    attributes.bowlingAccuracy = 50;
    attributes.bowlingPace = 50;
    attributes.bowlingSpin = 50;
    attributes.bowlingVariation = 50;
    attributes.bowlingStamina = 50;
    
    attributes.fielding = 50;
    attributes.throwing = 50;
    attributes.catching = 50;
    attributes.wicketKeeping = 50;
    
    attributes.leadership = 50;
    attributes.temperament = 50;
    attributes.pressureHandling = 50;
    attributes.teamwork = 50;
    attributes.experience = 50;
    
    attributes.fitness = 80;
    attributes.stamina = 80;
    attributes.speed = 50;
    attributes.strength = 50;
    attributes.agility = 50;
    
    // Adjust attributes based on role
    switch (role) {
        case PlayerRole::BATSMAN:
            attributes.battingTechnique = 70;
            attributes.battingPower = 65;
            attributes.battingTiming = 70;
            attributes.battingConcentration = 75;
            break;
            
        case PlayerRole::BOWLER:
            attributes.bowlingAccuracy = 70;
            attributes.bowlingPace = 75;
            attributes.bowlingVariation = 65;
            attributes.bowlingStamina = 70;
            break;
            
        case PlayerRole::ALL_ROUNDER:
            attributes.battingTechnique = 60;
            attributes.battingPower = 60;
            attributes.bowlingAccuracy = 60;
            attributes.bowlingPace = 60;
            break;
            
        case PlayerRole::WICKET_KEEPER:
            attributes.wicketKeeping = 80;
            attributes.catching = 75;
            attributes.battingTechnique = 65;
            break;
            
        case PlayerRole::CAPTAIN:
            attributes.leadership = 80;
            attributes.temperament = 75;
            attributes.pressureHandling = 80;
            break;
    }
    
    // Adjust based on age
    if (age < 25) {
        attributes.experience = 30;
        attributes.fitness = 90;
        attributes.stamina = 90;
    } else if (age > 35) {
        attributes.experience = 80;
        attributes.fitness = 70;
        attributes.stamina = 70;
    }
    
    CalculateRatings();
}

void Player::AddInjury(const Injury& injury) {
    injuries.push_back(injury);
    
    // Reduce fitness based on injury severity
    int fitnessReduction = injury.severity * 5;
    attributes.fitness = std::max(10, attributes.fitness - fitnessReduction);
    
    // Reduce morale
    UpdateMorale(-10);
}

void Player::UpdateInjuries() {
    for (auto& injury : injuries) {
        if (!injury.isRecovered) {
            injury.recoveryDays--;
            if (injury.recoveryDays <= 0) {
                injury.isRecovered = true;
                // Gradually recover fitness
                attributes.fitness = std::min(100, attributes.fitness + 5);
            }
        }
    }
    
    // Remove recovered injuries
    injuries.erase(
        std::remove_if(injuries.begin(), injuries.end(),
                      [](const Injury& injury) { return injury.isRecovered; }),
        injuries.end()
    );
}

void Player::UpdateStats(const PlayerStats& newStats) {
    stats = newStats;
    
    // Recalculate averages
    if (stats.innings > 0) {
        stats.average = static_cast<double>(stats.runs) / stats.innings;
    }
    
    if (stats.overs > 0) {
        stats.bowlingAverage = static_cast<double>(stats.runs) / stats.wickets;
        stats.economyRate = static_cast<double>(stats.runs) / stats.overs;
    }
}

void Player::SimulatePerformance() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    
    // Simulate form changes
    int formChange = dis(gen) - 50; // -49 to 50
    UpdateForm(formChange / 10);
    
    // Simulate morale changes
    int moraleChange = dis(gen) - 50;
    UpdateMorale(moraleChange / 10);
    
    // Simulate fitness changes
    int fitnessChange = dis(gen) - 50;
    attributes.fitness = std::max(10, std::min(100, attributes.fitness + fitnessChange / 20));
}

void Player::AddSpecialty(PlayerSpecialty specialty) {
    if (!HasSpecialty(specialty)) {
        specialties.push_back(specialty);
    }
}

bool Player::HasSpecialty(PlayerSpecialty specialty) const {
    return std::find(specialties.begin(), specialties.end(), specialty) != specialties.end();
}

void Player::AddAchievement(const std::string& achievement) {
    achievements.push_back(achievement);
    UpdateMorale(5); // Small morale boost for achievements
}

int Player::GetOverallRating() const {
    int battingRating = GetBattingRating();
    int bowlingRating = GetBowlingRating();
    int fieldingRating = GetFieldingRating();
    
    switch (role) {
        case PlayerRole::BATSMAN:
            return (battingRating * 7 + fieldingRating * 3) / 10;
        case PlayerRole::BOWLER:
            return (bowlingRating * 7 + fieldingRating * 3) / 10;
        case PlayerRole::ALL_ROUNDER:
            return (battingRating * 4 + bowlingRating * 4 + fieldingRating * 2) / 10;
        case PlayerRole::WICKET_KEEPER:
            return (battingRating * 5 + fieldingRating * 5) / 10;
        case PlayerRole::CAPTAIN:
            return (battingRating * 3 + bowlingRating * 2 + fieldingRating * 2 + attributes.leadership * 3) / 10;
        default:
            return (battingRating + bowlingRating + fieldingRating) / 3;
    }
}

int Player::GetBattingRating() const {
    int baseRating = (attributes.battingTechnique + attributes.battingPower + 
                     attributes.battingTiming + attributes.battingConcentration) / 4;
    
    // Adjust for form and morale
    baseRating += (form - 50) / 10;
    baseRating += (morale - 50) / 20;
    
    // Adjust for experience
    baseRating += attributes.experience / 20;
    
    return std::max(1, std::min(100, baseRating));
}

int Player::GetBowlingRating() const {
    int baseRating = (attributes.bowlingAccuracy + attributes.bowlingPace + 
                     attributes.bowlingSpin + attributes.bowlingVariation) / 4;
    
    // Adjust for form and morale
    baseRating += (form - 50) / 10;
    baseRating += (morale - 50) / 20;
    
    // Adjust for stamina
    baseRating += attributes.bowlingStamina / 20;
    
    return std::max(1, std::min(100, baseRating));
}

int Player::GetFieldingRating() const {
    int baseRating = (attributes.fielding + attributes.throwing + attributes.catching) / 3;
    
    // Add wicket keeping if applicable
    if (role == PlayerRole::WICKET_KEEPER) {
        baseRating = (baseRating + attributes.wicketKeeping) / 2;
    }
    
    // Adjust for fitness and agility
    baseRating += (attributes.fitness + attributes.agility) / 40;
    
    return std::max(1, std::min(100, baseRating));
}

void Player::UpdateForm(int formChange) {
    form = std::max(1, std::min(100, form + formChange));
}

void Player::UpdateMorale(int moraleChange) {
    morale = std::max(1, std::min(100, morale + moraleChange));
}

void Player::CalculateRatings() {
    // This method is called after attribute changes to recalculate ratings
    // The actual calculation is done in the getter methods
}

std::string Player::ToJSON() const {
    std::ostringstream json;
    json << "{";
    json << "\"name\":\"" << name << "\",";
    json << "\"age\":" << age << ",";
    json << "\"role\":" << static_cast<int>(role) << ",";
    json << "\"battingStyle\":" << static_cast<int>(battingStyle) << ",";
    json << "\"bowlingStyle\":" << static_cast<int>(bowlingStyle) << ",";
    json << "\"currentTeam\":\"" << currentTeam << "\",";
    json << "\"form\":" << form << ",";
    json << "\"morale\":" << morale;
    json << "}";
    return json.str();
}

std::unique_ptr<Player> Player::FromJSON(const std::string& json) {
    // TODO: Implement JSON parsing
    // For now, return a default player
    return std::make_unique<Player>("Unknown Player", 25, PlayerRole::BATSMAN);
} 