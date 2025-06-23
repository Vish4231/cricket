#include "DataManager.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// API Endpoints
const std::string DataManager::PLAYERS_ENDPOINT = "/players";
const std::string DataManager::TEAMS_ENDPOINT = "/teams";
const std::string DataManager::MATCHES_ENDPOINT = "/matches";
const std::string DataManager::VENUES_ENDPOINT = "/venues";
const std::string DataManager::STATS_ENDPOINT = "/stats";
const std::string DataManager::RANKINGS_ENDPOINT = "/rankings";
const std::string DataManager::TOURNAMENTS_ENDPOINT = "/tournaments";
const std::string DataManager::NEWS_ENDPOINT = "/news";

DataManager::DataManager() : 
    curl(nullptr),
    cachingEnabled(true),
    cacheExpirySeconds(300), // 5 minutes default
    rateLimitEnabled(true) {
    
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (!curl) {
        lastError = "Failed to initialize CURL";
        return;
    }
    
    // Set default API configuration
    baseUrl = "https://api.cricapi.com/v1"; // Example API endpoint
    apiKey = ""; // Will be set by user
    
    // Configure rate limiting
    rateLimit.requestsPerMinute = 60;
    rateLimit.requestTimes.clear();
}

DataManager::~DataManager() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

void DataManager::setApiKey(const std::string& key) {
    apiKey = key;
}

void DataManager::setBaseUrl(const std::string& url) {
    baseUrl = url;
}

std::vector<Player> DataManager::fetchPlayers(const std::string& teamId) {
    std::vector<Player> players;
    
    try {
        std::map<std::string, std::string> params;
        if (!teamId.empty()) {
            params["teamId"] = teamId;
        }
        
        json response = makeApiRequest(PLAYERS_ENDPOINT, params);
        
        if (response.contains("data") && response["data"].is_array()) {
            for (const auto& playerData : response["data"]) {
                Player player = parsePlayerData(playerData);
                players.push_back(player);
            }
        }
        
    } catch (const std::exception& e) {
        lastError = "Error fetching players: " + std::string(e.what());
    }
    
    return players;
}

Player DataManager::fetchPlayerById(const std::string& playerId) {
    try {
        std::map<std::string, std::string> params;
        params["id"] = playerId;
        
        json response = makeApiRequest(PLAYERS_ENDPOINT, params);
        
        if (response.contains("data")) {
            return parsePlayerData(response["data"]);
        }
        
    } catch (const std::exception& e) {
        lastError = "Error fetching player: " + std::string(e.what());
    }
    
    return Player("Unknown Player", 25, PlayerRole::BATSMAN); // Return default player on error
}

std::vector<Player> DataManager::searchPlayers(const std::string& query) {
    std::vector<Player> players;
    
    try {
        std::map<std::string, std::string> params;
        params["search"] = query;
        
        json response = makeApiRequest(PLAYERS_ENDPOINT, params);
        
        if (response.contains("data") && response["data"].is_array()) {
            for (const auto& playerData : response["data"]) {
                Player player = parsePlayerData(playerData);
                players.push_back(player);
            }
        }
        
    } catch (const std::exception& e) {
        lastError = "Error searching players: " + std::string(e.what());
    }
    
    return players;
}

std::vector<Team> DataManager::fetchTeams() {
    std::vector<Team> teams;
    
    try {
        json response = makeApiRequest(TEAMS_ENDPOINT);
        
        if (response.contains("data") && response["data"].is_array()) {
            for (const auto& teamData : response["data"]) {
                Team team = parseTeamData(teamData);
                teams.push_back(team);
            }
        }
        
    } catch (const std::exception& e) {
        lastError = "Error fetching teams: " + std::string(e.what());
    }
    
    return teams;
}

Team DataManager::fetchTeamById(const std::string& teamId) {
    try {
        std::map<std::string, std::string> params;
        params["id"] = teamId;
        
        json response = makeApiRequest(TEAMS_ENDPOINT, params);
        
        if (response.contains("data")) {
            return parseTeamData(response["data"]);
        }
        
    } catch (const std::exception& e) {
        lastError = "Error fetching team: " + std::string(e.what());
    }
    
    return Team("Unknown Team", TeamType::DOMESTIC); // Return default team on error
}

std::vector<Player> DataManager::fetchTeamPlayers(const std::string& teamId) {
    return fetchPlayers(teamId);
}

