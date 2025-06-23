#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "Team.h"
#include "Player.h"
#include "Venue.h"
#include "MatchEngine.h"

enum class TournamentType {
    WorldCup,
    T20WorldCup,
    ChampionsTrophy,
    IPL,
    BBL,
    PSL,
    BilateralSeries,
    TriSeries,
    Quadrangular,
    Custom
};

enum class TournamentFormat {
    RoundRobin,
    Knockout,
    GroupStage,
    League,
    Hybrid
};

enum class TournamentStatus {
    NotStarted,
    GroupStage,
    Knockout,
    SemiFinal,
    Final,
    Completed
};

struct TournamentMatch {
    int matchId;
    Team* team1;
    Team* team2;
    Venue* venue;
    MatchFormat format;
    std::string date;
    bool isCompleted;
    int team1Score;
    int team2Score;
    int team1Wickets;
    int team2Wickets;
    Team* winner;
    std::string result;
    float duration;
};

struct TournamentGroup {
    std::string name;
    std::vector<Team*> teams;
    std::vector<TournamentMatch> matches;
    std::map<Team*, int> points;
    std::map<Team*, float> netRunRate;
    std::vector<Team*> standings;
};

struct TournamentStats {
    int totalMatches;
    int completedMatches;
    int totalRuns;
    int totalWickets;
    float averageScore;
    int highestScore;
    int lowestScore;
    std::map<Player*, int> topRunScorers;
    std::map<Player*, int> topWicketTakers;
    std::map<Team*, int> teamWins;
    std::map<Team*, int> teamLosses;
};

class TournamentManager {
public:
    TournamentManager();
    ~TournamentManager();
    
    // Tournament Creation
    void createTournament(const std::string& name, TournamentType type, 
                         TournamentFormat format, const std::vector<Team*>& teams,
                         const std::vector<Venue*>& venues);
    
    // Tournament Management
    void startTournament();
    void advanceTournament();
    void pauseTournament();
    void resumeTournament();
    void endTournament();
    
    // Match Management
    void scheduleMatches();
    void playMatch(int matchId);
    void simulateMatch(int matchId);
    void updateMatchResult(int matchId, Team* winner, int team1Score, 
                          int team2Score, int team1Wickets, int team2Wickets);
    
    // Group Management
    void createGroups(int numGroups);
    void assignTeamsToGroups();
    void updateGroupStandings();
    std::vector<Team*> getGroupWinners();
    std::vector<Team*> getGroupRunnersUp();
    
    // Knockout Management
    void createKnockoutBracket();
    void advanceKnockout();
    std::vector<Team*> getSemiFinalists();
    std::vector<Team*> getFinalists();
    Team* getWinner();
    
    // Statistics
    TournamentStats getTournamentStats() const;
    std::vector<Player*> getTopRunScorers(int count = 10) const;
    std::vector<Player*> getTopWicketTakers(int count = 10) const;
    std::vector<Team*> getTeamStandings() const;
    
    // Tournament Information
    std::string getName() const { return name; }
    TournamentType getType() const { return type; }
    TournamentFormat getFormat() const { return format; }
    TournamentStatus getStatus() const { return status; }
    int getCurrentStage() const { return currentStage; }
    int getTotalStages() const { return totalStages; }
    
    // Teams and Venues
    std::vector<Team*>& getTeams() { return teams; }
    std::vector<Venue*>& getVenues() { return venues; }
    std::vector<TournamentMatch>& getMatches() { return matches; }
    std::vector<TournamentGroup>& getGroups() { return groups; }
    
    // Match Engine Integration
    void setMatchEngine(MatchEngine* engine) { matchEngine = engine; }
    MatchEngine* getMatchEngine() const { return matchEngine; }
    
    // Callbacks
    void setMatchCompletedCallback(std::function<void(const TournamentMatch&)> callback);
    void setTournamentCompletedCallback(std::function<void(Team*)> callback);
    void setStageCompletedCallback(std::function<void(int)> callback);
    
    // Configuration
    void setMatchDuration(float duration) { matchDuration = duration; }
    void setSimulationSpeed(float speed) { simulationSpeed = speed; }
    void setAutoAdvance(bool autoAdvance) { this->autoAdvance = autoAdvance; }
    
    // Save/Load
    void saveTournament(const std::string& filename);
    void loadTournament(const std::string& filename);
    
    // Utility
    bool isTournamentComplete() const;
    bool isMatchScheduled(int matchId) const;
    TournamentMatch* getMatch(int matchId);
    std::string getTournamentProgress() const;
    void printStandings() const;

private:
    // Tournament Data
    std::string name;
    TournamentType type;
    TournamentFormat format;
    TournamentStatus status;
    int currentStage;
    int totalStages;
    
    // Participants
    std::vector<Team*> teams;
    std::vector<Venue*> venues;
    std::vector<TournamentMatch> matches;
    std::vector<TournamentGroup> groups;
    
    // Statistics
    TournamentStats stats;
    
    // Match Engine
    MatchEngine* matchEngine;
    
    // Configuration
    float matchDuration;
    float simulationSpeed;
    bool autoAdvance;
    
    // Callbacks
    std::function<void(const TournamentMatch&)> matchCompletedCallback;
    std::function<void(Team*)> tournamentCompletedCallback;
    std::function<void(int)> stageCompletedCallback;
    
    // Helper Methods
    void initializeTournament();
    void createRoundRobinMatches();
    void createKnockoutMatches();
    void updateStatistics(const TournamentMatch& match);
    void calculateNetRunRate();
    void sortGroupStandings();
    void advanceToNextStage();
    bool allGroupMatchesComplete() const;
    bool allKnockoutMatchesComplete() const;
    std::vector<Team*> getQualifiedTeams() const;
    void generateFixtures();
    void assignVenues();
    std::string generateMatchResult(const TournamentMatch& match) const;
    
    // Tournament-specific logic
    void handleWorldCup();
    void handleIPL();
    void handleBilateralSeries();
    void handleCustomTournament();
    
    // Utility functions
    int calculatePoints(bool isWin, bool isTie, bool isNoResult);
    float calculateNetRunRate(int runsScored, float oversFaced, int runsConceded, float oversBowled);
    std::string formatDate(const std::string& date);
    bool isValidMatch(const TournamentMatch& match) const;
}; 