#include "MatchEngine.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>

MatchEngine::MatchEngine() 
    : team1(nullptr)
    , team2(nullptr)
    , venue(nullptr)
    , matchType(MatchType::T20)
    , isPaused(false)
    , isMatchComplete(false)
{
    // Initialize random number generator
    gen = std::mt19937(rd());
}

MatchEngine::~MatchEngine() {
}

void MatchEngine::InitializeMatch(Team* t1, Team* t2, Venue* v, MatchType type) {
    team1 = t1;
    team2 = t2;
    venue = v;
    matchType = type;
    
    // Initialize match state
    matchState.matchType = type;
    matchState.format = MatchFormat::FIRST_INNINGS;
    matchState.currentInnings = 1;
    matchState.currentOver = 0;
    matchState.currentBall = 0;
    matchState.isMatchComplete = false;
    
    // Set initial teams
    matchState.battingTeam = team1->GetName();
    matchState.bowlingTeam = team2->GetName();
    
    // Set initial players
    if (!team1->GetBattingOrder().empty()) {
        matchState.striker = team1->GetBattingOrder()[0]->GetName();
        if (team1->GetBattingOrder().size() > 1) {
            matchState.nonStriker = team1->GetBattingOrder()[1]->GetName();
        }
    }
    
    if (!team2->GetBowlingOrder().empty()) {
        matchState.bowler = team2->GetBowlingOrder()[0]->GetName();
    }
    
    std::cout << "Match initialized: " << team1->GetName() << " vs " << team2->GetName() 
              << " at " << venue->GetName() << std::endl;
}

void MatchEngine::SetPitchConditions(const PitchConditions& conditions) {
    pitchConditions = conditions;
}

void MatchEngine::SetWeatherConditions(const WeatherConditions& weather) {
    weatherConditions = weather;
}

void MatchEngine::SimulateBall() {
    if (isPaused || isMatchComplete) {
        return;
    }
    
    // Generate ball event
    BallEvent event = SimulateBallResult();
    
    // Update match state
    UpdateMatchState(event);
    
    // Add to ball history
    ballHistory.push_back(event);
    
    // Add commentary
    AddCommentary(event.commentary);
    
    // Call callback if set
    if (ballEventCallback) {
        ballEventCallback(event);
    }
    
    // Update player stats
    UpdatePlayerStats(event);
}

void MatchEngine::SimulateOver() {
    for (int i = 0; i < 6; ++i) {
        SimulateBall();
        if (isMatchComplete) break;
    }
}

void MatchEngine::SimulateInnings() {
    int maxOvers = (matchType == MatchType::T20) ? 20 : 
                   (matchType == MatchType::ODI) ? 50 : 90;
    
    for (int over = 0; over < maxOvers; ++over) {
        SimulateOver();
        if (isMatchComplete) break;
    }
}

void MatchEngine::SimulateMatch() {
    // Simulate first innings
    SimulateInnings();
    
    if (!isMatchComplete) {
        // Switch teams for second innings
        matchState.format = MatchFormat::SECOND_INNINGS;
        matchState.battingTeam = team2->GetName();
        matchState.bowlingTeam = team1->GetName();
        
        // Simulate second innings
        SimulateInnings();
    }
    
    DetermineMatchResult();
}

int MatchEngine::GetCurrentScore() const {
    // Calculate from ball history
    int score = 0;
    for (const auto& ball : ballHistory) {
        score += ball.runs;
    }
    return score;
}

int MatchEngine::GetCurrentWickets() const {
    int wickets = 0;
    for (const auto& ball : ballHistory) {
        if (ball.result == BallResult::WICKET) {
            wickets++;
        }
    }
    return wickets;
}

double MatchEngine::GetCurrentRunRate() const {
    int totalBalls = ballHistory.size();
    if (totalBalls == 0) return 0.0;
    
    int totalRuns = GetCurrentScore();
    return (totalRuns * 6.0) / totalBalls;
}

int MatchEngine::GetRequiredRuns() const {
    // TODO: Implement based on match format
    return 0;
}

int MatchEngine::GetRemainingOvers() const {
    int maxOvers = (matchType == MatchType::T20) ? 20 : 
                   (matchType == MatchType::ODI) ? 50 : 90;
    return maxOvers - matchState.currentOver;
}

void MatchEngine::UpdatePlayerStats(const BallEvent& event) {
    // TODO: Implement player stats update
}

std::map<std::string, PlayerStats> MatchEngine::GetPlayerMatchStats() const {
    // TODO: Implement player match stats
    return std::map<std::string, PlayerStats>();
}

void MatchEngine::SetFieldingPositions(const std::map<std::string, FieldingPosition>& positions) {
    // TODO: Implement fielding positions
}

void MatchEngine::SetBowlingChange(const std::string& newBowler) {
    matchState.bowler = newBowler;
}

void MatchEngine::SetBattingOrder(const std::vector<std::string>& order) {
    // TODO: Implement batting order change
}

void MatchEngine::PauseMatch() {
    isPaused = true;
}

void MatchEngine::ResumeMatch() {
    isPaused = false;
}

void MatchEngine::EndMatch() {
    isMatchComplete = true;
}

void MatchEngine::AddCommentary(const std::string& commentary) {
    this->commentary.push_back(commentary);
}

void MatchEngine::ClearCommentary() {
    commentary.clear();
}

void MatchEngine::CalculateMatchStats() {
    // TODO: Implement match statistics calculation
}

