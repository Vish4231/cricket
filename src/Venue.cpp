#include "Venue.h"
#include <random>
#include <sstream>

Venue::Venue(const std::string& name, const std::string& location, VenueType type)
    : name(name)
    , location(location)
    , type(type)
{
    InitializePitchCharacteristics();
    GenerateWeatherPattern();
}

Venue::~Venue() {
}

void Venue::SetPitchCharacteristics(const PitchCharacteristics& characteristics) {
    pitchCharacteristics = characteristics;
    CalculatePitchEffects();
}

void Venue::SetCurrentWeather(const WeatherPattern& weather) {
    currentWeather = weather;
    ApplyWeatherEffects();
}

void Venue::UpdateWeather() {
    // Simulate weather changes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> tempChange(-2, 2);
    std::uniform_int_distribution<> humidityChange(-5, 5);
    std::uniform_int_distribution<> windChange(-3, 3);
    
    currentWeather.temperature += tempChange(gen);
    currentWeather.humidity += humidityChange(gen);
    currentWeather.windSpeed += windChange(gen);
    
    // Clamp values
    currentWeather.temperature = std::max(-10, std::min(50, currentWeather.temperature));
    currentWeather.humidity = std::max(0, std::min(100, currentWeather.humidity));
    currentWeather.windSpeed = std::max(0, std::min(50, currentWeather.windSpeed));
    
    // Update weather type based on conditions
    if (currentWeather.temperature < 10) {
        currentWeather.type = WeatherType::OVERCAST;
    } else if (currentWeather.humidity > 80) {
        currentWeather.type = WeatherType::HUMID;
    } else if (currentWeather.windSpeed > 20) {
        currentWeather.type = WeatherType::WINDY;
    } else if (currentWeather.isRaining) {
        currentWeather.type = WeatherType::RAINY;
    } else if (currentWeather.isOvercast) {
        currentWeather.type = WeatherType::OVERCAST;
    } else {
        currentWeather.type = WeatherType::SUNNY;
    }
    
    ApplyWeatherEffects();
}

void Venue::GenerateWeatherForecast() {
    weatherForecast.clear();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> tempVar(-5, 5);
    std::uniform_int_distribution<> humidityVar(-10, 10);
    std::uniform_real_distribution<> rainProb(0.0, 1.0);
    
    for (int i = 0; i < 7; ++i) { // 7-day forecast
        WeatherPattern forecast = currentWeather;
        forecast.temperature += tempVar(gen);
        forecast.humidity += humidityVar(gen);
        forecast.rainProbability = rainProb(gen);
        
        // Clamp values
        forecast.temperature = std::max(-10, std::min(50, forecast.temperature));
        forecast.humidity = std::max(0, std::min(100, forecast.humidity));
        
        weatherForecast.push_back(forecast);
    }
}

void Venue::AddMatchResult(const std::string& team1, const std::string& team2, 
                          int score1, int score2, const std::string& winner) {
    std::string result = FormatMatchResult(team1, team2, score1, score2, winner);
    matchHistory.push_back(result);
    
    // Update pitch wear
    UpdatePitchWear();
}

void Venue::UpdatePitchConditions(int daysSinceLastMatch) {
    // Pitch recovers over time
    if (daysSinceLastMatch > 0) {
        pitchCharacteristics.wear = std::max(1, pitchCharacteristics.wear - daysSinceLastMatch);
        pitchCharacteristics.moisture = std::min(10, pitchCharacteristics.moisture + daysSinceLastMatch);
    }
    
    CalculatePitchEffects();
}

void Venue::PreparePitchForMatch() {
    // Set pitch conditions for match day
    pitchCharacteristics.wear = 5; // Moderate wear
    pitchCharacteristics.moisture = 6; // Moderate moisture
    
    // Apply weather effects
    ApplyWeatherEffects();
    
    CalculatePitchEffects();
}

double Venue::GetPitchDifficulty() const {
    double difficulty = 5.0; // Base difficulty
    
    // Adjust based on pitch characteristics
    difficulty += (pitchCharacteristics.hardness - 5) * 0.2;
    difficulty += (pitchCharacteristics.wear - 5) * 0.3;
    difficulty -= (pitchCharacteristics.moisture - 5) * 0.1;
    
    // Weather effects
    if (currentWeather.isRaining) {
        difficulty += 1.0;
    }
    if (currentWeather.windSpeed > 15) {
        difficulty += 0.5;
    }
    
    return std::max(1.0, std::min(10.0, difficulty));
}