json DataManager::fetchLiveMatches() {
    try {
        std::map<std::string, std::string> params;
        params["status"] = "live";
        
        return makeApiRequest(MATCHES_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching live matches: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchMatchDetails(const std::string& matchId) {
    try {
        std::map<std::string, std::string> params;
        params["id"] = matchId;
        
        return makeApiRequest(MATCHES_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching match details: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchMatchScorecard(const std::string& matchId) {
    try {
        std::map<std::string, std::string> params;
        params["id"] = matchId;
        params["scorecard"] = "true";
        
        return makeApiRequest(MATCHES_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching scorecard: " + std::string(e.what());
        return json();
    }
}

std::vector<Venue> DataManager::fetchVenues() {
    std::vector<Venue> venues;
    
    try {
        json response = makeApiRequest(VENUES_ENDPOINT);
        
        if (response.contains("data") && response["data"].is_array()) {
            for (const auto& venueData : response["data"]) {
                Venue venue = parseVenueData(venueData);
                venues.push_back(venue);
            }
        }
        
    } catch (const std::exception& e) {
        lastError = "Error fetching venues: " + std::string(e.what());
    }
    
    return venues;
}

Venue DataManager::fetchVenueById(const std::string& venueId) {
    try {
        std::map<std::string, std::string> params;
        params["id"] = venueId;
        
        json response = makeApiRequest(VENUES_ENDPOINT, params);
        
        if (response.contains("data")) {
            return parseVenueData(response["data"]);
        }
        
    } catch (const std::exception& e) {
        lastError = "Error fetching venue: " + std::string(e.what());
    }
    
    return Venue("Unknown Venue", "Unknown Location", VenueType::DOMESTIC); // Return default venue on error
}

json DataManager::fetchPlayerStats(const std::string& playerId, const std::string& format) {
    try {
        std::map<std::string, std::string> params;
        params["playerId"] = playerId;
        params["format"] = format;
        
        return makeApiRequest(STATS_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching player stats: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchTeamStats(const std::string& teamId) {
    try {
        std::map<std::string, std::string> params;
        params["teamId"] = teamId;
        
        return makeApiRequest(STATS_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching team stats: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchRankings(const std::string& category) {
    try {
        std::map<std::string, std::string> params;
        params["category"] = category;
        
        return makeApiRequest(RANKINGS_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching rankings: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchTournaments() {
    try {
        return makeApiRequest(TOURNAMENTS_ENDPOINT);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching tournaments: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchTournamentDetails(const std::string& tournamentId) {
    try {
        std::map<std::string, std::string> params;
        params["id"] = tournamentId;
        
        return makeApiRequest(TOURNAMENTS_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching tournament details: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchTournamentStandings(const std::string& tournamentId) {
    try {
        std::map<std::string, std::string> params;
        params["id"] = tournamentId;
        params["standings"] = "true";
        
        return makeApiRequest(TOURNAMENTS_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching tournament standings: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchCricketNews() {
    try {
        return makeApiRequest(NEWS_ENDPOINT);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching cricket news: " + std::string(e.what());
        return json();
    }
}

json DataManager::fetchPlayerNews(const std::string& playerId) {
    try {
        std::map<std::string, std::string> params;
        params["playerId"] = playerId;
        
        return makeApiRequest(NEWS_ENDPOINT, params);
        
    } catch (const std::exception& e) {
        lastError = "Error fetching player news: " + std::string(e.what());
        return json();
    }
}

void DataManager::enableCaching(bool enable) {
    cachingEnabled = enable;
}

void DataManager::setCacheExpiry(int seconds) {
    cacheExpirySeconds = seconds;
}

void DataManager::clearCache() {
    cache.clear();
}

std::string DataManager::getLastError() const {
    return lastError;
}

bool DataManager::hasError() const {
    return !lastError.empty();
}

void DataManager::setRateLimit(int requestsPerMinute) {
    rateLimit.requestsPerMinute = requestsPerMinute;
}

bool DataManager::isRateLimited() const {
    if (!rateLimitEnabled) {
        return false;
    }
    
    time_t now = std::time(nullptr);
    
    // Count requests in the last minute
    int recentRequests = 0;
    for (time_t requestTime : rateLimit.requestTimes) {
        if ((now - requestTime) < 60) {
            recentRequests++;
        }
    }
    
    return recentRequests >= rateLimit.requestsPerMinute;
}

json DataManager::makeApiRequest(const std::string& endpoint, const std::map<std::string, std::string>& params) {
    // Check rate limiting
    if (rateLimitEnabled && !checkRateLimit()) {
        throw std::runtime_error("Rate limit exceeded");
    }
    
    // Check cache first
    std::string cacheKey = endpoint;
    for (const auto& param : params) {
        cacheKey += "_" + param.first + "_" + param.second;
    }
    
    if (cachingEnabled) {
        json cachedData = getCachedData(cacheKey);
        if (!cachedData.is_null()) {
            return cachedData;
        }
    }
    
    // Build URL
    std::string url = buildUrl(endpoint, params);
    
    // Prepare CURL
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CricketManager/1.0");
    
    // Add headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (!apiKey.empty()) {
        headers = curl_slist_append(headers, ("X-API-Key: " + apiKey).c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // Make request
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    
    if (res != CURLE_OK) {
        lastError = "CURL error: " + std::string(curl_easy_strerror(res));
        throw std::runtime_error(lastError);
    }
    
    // Parse response
    json responseJson;
    try {
        responseJson = json::parse(response);
    } catch (const json::parse_error& e) {
        lastError = "JSON parse error: " + std::string(e.what());
        throw std::runtime_error(lastError);
    }
    
    // Check for API errors
    if (responseJson.contains("error") && responseJson["error"].get<bool>()) {
        std::string errorMsg = responseJson.value("message", "Unknown API error");
        lastError = "API error: " + errorMsg;
        throw std::runtime_error(lastError);
    }
    
    // Cache the response
    if (cachingEnabled) {
        setCachedData(cacheKey, responseJson);
    }
    
    // Update rate limiting
    if (rateLimitEnabled) {
        updateRateLimit();
    }
    
    return responseJson;
}

std::string DataManager::buildUrl(const std::string& endpoint, const std::map<std::string, std::string>& params) {
    std::string url = baseUrl + endpoint;
    
    if (!params.empty()) {
        url += "?";
        bool first = true;
        for (const auto& param : params) {
            if (!first) url += "&";
            url += param.first + "=" + param.second;
            first = false;
        }
    }
    
    return url;
}

size_t DataManager::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Player DataManager::parsePlayerData(const json& data) {
    try {
        std::string name = data.value("name", "Unknown Player");
        int age = data.value("age", 25);
        std::string roleStr = data.value("role", "Batsman");
        
        PlayerRole role = PlayerRole::BATSMAN;
        if (roleStr == "Bowler") role = PlayerRole::BOWLER;
        else if (roleStr == "AllRounder") role = PlayerRole::ALL_ROUNDER;
        else if (roleStr == "WicketKeeper") role = PlayerRole::WICKET_KEEPER;
        
        Player player(name, age, role);
        
        // Set additional data using available methods
        player.SetCurrentTeam(data.value("team", ""));
        
        // Note: Most stats would need to be set through the constructor or other methods
        // For now, we'll use the basic constructor
        
        return player;
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing player data: " << e.what() << std::endl;
        return Player("Unknown Player", 25, PlayerRole::BATSMAN);
    }
}

Team DataManager::parseTeamData(const json& data) {
    try {
        std::string name = data.value("name", "Unknown Team");
        std::string typeStr = data.value("type", "DOMESTIC");
        
        TeamType type = TeamType::DOMESTIC;
        if (typeStr == "INTERNATIONAL") type = TeamType::INTERNATIONAL;
        else if (typeStr == "FRANCHISE") type = TeamType::FRANCHISE;
        else if (typeStr == "CLUB") type = TeamType::CLUB;
        
        Team team(name, type);
        
        // Set additional data using available methods
        team.SetHomeVenue(data.value("home_venue", ""));
        
        // Note: Players would need to be added through AddPlayer method
        // For now, we'll use the basic constructor
        
        return team;
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing team data: " << e.what() << std::endl;
        return Team("Unknown Team", TeamType::DOMESTIC);
    }
}

Venue DataManager::parseVenueData(const json& data) {
    try {
        std::string name = data.value("name", "Unknown Venue");
        std::string location = data.value("location", "Unknown Location");
        std::string typeStr = data.value("type", "DOMESTIC");
        
        VenueType type = VenueType::DOMESTIC;
        if (typeStr == "INTERNATIONAL") type = VenueType::INTERNATIONAL;
        else if (typeStr == "FRANCHISE") type = VenueType::FRANCHISE;
        else if (typeStr == "CLUB") type = VenueType::CLUB;
        else if (typeStr == "STADIUM") type = VenueType::STADIUM;
        
        Venue venue(name, location, type);
        
        // Note: Additional venue data would need to be set through available methods
        // For now, we'll use the basic constructor
        
        return venue;
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing venue data: " << e.what() << std::endl;
        return Venue("Unknown Venue", "Unknown Location", VenueType::DOMESTIC);
    }
}

json DataManager::getCachedData(const std::string& key) {
    auto it = cache.find(key);
    if (it != cache.end() && isCacheValid(key)) {
        return it->second.data;
    }
    return json();
}

void DataManager::setCachedData(const std::string& key, const json& data) {
    CacheEntry entry;
    entry.data = data;
    entry.timestamp = std::time(nullptr);
    entry.expiry = cacheExpirySeconds;
    
    cache[key] = entry;
}

bool DataManager::isCacheValid(const std::string& key) {
    auto it = cache.find(key);
    if (it == cache.end()) {
        return false;
    }
    
    time_t now = std::time(nullptr);
    return (now - it->second.timestamp) < it->second.expiry;
}

bool DataManager::checkRateLimit() {
    if (!rateLimitEnabled) {
        return true;
    }
    
    time_t now = std::time(nullptr);
    
    // Remove old requests (older than 1 minute)
    rateLimit.requestTimes.erase(
        std::remove_if(rateLimit.requestTimes.begin(), rateLimit.requestTimes.end(),
            [now](time_t time) { return (now - time) >= 60; }),
        rateLimit.requestTimes.end()
    );
    
    return rateLimit.requestTimes.size() < rateLimit.requestsPerMinute;
}

void DataManager::updateRateLimit() {
    if (rateLimitEnabled) {
        rateLimit.requestTimes.push_back(std::time(nullptr));
    }
} 