#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "Player.h"
#include "Team.h"
#include "Venue.h"

using json = nlohmann::json;

class DataManager {
public:
    DataManager();
    ~DataManager();
    
    // API Configuration
    void setApiKey(const std::string& apiKey);
    void setBaseUrl(const std::string& baseUrl);
    
    // Player Data
    std::vector<Player> fetchPlayers(const std::string& teamId = "");
    Player fetchPlayerById(const std::string& playerId);
    std::vector<Player> searchPlayers(const std::string& query);
    
    // Team Data
    std::vector<Team> fetchTeams();
    Team fetchTeamById(const std::string& teamId);
    std::vector<Player> fetchTeamPlayers(const std::string& teamId);
    
    // Match Data
    json fetchLiveMatches();
    json fetchMatchDetails(const std::string& matchId);
    json fetchMatchScorecard(const std::string& matchId);
    
    // Venue Data
    std::vector<Venue> fetchVenues();
    Venue fetchVenueById(const std::string& venueId);
    
    // Statistics
    json fetchPlayerStats(const std::string& playerId, const std::string& format = "all");
    json fetchTeamStats(const std::string& teamId);
    json fetchRankings(const std::string& category = "batting");
    
    // Tournament Data
    json fetchTournaments();
    json fetchTournamentDetails(const std::string& tournamentId);
    json fetchTournamentStandings(const std::string& tournamentId);
    
    // News and Updates
    json fetchCricketNews();
    json fetchPlayerNews(const std::string& playerId);
    
    // Caching
    void enableCaching(bool enable);
    void setCacheExpiry(int seconds);
    void clearCache();
    
    // Error Handling
    std::string getLastError() const;
    bool hasError() const;
    
    // Rate Limiting
    void setRateLimit(int requestsPerMinute);
    bool isRateLimited() const;

private:
    // HTTP Request Methods
    json makeApiRequest(const std::string& endpoint, const std::map<std::string, std::string>& params = {});
    std::string buildUrl(const std::string& endpoint, const std::map<std::string, std::string>& params);
    
    // CURL Callback
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    
    // Data Parsing
    Player parsePlayerData(const json& data);
    Team parseTeamData(const json& data);
    Venue parseVenueData(const json& data);
    
    // Cache Management
    struct CacheEntry {
        json data;
        time_t timestamp;
        int expiry;
    };
    
    json getCachedData(const std::string& key);
    void setCachedData(const std::string& key, const json& data);
    bool isCacheValid(const std::string& key);
    
    // Rate Limiting
    struct RateLimit {
        int requestsPerMinute;
        std::vector<time_t> requestTimes;
    };
    
    bool checkRateLimit();
    void updateRateLimit();
    
    // Member Variables
    CURL* curl;
    std::string apiKey;
    std::string baseUrl;
    std::string lastError;
    bool cachingEnabled;
    int cacheExpirySeconds;
    std::map<std::string, CacheEntry> cache;
    RateLimit rateLimit;
    bool rateLimitEnabled;
    
    // API Endpoints
    static const std::string PLAYERS_ENDPOINT;
    static const std::string TEAMS_ENDPOINT;
    static const std::string MATCHES_ENDPOINT;
    static const std::string VENUES_ENDPOINT;
    static const std::string STATS_ENDPOINT;
    static const std::string RANKINGS_ENDPOINT;
    static const std::string TOURNAMENTS_ENDPOINT;
    static const std::string NEWS_ENDPOINT;
}; 