double Venue::GetBattingAdvantage() const {
    double advantage = 5.0; // Neutral
    
    // Pitch type effects
    switch (pitchCharacteristics.type) {
        case PitchType::BATTER_FRIENDLY:
            advantage += 1.5;
            break;
        case PitchType::BOWLER_FRIENDLY:
            advantage -= 1.5;
            break;
        case PitchType::SPIN_FRIENDLY:
            advantage -= 0.5;
            break;
        case PitchType::SEAM_FRIENDLY:
            advantage -= 1.0;
            break;
        default:
            break;
    }
    
    // Weather effects
    if (currentWeather.isRaining) {
        advantage -= 1.0;
    }
    if (currentWeather.windSpeed > 20) {
        advantage -= 0.5;
    }
    
    return std::max(1.0, std::min(10.0, advantage));
}

double Venue::GetBowlingAdvantage() const {
    return 10.0 - GetBattingAdvantage(); // Inverse of batting advantage
}

void Venue::ApplyWeatherEffects() {
    // Rain affects pitch moisture
    if (currentWeather.isRaining) {
        pitchCharacteristics.moisture = std::min(10, pitchCharacteristics.moisture + 2);
    }
    
    // Wind affects ball movement
    if (currentWeather.windSpeed > 15) {
        pitchCharacteristics.isSeaming = true;
    }
    
    // Temperature affects pitch hardness
    if (currentWeather.temperature > 35) {
        pitchCharacteristics.hardness = std::min(10, pitchCharacteristics.hardness + 1);
    } else if (currentWeather.temperature < 15) {
        pitchCharacteristics.hardness = std::max(1, pitchCharacteristics.hardness - 1);
    }
}

bool Venue::IsMatchPlayable() const {
    // Check if weather allows play
    if (currentWeather.isRaining && currentWeather.rainProbability > 0.7) {
        return false;
    }
    
    if (currentWeather.visibility < 3) {
        return false;
    }
    
    return true;
}

int Venue::GetRainDelay() const {
    if (!currentWeather.isRaining) {
        return 0;
    }
    
    // Calculate delay based on rain intensity
    int delay = static_cast<int>(currentWeather.rainProbability * 60); // 0-60 minutes
    return delay;
}

double Venue::GetWeatherImpact() const {
    double impact = 1.0; // No impact
    
    if (currentWeather.isRaining) {
        impact *= 0.7; // 30% reduction in performance
    }
    
    if (currentWeather.windSpeed > 20) {
        impact *= 0.9; // 10% reduction
    }
    
    if (currentWeather.temperature > 35 || currentWeather.temperature < 10) {
        impact *= 0.8; // 20% reduction in extreme temperatures
    }
    
    return impact;
}

bool Venue::HasFloodlights() const {
    return (stats.facilities & 0x01) != 0; // Bit flag for floodlights
}

bool Venue::HasCoveredStands() const {
    return (stats.facilities & 0x02) != 0; // Bit flag for covered stands
}

bool Venue::HasPracticeNets() const {
    return (stats.facilities & 0x04) != 0; // Bit flag for practice nets
}

int Venue::CalculateRevenue(int attendance) const {
    double revenue = attendance * stats.ticketPrice;
    
    // Bonus for high attendance
    if (attendance > stats.capacity * 0.8) {
        revenue *= 1.2; // 20% bonus for near capacity
    }
    
    // Merchandise sales (assume 10% of attendees buy merchandise)
    revenue += attendance * 0.1 * 500; // 500 rupees average per item
    
    return static_cast<int>(revenue);
}

void Venue::UpdateReputation(int change) {
    stats.reputation = std::max(1, std::min(100, stats.reputation + change));
}

std::string Venue::GetReputationLevel() const {
    if (stats.reputation >= 90) return "Legendary";
    if (stats.reputation >= 80) return "World Class";
    if (stats.reputation >= 70) return "Excellent";
    if (stats.reputation >= 60) return "Good";
    if (stats.reputation >= 50) return "Average";
    if (stats.reputation >= 40) return "Below Average";
    if (stats.reputation >= 30) return "Poor";
    return "Terrible";
}

