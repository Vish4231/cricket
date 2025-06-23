#pragma once

#include <string>
#include <vector>
#include <memory>

enum class VenueType {
    INTERNATIONAL,
    DOMESTIC,
    FRANCHISE,
    CLUB
};

enum class PitchType {
    BATTER_FRIENDLY,
    BOWLER_FRIENDLY,
    SPIN_FRIENDLY,
    SEAM_FRIENDLY,
    BALANCED
};

enum class WeatherType {
    SUNNY,
    CLOUDY,
    OVERCAST,
    RAINY,
    WINDY,
    HUMID
};

struct VenueStats {
    int capacity;
    int averageAttendance;
    double ticketPrice;
    int facilities;
    int reputation;
    std::string location;
    std::string country;
};

struct PitchCharacteristics {
    PitchType type;
    int hardness; // 1-10
    int moisture; // 1-10
    int grass; // 1-10
    int wear; // 1-10
    bool isSpinning;
    bool isSeaming;
    bool isBouncy;
    double averageScore;
    double bowlingAverage;
    double spinEffectiveness;
    double seamEffectiveness;
};

struct WeatherPattern {
    WeatherType type;
    int temperature; // Celsius
    int humidity; // Percentage
    int windSpeed; // km/h
    std::string windDirection;
    bool isOvercast;
    bool isRaining;
    int visibility; // 1-10
    double rainProbability;
};

class Venue {
public:
    Venue(const std::string& name, const std::string& location, VenueType type);
    ~Venue();
    
    // Basic info
    const std::string& GetName() const { return name; }
    const std::string& GetLocation() const { return location; }
    VenueType GetType() const { return type; }
    
    // Stats and characteristics
    const VenueStats& GetStats() const { return stats; }
    void SetStats(const VenueStats& newStats) { stats = newStats; }
    
    const PitchCharacteristics& GetPitchCharacteristics() const { return pitchCharacteristics; }
    void SetPitchCharacteristics(const PitchCharacteristics& characteristics);
    
    // Weather management
    void SetCurrentWeather(const WeatherPattern& weather);
    const WeatherPattern& GetCurrentWeather() const { return currentWeather; }
    void UpdateWeather();
    void GenerateWeatherForecast();
    const std::vector<WeatherPattern>& GetWeatherForecast() const { return weatherForecast; }
    
    // Match history
    void AddMatchResult(const std::string& team1, const std::string& team2, 
                       int score1, int score2, const std::string& winner);
    const std::vector<std::string>& GetMatchHistory() const { return matchHistory; }
    
    // Pitch conditions
    void UpdatePitchConditions(int daysSinceLastMatch);
    void PreparePitchForMatch();
    double GetPitchDifficulty() const;
    double GetBattingAdvantage() const;
    double GetBowlingAdvantage() const;
    
    // Weather effects
    void ApplyWeatherEffects();
    bool IsMatchPlayable() const;
    int GetRainDelay() const;
    double GetWeatherImpact() const;
    
    // Facilities and amenities
    void SetFacilities(int facilities) { stats.facilities = facilities; }
    int GetFacilities() const { return stats.facilities; }
    bool HasFloodlights() const;
    bool HasCoveredStands() const;
    bool HasPracticeNets() const;
    
    // Financial aspects
    void SetTicketPrice(double price) { stats.ticketPrice = price; }
    double GetTicketPrice() const { return stats.ticketPrice; }
    int CalculateRevenue(int attendance) const;
    
    // Reputation and prestige
    void UpdateReputation(int change);
    int GetReputation() const { return stats.reputation; }
    std::string GetReputationLevel() const;
    
    // JSON serialization
    std::string ToJSON() const;
    static std::unique_ptr<Venue> FromJSON(const std::string& json);
    
private:
    // Basic info
    std::string name;
    std::string location;
    VenueType type;
    
    // Stats and characteristics
    VenueStats stats;
    PitchCharacteristics pitchCharacteristics;
    
    // Weather
    WeatherPattern currentWeather;
    std::vector<WeatherPattern> weatherForecast;
    
    // Match history
    std::vector<std::string> matchHistory;
    
    // Helper methods
    void InitializePitchCharacteristics();
    void GenerateWeatherPattern();
    void UpdatePitchWear();
    void CalculatePitchEffects();
    std::string FormatMatchResult(const std::string& team1, const std::string& team2, 
                                 int score1, int score2, const std::string& winner);
}; 