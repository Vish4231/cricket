# API Integration Guide

## Overview

The Cricket Manager game includes comprehensive API integration to fetch real-time cricket data from multiple sources. This allows the game to provide up-to-date player statistics, live match data, team information, and more.

## Supported APIs

### 1. CricAPI (Primary)
- **Base URL**: `https://api.cricapi.com/v1`
- **Features**: Players, Teams, Matches, Venues, Stats, Rankings, Tournaments, News
- **Rate Limit**: 60 requests/minute, 1000 requests/hour
- **Documentation**: [CricAPI Documentation](https://cricapi.com/docs/)

### 2. Cricket Data Org (Fallback)
- **Base URL**: `https://api.cricket-data.org/v2`
- **Features**: Players, Teams, Matches, Venues, Series
- **Rate Limit**: 30 requests/minute, 500 requests/hour
- **Documentation**: [Cricket Data Org](https://cricketdata.org/)

### 3. ESPN Cricinfo (Alternative)
- **Base URL**: `https://hs-consumer-api.espncricinfo.com/v1`
- **Features**: Players, Teams, Matches, Series
- **Rate Limit**: 20 requests/minute, 300 requests/hour

## Setup

### 1. Get API Keys

#### CricAPI
1. Visit [CricAPI](https://cricapi.com/)
2. Sign up for a free account
3. Get your API key from the dashboard
4. Free tier includes 100 requests/day

#### Cricket Data Org
1. Visit [Cricket Data Org](https://cricketdata.org/)
2. Register for an account
3. Get your API key
4. Free tier includes limited requests

### 2. Configure API Keys

#### Method 1: In-Game Settings
1. Launch Cricket Manager
2. Go to Settings (F6)
3. Enter your API key in the "API Key" field
4. Click "Save Settings"

#### Method 2: Configuration File
Edit `config/api_config.json`:
```json
{
  "api_keys": {
    "cricapi": "your_cricapi_key_here",
    "cricket_data_org": "your_cricket_data_org_key_here"
  }
}
```

#### Method 3: Environment Variable
```bash
export CRICKET_API_KEY="your_api_key_here"
```

## Features

### Real-Time Data
- **Live Matches**: Get current match scores and commentary
- **Player Stats**: Real-time player statistics and rankings
- **Team Information**: Current team rosters and performance
- **Venue Details**: Stadium information and pitch conditions

### Historical Data
- **Player Careers**: Complete career statistics
- **Match History**: Past match results and scorecards
- **Tournament Records**: Historical tournament data
- **Team Performance**: Historical team statistics

### News and Updates
- **Cricket News**: Latest cricket news and updates
- **Player News**: Player-specific news and updates
- **Tournament News**: Tournament-related news

## Data Manager Class

The `DataManager` class handles all API interactions:

```cpp
// Initialize with API key
DataManager dataManager;
dataManager.setApiKey("your_api_key");

// Fetch players
std::vector<Player> players = dataManager.fetchPlayers();

// Fetch teams
std::vector<Team> teams = dataManager.fetchTeams();

// Fetch live matches
json liveMatches = dataManager.fetchLiveMatches();

// Search players
std::vector<Player> searchResults = dataManager.searchPlayers("Virat Kohli");
```

## Caching System

The DataManager includes an intelligent caching system:

- **Automatic Caching**: Responses are cached to reduce API calls
- **Configurable Expiry**: Set cache duration (default: 5 minutes)
- **Smart Invalidation**: Cache is invalidated when data is stale
- **Memory Efficient**: LRU cache with size limits

```cpp
// Enable/disable caching
dataManager.enableCaching(true);

// Set cache expiry (in seconds)
dataManager.setCacheExpiry(300);

// Clear cache
dataManager.clearCache();
```

## Rate Limiting

Built-in rate limiting prevents API quota exhaustion:

- **Automatic Throttling**: Respects API rate limits
- **Request Queuing**: Queues requests when rate limit is reached
- **Configurable Limits**: Set custom rate limits per API
- **Error Handling**: Graceful handling of rate limit errors

```cpp
// Set rate limit (requests per minute)
dataManager.setRateLimit(60);

// Check if rate limited
if (dataManager.isRateLimited()) {
    // Handle rate limiting
}
```

## Error Handling

Comprehensive error handling for API failures:

```cpp
// Check for errors
if (dataManager.hasError()) {
    std::string error = dataManager.getLastError();
    // Handle error
}

// Try-catch for API calls
try {
    auto players = dataManager.fetchPlayers();
} catch (const std::exception& e) {
    // Handle API error
    std::cerr << "API Error: " << e.what() << std::endl;
}
```

## Offline Mode

The game can work without internet connection:

- **Fallback Data**: Uses local data when API is unavailable
- **Offline Mode**: Completely offline operation
- **Data Sync**: Syncs data when connection is restored
- **Local Storage**: Stores frequently used data locally

## Configuration

### API Configuration File
The `config/api_config.json` file contains all API settings:

```json
{
  "api_config": {
    "cricapi": {
      "base_url": "https://api.cricapi.com/v1",
      "rate_limit": {
        "requests_per_minute": 60
      }
    }
  },
  "default_api": "cricapi",
  "features": {
    "live_data": true,
    "historical_data": true
  }
}
```

### Feature Flags
Enable/disable specific features:

```json
{
  "features": {
    "live_data": true,
    "player_stats": true,
    "news_feed": true,
    "rankings": true
  }
}
```

## Usage Examples

### Fetching Player Data
```cpp
// Get all players
auto players = dataManager.fetchPlayers();

// Get player by ID
Player player = dataManager.fetchPlayerById("player_id");

// Search players
auto results = dataManager.searchPlayers("Kohli");
```

### Fetching Team Data
```cpp
// Get all teams
auto teams = dataManager.fetchTeams();

// Get team by ID
Team team = dataManager.fetchTeamById("team_id");

// Get team players
auto players = dataManager.fetchTeamPlayers("team_id");
```

### Live Match Data
```cpp
// Get live matches
auto liveMatches = dataManager.fetchLiveMatches();

// Get match details
auto matchDetails = dataManager.fetchMatchDetails("match_id");

// Get scorecard
auto scorecard = dataManager.fetchMatchScorecard("match_id");
```

### Statistics and Rankings
```cpp
// Get player stats
auto stats = dataManager.fetchPlayerStats("player_id", "odi");

// Get team stats
auto teamStats = dataManager.fetchTeamStats("team_id");

// Get rankings
auto rankings = dataManager.fetchRankings("batting");
```

## Troubleshooting

### Common Issues

1. **API Key Invalid**
   - Verify your API key is correct
   - Check if the key has expired
   - Ensure you have sufficient quota

2. **Rate Limit Exceeded**
   - Wait for rate limit to reset
   - Reduce request frequency
   - Enable caching to reduce API calls

3. **Network Issues**
   - Check internet connection
   - Verify API endpoints are accessible
   - Try fallback APIs

4. **Data Not Loading**
   - Check API response format
   - Verify data parsing logic
   - Enable debug logging

### Debug Mode

Enable debug logging to troubleshoot issues:

```cpp
// Enable debug mode (if implemented)
dataManager.setDebugMode(true);
```

### Logs

Check logs for API-related issues:
- API request/response logs
- Error messages
- Rate limiting information
- Cache hit/miss statistics

## Best Practices

1. **API Key Security**
   - Never commit API keys to version control
   - Use environment variables or secure storage
   - Rotate keys regularly

2. **Rate Limiting**
   - Respect API rate limits
   - Implement exponential backoff
   - Use caching to reduce requests

3. **Error Handling**
   - Always handle API errors gracefully
   - Provide fallback data when possible
   - Log errors for debugging

4. **Data Management**
   - Cache frequently accessed data
   - Implement data validation
   - Handle data format changes

## Future Enhancements

- **WebSocket Support**: Real-time data streaming
- **GraphQL Integration**: More efficient data fetching
- **Multiple API Support**: Additional cricket APIs
- **Advanced Caching**: Redis/Memcached integration
- **Data Analytics**: Usage analytics and optimization

## Support

For API-related issues:
1. Check the troubleshooting section
2. Review API documentation
3. Contact API providers for quota/access issues
4. Check game logs for detailed error information 