void Venue::InitializePitchCharacteristics() {
    // Set default pitch characteristics based on venue type
    switch (type) {
        case VenueType::INTERNATIONAL:
            pitchCharacteristics.type = PitchType::BALANCED;
            pitchCharacteristics.hardness = 7;
            pitchCharacteristics.moisture = 5;
            pitchCharacteristics.grass = 6;
            pitchCharacteristics.wear = 3;
            pitchCharacteristics.averageScore = 280.0;
            pitchCharacteristics.bowlingAverage = 25.0;
            break;
            
        case VenueType::DOMESTIC:
            pitchCharacteristics.type = PitchType::BATTER_FRIENDLY;
            pitchCharacteristics.hardness = 6;
            pitchCharacteristics.moisture = 4;
            pitchCharacteristics.grass = 4;
            pitchCharacteristics.wear = 4;
            pitchCharacteristics.averageScore = 300.0;
            pitchCharacteristics.bowlingAverage = 28.0;
            break;
            
        case VenueType::FRANCHISE:
            pitchCharacteristics.type = PitchType::BALANCED;
            pitchCharacteristics.hardness = 7;
            pitchCharacteristics.moisture = 5;
            pitchCharacteristics.grass = 5;
            pitchCharacteristics.wear = 3;
            pitchCharacteristics.averageScore = 290.0;
            pitchCharacteristics.bowlingAverage = 26.0;
            break;
            
        case VenueType::CLUB:
            pitchCharacteristics.type = PitchType::BOWLER_FRIENDLY;
            pitchCharacteristics.hardness = 8;
            pitchCharacteristics.moisture = 6;
            pitchCharacteristics.grass = 7;
            pitchCharacteristics.wear = 5;
            pitchCharacteristics.averageScore = 250.0;
            pitchCharacteristics.bowlingAverage = 22.0;
            break;
    }
    
    // Set spin and seam effectiveness
    pitchCharacteristics.isSpinning = (pitchCharacteristics.type == PitchType::SPIN_FRIENDLY);
    pitchCharacteristics.isSeaming = (pitchCharacteristics.type == PitchType::SEAM_FRIENDLY);
    pitchCharacteristics.isBouncy = (pitchCharacteristics.hardness > 7);
    
    pitchCharacteristics.spinEffectiveness = pitchCharacteristics.isSpinning ? 0.8 : 0.5;
    pitchCharacteristics.seamEffectiveness = pitchCharacteristics.isSeaming ? 0.8 : 0.5;
}

void Venue::GenerateWeatherPattern() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> tempDist(15, 35);
    std::uniform_int_distribution<> humidityDist(40, 80);
    std::uniform_int_distribution<> windDist(5, 15);
    std::uniform_real_distribution<> rainProb(0.0, 0.3);
    
    currentWeather.temperature = tempDist(gen);
    currentWeather.humidity = humidityDist(gen);
    currentWeather.windSpeed = windDist(gen);
    currentWeather.windDirection = "North";
    currentWeather.isOvercast = false;
    currentWeather.isRaining = false;
    currentWeather.visibility = 10;
    currentWeather.rainProbability = rainProb(gen);
    
    // Set weather type
    if (currentWeather.rainProbability > 0.2) {
        currentWeather.type = WeatherType::RAINY;
        currentWeather.isRaining = true;
    } else if (currentWeather.humidity > 70) {
        currentWeather.type = WeatherType::HUMID;
    } else if (currentWeather.windSpeed > 12) {
        currentWeather.type = WeatherType::WINDY;
    } else {
        currentWeather.type = WeatherType::SUNNY;
    }
}

void Venue::UpdatePitchWear() {
    // Increase wear after each match
    pitchCharacteristics.wear = std::min(10, pitchCharacteristics.wear + 2);
    
    // Reduce moisture
    pitchCharacteristics.moisture = std::max(1, pitchCharacteristics.moisture - 1);
}

void Venue::CalculatePitchEffects() {
    // Recalculate spin and seam effectiveness based on current conditions
    pitchCharacteristics.isSpinning = (pitchCharacteristics.wear > 6 || 
                                      pitchCharacteristics.type == PitchType::SPIN_FRIENDLY);
    pitchCharacteristics.isSeaming = (pitchCharacteristics.moisture > 6 || 
                                     pitchCharacteristics.grass > 5 ||
                                     pitchCharacteristics.type == PitchType::SEAM_FRIENDLY);
    pitchCharacteristics.isBouncy = (pitchCharacteristics.hardness > 7);
    
    // Update effectiveness values
    pitchCharacteristics.spinEffectiveness = pitchCharacteristics.isSpinning ? 0.8 : 0.5;
    pitchCharacteristics.seamEffectiveness = pitchCharacteristics.isSeaming ? 0.8 : 0.5;
}

std::string Venue::FormatMatchResult(const std::string& team1, const std::string& team2, 
                                    int score1, int score2, const std::string& winner) {
    std::ostringstream result;
    result << team1 << " " << score1 << " vs " << team2 << " " << score2;
    if (!winner.empty()) {
        result << " - " << winner << " won";
    }
    return result.str();
}

std::string Venue::ToJSON() const {
    std::ostringstream json;
    json << "{";
    json << "\"name\":\"" << name << "\",";
    json << "\"location\":\"" << location << "\",";
    json << "\"type\":" << static_cast<int>(type) << ",";
    json << "\"capacity\":" << stats.capacity << ",";
    json << "\"reputation\":" << stats.reputation;
    json << "}";
    return json.str();
}

std::unique_ptr<Venue> Venue::FromJSON(const std::string& json) {
    // TODO: Implement JSON parsing
    return std::make_unique<Venue>("Unknown Venue", "Unknown Location", VenueType::CLUB);
} 