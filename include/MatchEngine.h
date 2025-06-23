#pragma once

#include "Player.h"
#include "Team.h"
#include "Venue.h"
#include <vector>
#include <memory>
#include <functional>
#include <random>

enum class MatchType {
    TEST,
    ODI,
    T20
};

enum class MatchFormat {
    FIRST_INNINGS,
    SECOND_INNINGS,
    SUPER_OVER
};

enum class BallResult {
    DOT_BALL,
    SINGLE,
    DOUBLE,
    TRIPLE,
    FOUR,
    SIX,
    WICKET,
    WIDE,
    NO_BALL,
    BYE,
    LEG_BYE,
    RUN_OUT
};

enum class WicketType {
    BOWLED,
    CAUGHT,
    LBW,
    RUN_OUT,
    STUMPED,
    HIT_WICKET,
    OBSTRUCTING_THE_FIELD,
    HANDLED_THE_BALL,
    HIT_THE_BALL_TWICE,
    TIMED_OUT
};

enum class FieldingPosition {
    SLIP,
    GULLY,
    THIRD_MAN,
    POINT,
    COVER,
    EXTRA_COVER,
    MID_OFF,
    MID_ON,
    MID_WICKET,
    SQUARE_LEG,
    FINE_LEG,
    DEEP_SQUARE_LEG,
    DEEP_MID_WICKET,
    LONG_ON,
    LONG_OFF,
    DEEP_COVER,
    DEEP_POINT,
    DEEP_THIRD_MAN,
    WICKET_KEEPER,
    BOWLER
};

struct BallEvent {
    BallResult result;
    WicketType wicketType;
    int runs;
    std::string striker;
    std::string nonStriker;
    std::string bowler;
    std::string fielder;
    FieldingPosition fieldingPosition;
    bool isExtra;
    std::string commentary;
};

struct Over {
    int overNumber;
    std::vector<BallEvent> balls;
    int runs;
    int wickets;
    int extras;
    std::string bowler;
};

struct Innings {
    int inningsNumber;
    std::string battingTeam;
    std::string bowlingTeam;
    std::vector<Over> overs;
    int totalRuns;
    int totalWickets;
    int totalOvers;
    double runRate;
    std::vector<std::string> battingOrder;
    std::vector<std::string> bowlingOrder;
    bool isComplete;
};

struct MatchState {
    MatchType matchType;
    MatchFormat format;
    int currentInnings;
    int currentOver;
    int currentBall;
    std::string striker;
    std::string nonStriker;
    std::string bowler;
    std::string fieldingTeam;
    std::string battingTeam;
    std::string bowlingTeam;
    bool isMatchComplete;
    std::string winner;
    std::string result;
};

struct PitchConditions {
    int hardness; // 1-10
    int moisture; // 1-10
    int grass; // 1-10
    int wear; // 1-10
    bool isSpinning;
    bool isSeaming;
    bool isBouncy;
};

struct WeatherConditions {
    int temperature; // Celsius
    int humidity; // Percentage
    int windSpeed; // km/h
    std::string windDirection;
    bool isOvercast;
    bool isRaining;
    int visibility; // 1-10
};

class MatchEngine {
public:
    MatchEngine();
    ~MatchEngine();
    
    // Match setup
    void InitializeMatch(Team* team1, Team* team2, Venue* venue, MatchType type);
    void SetPitchConditions(const PitchConditions& conditions);
    void SetWeatherConditions(const WeatherConditions& conditions);
    
    // Match simulation
    void SimulateBall();
    void SimulateOver();
    void SimulateInnings();
    void SimulateMatch();
    
    // Match state
    const MatchState& GetMatchState() const { return matchState; }
    const std::vector<Innings>& GetInnings() const { return innings; }
    const std::vector<BallEvent>& GetBallHistory() const { return ballHistory; }
    
    // Current state queries
    int GetCurrentScore() const;
    int GetCurrentWickets() const;
    double GetCurrentRunRate() const;
    int GetRequiredRuns() const;
    int GetRemainingOvers() const;
    
    // Player performance
    void UpdatePlayerStats(const BallEvent& event);
    std::map<std::string, PlayerStats> GetPlayerMatchStats() const;
    
    // Tactical decisions
    void SetFieldingPositions(const std::map<std::string, FieldingPosition>& positions);
    void SetBowlingChange(const std::string& newBowler);
    void SetBattingOrder(const std::vector<std::string>& order);
    
    // Match control
    void PauseMatch();
    void ResumeMatch();
    void EndMatch();
    bool IsMatchPaused() const { return isPaused; }
    
    // Update method
    void Update(float deltaTime);
    
    // Commentary and events
    void AddCommentary(const std::string& commentary);
    const std::vector<std::string>& GetCommentary() const { return commentary; }
    void ClearCommentary();
    
    // Statistics
    void CalculateMatchStats();
    std::string GenerateScorecard() const;
    std::string GenerateMatchReport() const;
    
    // Callbacks
    void SetBallEventCallback(std::function<void(const BallEvent&)> callback);
    void SetInningsEndCallback(std::function<void(const Innings&)> callback);
    void SetMatchEndCallback(std::function<void(const std::string&)> callback);
    
private:
    // Match data
    Team* team1;
    Team* team2;
    Venue* venue;
    MatchType matchType;
    
    // Match state
    MatchState matchState;
    std::vector<Innings> innings;
    std::vector<BallEvent> ballHistory;
    std::vector<std::string> commentary;
    
    // Conditions
    PitchConditions pitchConditions;
    WeatherConditions weatherConditions;
    
    // Control
    bool isPaused;
    bool isMatchComplete;
    
    // Callbacks
    std::function<void(const BallEvent&)> ballEventCallback;
    std::function<void(const Innings&)> inningsEndCallback;
    std::function<void(const std::string&)> matchEndCallback;
    
    // Random number generation
    std::random_device rd;
    std::mt19937 gen;
    
    // Helper methods
    BallEvent SimulateBallResult();
    bool SimulateWicket();
    WicketType DetermineWicketType();
    int SimulateRuns();
    BallResult DetermineBallResult();
    void UpdateMatchState(const BallEvent& event);
    void SwitchStriker();
    void HandleWicket();
    void HandleExtra();
    void CalculateRequiredRuns();
    void DetermineMatchResult();
    
    // Probability calculations
    double CalculateBattingSuccessRate(const Player* batsman, const Player* bowler);
    double CalculateBowlingSuccessRate(const Player* bowler, const Player* batsman);
    double CalculateFieldingSuccessRate(const Player* fielder);
    
    // Weather and pitch effects
    void ApplyWeatherEffects();
    void ApplyPitchEffects();
    double GetPitchDifficulty();
}; 