std::string MatchEngine::GenerateScorecard() const {
    std::ostringstream scorecard;
    scorecard << "=== MATCH SCORECARD ===" << std::endl;
    scorecard << team1->GetName() << " vs " << team2->GetName() << std::endl;
    scorecard << "Venue: " << venue->GetName() << std::endl;
    scorecard << "Format: " << (matchType == MatchType::T20 ? "T20" : 
                               matchType == MatchType::ODI ? "ODI" : "Test") << std::endl;
    scorecard << "Score: " << GetCurrentScore() << "/" << GetCurrentWickets() << std::endl;
    scorecard << "Overs: " << matchState.currentOver << "." << matchState.currentBall << std::endl;
    scorecard << "Run Rate: " << GetCurrentRunRate() << std::endl;
    return scorecard.str();
}

std::string MatchEngine::GenerateMatchReport() const {
    return GenerateScorecard();
}

void MatchEngine::SetBallEventCallback(std::function<void(const BallEvent&)> callback) {
    ballEventCallback = callback;
}

void MatchEngine::SetInningsEndCallback(std::function<void(const Innings&)> callback) {
    inningsEndCallback = callback;
}

void MatchEngine::SetMatchEndCallback(std::function<void(const std::string&)> callback) {
    matchEndCallback = callback;
}

void MatchEngine::Update(float deltaTime) {
    // TODO: Implement real-time match updates
}

// Private helper methods
BallEvent MatchEngine::SimulateBallResult() {
    BallEvent event;
    
    // Simple random ball result
    std::uniform_int_distribution<> resultDist(0, 100);
    int result = resultDist(gen);
    
    if (result < 30) {
        event.result = BallResult::DOT_BALL;
        event.runs = 0;
        event.commentary = "Dot ball!";
    } else if (result < 50) {
        event.result = BallResult::SINGLE;
        event.runs = 1;
        event.commentary = "Single taken.";
    } else if (result < 65) {
        event.result = BallResult::FOUR;
        event.runs = 4;
        event.commentary = "FOUR! Beautiful shot!";
    } else if (result < 75) {
        event.result = BallResult::SIX;
        event.runs = 6;
        event.commentary = "SIX! Massive hit!";
    } else if (result < 85) {
        event.result = BallResult::WICKET;
        event.runs = 0;
        event.wicketType = WicketType::BOWLED;
        event.commentary = "WICKET! Bowled!";
    } else {
        event.result = BallResult::WIDE;
        event.runs = 1;
        event.commentary = "Wide ball.";
    }
    
    // Set player names
    event.striker = matchState.striker;
    event.nonStriker = matchState.nonStriker;
    event.bowler = matchState.bowler;
    
    return event;
}

bool MatchEngine::SimulateWicket() {
    std::uniform_int_distribution<> wicketDist(0, 100);
    return wicketDist(gen) < 15; // 15% chance of wicket
}

WicketType MatchEngine::DetermineWicketType() {
    std::uniform_int_distribution<> typeDist(0, 4);
    int type = typeDist(gen);
    
    switch (type) {
        case 0: return WicketType::BOWLED;
        case 1: return WicketType::CAUGHT;
        case 2: return WicketType::LBW;
        case 3: return WicketType::RUN_OUT;
        default: return WicketType::STUMPED;
    }
}

int MatchEngine::SimulateRuns() {
    std::uniform_int_distribution<> runsDist(0, 6);
    return runsDist(gen);
}

BallResult MatchEngine::DetermineBallResult() {
    std::uniform_int_distribution<> resultDist(0, 100);
    int result = resultDist(gen);
    
    if (result < 30) return BallResult::DOT_BALL;
    if (result < 50) return BallResult::SINGLE;
    if (result < 60) return BallResult::DOUBLE;
    if (result < 65) return BallResult::TRIPLE;
    if (result < 75) return BallResult::FOUR;
    if (result < 80) return BallResult::SIX;
    if (result < 85) return BallResult::WICKET;
    if (result < 90) return BallResult::WIDE;
    if (result < 95) return BallResult::NO_BALL;
    return BallResult::BYE;
}

void MatchEngine::UpdateMatchState(const BallEvent& event) {
    matchState.currentBall++;
    
    if (matchState.currentBall >= 6) {
        matchState.currentBall = 0;
        matchState.currentOver++;
        SwitchStriker();
    }
    
    if (event.result == BallResult::WICKET) {
        HandleWicket();
    }
}

void MatchEngine::SwitchStriker() {
    std::swap(matchState.striker, matchState.nonStriker);
}

void MatchEngine::HandleWicket() {
    // TODO: Implement wicket handling
}

void MatchEngine::HandleExtra() {
    // TODO: Implement extra handling
}

void MatchEngine::CalculateRequiredRuns() {
    // TODO: Implement required runs calculation
}

void MatchEngine::DetermineMatchResult() {
    // TODO: Implement match result determination
    isMatchComplete = true;
    
    if (matchEndCallback) {
        matchEndCallback("Match completed");
    }
}

double MatchEngine::CalculateBattingSuccessRate(const Player* batsman, const Player* bowler) {
    // TODO: Implement batting success rate calculation
    return 0.5;
}

double MatchEngine::CalculateBowlingSuccessRate(const Player* bowler, const Player* batsman) {
    // TODO: Implement bowling success rate calculation
    return 0.3;
}

double MatchEngine::CalculateFieldingSuccessRate(const Player* fielder) {
    // TODO: Implement fielding success rate calculation
    return 0.8;
}

void MatchEngine::ApplyWeatherEffects() {
    // TODO: Implement weather effects
}

void MatchEngine::ApplyPitchEffects() {
    // TODO: Implement pitch effects
}

double MatchEngine::GetPitchDifficulty() {
    // TODO: Implement pitch difficulty calculation
    return 5.0;
} 