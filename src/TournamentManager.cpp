#include "TournamentManager.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

TournamentManager::TournamentManager() : 
    status(TournamentStatus::NotStarted),
    currentStage(0),
    totalStages(1),
    matchEngine(nullptr),
    matchDuration(3.0f),
    simulationSpeed(1.0f),
    autoAdvance(false) {
    
    // Initialize statistics
    stats.totalMatches = 0;
    stats.completedMatches = 0;
    stats.totalRuns = 0;
    stats.totalWickets = 0;
    stats.averageScore = 0.0f;
    stats.highestScore = 0;
    stats.lowestScore = 999;
}

TournamentManager::~TournamentManager() {
    // Cleanup is handled automatically
}

void TournamentManager::createTournament(const std::string& tournamentName, TournamentType tournamentType, 
                                        TournamentFormat tournamentFormat, const std::vector<Team*>& tournamentTeams,
                                        const std::vector<Venue*>& tournamentVenues) {
    name = tournamentName;
    type = tournamentType;
    format = tournamentFormat;
    teams = tournamentTeams;
    venues = tournamentVenues;
    
    // Set total stages based on format
    switch (format) {
        case TournamentFormat::GroupStage:
            totalStages = 2; // Group stage + knockout
            break;
        case TournamentFormat::Knockout:
            totalStages = 1;
            break;
        case TournamentFormat::RoundRobin:
            totalStages = 1;
            break;
        case TournamentFormat::Hybrid:
            totalStages = 3; // Group + Super League + Final
            break;
        default:
            totalStages = 1;
    }
    
    initializeTournament();
    std::cout << "Tournament '" << name << "' created successfully!" << std::endl;
}

void TournamentManager::initializeTournament() {
    status = TournamentStatus::NotStarted;
    currentStage = 0;
    
    // Clear previous data
    matches.clear();
    groups.clear();
    stats = TournamentStats();
    
    // Initialize based on tournament type
    switch (type) {
        case TournamentType::WorldCup:
            handleWorldCup();
            break;
        case TournamentType::IPL:
            handleIPL();
            break;
        case TournamentType::BilateralSeries:
            handleBilateralSeries();
            break;
        default:
            handleCustomTournament();
    }
}

void TournamentManager::startTournament() {
    if (status == TournamentStatus::NotStarted) {
        status = TournamentStatus::GroupStage;
        currentStage = 1;
        
        if (format == TournamentFormat::GroupStage || format == TournamentFormat::Hybrid) {
            createGroups(2); // Default to 2 groups
            assignTeamsToGroups();
        }
        
        scheduleMatches();
        std::cout << "Tournament '" << name << "' started!" << std::endl;
    }
}

void TournamentManager::advanceTournament() {
    if (status == TournamentStatus::NotStarted) {
        startTournament();
        return;
    }
    
    // Check if current stage is complete
    bool stageComplete = false;
    
    switch (status) {
        case TournamentStatus::GroupStage:
            stageComplete = allGroupMatchesComplete();
            if (stageComplete) {
                updateGroupStandings();
                if (format == TournamentFormat::GroupStage || format == TournamentFormat::Hybrid) {
                    status = TournamentStatus::Knockout;
                    createKnockoutBracket();
                } else {
                    status = TournamentStatus::Completed;
                }
            }
            break;
            
        case TournamentStatus::Knockout:
            stageComplete = allKnockoutMatchesComplete();
            if (stageComplete) {
                std::vector<Team*> finalists = getFinalists();
                if (finalists.size() == 2) {
                    status = TournamentStatus::Final;
                    // Create final match
                    TournamentMatch final;
                    final.matchId = matches.size() + 1;
                    final.team1 = finalists[0];
                    final.team2 = finalists[1];
                    final.venue = venues[0]; // Use first venue for final
                    final.format = MatchFormat::T20; // Default format
                    final.date = "Final";
                    final.isCompleted = false;
                    matches.push_back(final);
                }
            }
            break;
            
        case TournamentStatus::Final:
            // Final match is played separately
            break;
            
        default:
            break;
    }
    
    if (stageComplete && stageCompletedCallback) {
        stageCompletedCallback(currentStage);
    }
}

void TournamentManager::pauseTournament() {
    // Tournament can be paused at any stage
    std::cout << "Tournament paused" << std::endl;
}

void TournamentManager::resumeTournament() {
    std::cout << "Tournament resumed" << std::endl;
}

void TournamentManager::endTournament() {
    status = TournamentStatus::Completed;
    std::cout << "Tournament '" << name << "' completed!" << std::endl;
    
    if (tournamentCompletedCallback) {
        Team* winner = getWinner();
        tournamentCompletedCallback(winner);
    }
}

void TournamentManager::scheduleMatches() {
    switch (format) {
        case TournamentFormat::RoundRobin:
            createRoundRobinMatches();
            break;
        case TournamentFormat::Knockout:
            createKnockoutMatches();
            break;
        case TournamentFormat::GroupStage:
            // Groups are handled separately
            break;
        default:
            createRoundRobinMatches();
    }
    
    assignVenues();
    stats.totalMatches = matches.size();
}

void TournamentManager::createRoundRobinMatches() {
    int matchId = 1;
    
    for (size_t i = 0; i < teams.size(); ++i) {
        for (size_t j = i + 1; j < teams.size(); ++j) {
            TournamentMatch match;
            match.matchId = matchId++;
            match.team1 = teams[i];
            match.team2 = teams[j];
            match.venue = nullptr; // Will be assigned later
            match.format = MatchFormat::T20;
            match.date = "TBD";
            match.isCompleted = false;
            match.team1Score = 0;
            match.team2Score = 0;
            match.team1Wickets = 0;
            match.team2Wickets = 0;
            match.winner = nullptr;
            match.result = "";
            match.duration = 0.0f;
            
            matches.push_back(match);
        }
    }
}

void TournamentManager::createKnockoutMatches() {
    // Create knockout bracket based on number of teams
    int numTeams = teams.size();
    int numMatches = numTeams - 1; // Single elimination
    
    for (int i = 0; i < numMatches; ++i) {
        TournamentMatch match;
        match.matchId = i + 1;
        match.team1 = nullptr; // Will be filled as tournament progresses
        match.team2 = nullptr;
        match.venue = nullptr;
        match.format = MatchFormat::T20;
        match.date = "TBD";
        match.isCompleted = false;
        match.team1Score = 0;
        match.team2Score = 0;
        match.team1Wickets = 0;
        match.team2Wickets = 0;
        match.winner = nullptr;
        match.result = "";
        match.duration = 0.0f;
        
        matches.push_back(match);
    }
}

void TournamentManager::playMatch(int matchId) {
    TournamentMatch* match = getMatch(matchId);
    if (!match || match->isCompleted) {
        return;
    }
    
    if (matchEngine) {
        // Use match engine to play the match
        matchEngine->startMatch(*match->team1, *match->team2, *match->venue, match->format);
        
        // Simulate the match
        while (!matchEngine->isMatchComplete()) {
            matchEngine->update(0.016f); // 60 FPS
        }
        
        // Get match result
        auto matchState = matchEngine->getMatchState();
        match->team1Score = matchState.team1Score;
        match->team2Score = matchState.team2Score;
        match->team1Wickets = matchState.team1Wickets;
        match->team2Wickets = matchState.team2Wickets;
        match->winner = (matchState.team1Score > matchState.team2Score) ? match->team1 : match->team2;
        match->isCompleted = true;
        match->result = generateMatchResult(*match);
        
        updateMatchResult(matchId, match->winner, match->team1Score, 
                         match->team2Score, match->team1Wickets, match->team2Wickets);
    }
}

void TournamentManager::simulateMatch(int matchId) {
    TournamentMatch* match = getMatch(matchId);
    if (!match || match->isCompleted) {
        return;
    }
    
    // Simple simulation without match engine
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Generate scores based on team strengths
    int team1Strength = match->team1->getTeamStrength();
    int team2Strength = match->team2->getTeamStrength();
    
    std::normal_distribution<float> scoreDist(150.0f, 30.0f);
    int team1Score = std::max(80, std::min(250, (int)scoreDist(gen)));
    int team2Score = std::max(80, std::min(250, (int)scoreDist(gen)));
    
    // Adjust scores based on team strength
    team1Score = (int)(team1Score * (1.0f + (team1Strength - 50) / 100.0f));
    team2Score = (int)(team2Score * (1.0f + (team2Strength - 50) / 100.0f));
    
    // Add some randomness
    std::uniform_int_distribution<int> randomDist(-20, 20);
    team1Score += randomDist(gen);
    team2Score += randomDist(gen);
    
    // Ensure scores are within reasonable bounds
    team1Score = std::max(80, std::min(250, team1Score));
    team2Score = std::max(80, std::min(250, team2Score));
    
    // Determine winner
    Team* winner = (team1Score > team2Score) ? match->team1 : match->team2;
    
    updateMatchResult(matchId, winner, team1Score, team2Score, 10, 10);
}

void TournamentManager::updateMatchResult(int matchId, Team* winner, int team1Score, 
                                         int team2Score, int team1Wickets, int team2Wickets) {
    TournamentMatch* match = getMatch(matchId);
    if (!match) {
        return;
    }
    
    match->team1Score = team1Score;
    match->team2Score = team2Score;
    match->team1Wickets = team1Wickets;
    match->team2Wickets = team2Wickets;
    match->winner = winner;
    match->isCompleted = true;
    match->result = generateMatchResult(*match);
    
    // Update statistics
    updateStatistics(*match);
    stats.completedMatches++;
    
    // Update group standings if applicable
    if (status == TournamentStatus::GroupStage) {
        updateGroupStandings();
    }
    
    // Call callback
    if (matchCompletedCallback) {
        matchCompletedCallback(*match);
    }
    
    std::cout << "Match " << matchId << " completed: " << match->team1->getName() 
              << " " << team1Score << "/" << team1Wickets << " vs " 
              << match->team2->getName() << " " << team2Score << "/" << team2Wickets
              << " - Winner: " << winner->getName() << std::endl;
}

void TournamentManager::createGroups(int numGroups) {
    groups.clear();
    
    for (int i = 0; i < numGroups; ++i) {
        TournamentGroup group;
        group.name = "Group " + std::to_string(i + 1);
        groups.push_back(group);
    }
}

void TournamentManager::assignTeamsToGroups() {
    // Simple round-robin assignment
    for (size_t i = 0; i < teams.size(); ++i) {
        int groupIndex = i % groups.size();
        groups[groupIndex].teams.push_back(teams[i]);
    }
    
    // Create matches within each group
    for (auto& group : groups) {
        for (size_t i = 0; i < group.teams.size(); ++i) {
            for (size_t j = i + 1; j < group.teams.size(); ++j) {
                TournamentMatch match;
                match.matchId = matches.size() + 1;
                match.team1 = group.teams[i];
                match.team2 = group.teams[j];
                match.venue = nullptr;
                match.format = MatchFormat::T20;
                match.date = "TBD";
                match.isCompleted = false;
                
                group.matches.push_back(match);
                matches.push_back(match);
            }
        }
    }
}

void TournamentManager::updateGroupStandings() {
    for (auto& group : groups) {
        // Clear previous standings
        group.points.clear();
        group.netRunRate.clear();
        group.standings.clear();
        
        // Initialize points
        for (auto team : group.teams) {
            group.points[team] = 0;
            group.netRunRate[team] = 0.0f;
        }
        
        // Calculate points from completed matches
        for (const auto& match : group.matches) {
            if (match.isCompleted && match.winner) {
                // Winner gets 2 points
                group.points[match.winner] += 2;
                
                // Loser gets 0 points (no ties for simplicity)
                Team* loser = (match.winner == match.team1) ? match.team2 : match.team1;
                group.points[loser] += 0;
            }
        }
        
        // Calculate net run rate (simplified)
        for (auto team : group.teams) {
            float runsScored = 0.0f;
            float oversFaced = 0.0f;
            float runsConceded = 0.0f;
            float oversBowled = 0.0f;
            
            for (const auto& match : group.matches) {
                if (match.isCompleted) {
                    if (match.team1 == team) {
                        runsScored += match.team1Score;
                        oversFaced += 20.0f; // Assuming T20
                        runsConceded += match.team2Score;
                        oversBowled += 20.0f;
                    } else if (match.team2 == team) {
                        runsScored += match.team2Score;
                        oversFaced += 20.0f;
                        runsConceded += match.team1Score;
                        oversBowled += 20.0f;
                    }
                }
            }
            
            if (oversFaced > 0 && oversBowled > 0) {
                group.netRunRate[team] = calculateNetRunRate(runsScored, oversFaced, runsConceded, oversBowled);
            }
        }
        
        // Sort standings
        sortGroupStandings();
    }
}

void TournamentManager::sortGroupStandings() {
    for (auto& group : groups) {
        group.standings = group.teams;
        
        std::sort(group.standings.begin(), group.standings.end(),
            [&group](Team* a, Team* b) {
                // Sort by points first
                if (group.points[a] != group.points[b]) {
                    return group.points[a] > group.points[b];
                }
                // Then by net run rate
                return group.netRunRate[a] > group.netRunRate[b];
            });
    }
}

std::vector<Team*> TournamentManager::getGroupWinners() {
    std::vector<Team*> winners;
    for (const auto& group : groups) {
        if (!group.standings.empty()) {
            winners.push_back(group.standings[0]);
        }
    }
    return winners;
}

std::vector<Team*> TournamentManager::getGroupRunnersUp() {
    std::vector<Team*> runnersUp;
    for (const auto& group : groups) {
        if (group.standings.size() > 1) {
            runnersUp.push_back(group.standings[1]);
        }
    }
    return runnersUp;
}

void TournamentManager::createKnockoutBracket() {
    std::vector<Team*> qualifiedTeams = getQualifiedTeams();
    
    // Clear existing knockout matches
    matches.erase(std::remove_if(matches.begin(), matches.end(),
        [](const TournamentMatch& match) { return !match.team1 || !match.team2; }), matches.end());
    
    // Create new knockout matches
    for (size_t i = 0; i < qualifiedTeams.size(); i += 2) {
        if (i + 1 < qualifiedTeams.size()) {
            TournamentMatch match;
            match.matchId = matches.size() + 1;
            match.team1 = qualifiedTeams[i];
            match.team2 = qualifiedTeams[i + 1];
            match.venue = venues[0];
            match.format = MatchFormat::T20;
            match.date = "Knockout";
            match.isCompleted = false;
            
            matches.push_back(match);
        }
    }
}

std::vector<Team*> TournamentManager::getQualifiedTeams() {
    std::vector<Team*> qualified;
    
    if (format == TournamentFormat::GroupStage) {
        auto winners = getGroupWinners();
        auto runnersUp = getGroupRunnersUp();
        qualified.insert(qualified.end(), winners.begin(), winners.end());
        qualified.insert(qualified.end(), runnersUp.begin(), runnersUp.end());
    } else {
        qualified = teams;
    }
    
    return qualified;
}

std::vector<Team*> TournamentManager::getSemiFinalists() {
    std::vector<Team*> semiFinalists;
    
    // Get teams that won their knockout matches
    for (const auto& match : matches) {
        if (match.isCompleted && match.winner) {
            semiFinalists.push_back(match.winner);
        }
    }
    
    return semiFinalists;
}

std::vector<Team*> TournamentManager::getFinalists() {
    return getSemiFinalists();
}

Team* TournamentManager::getWinner() {
    if (status == TournamentStatus::Completed) {
        // Find the final match winner
        for (const auto& match : matches) {
            if (match.isCompleted && match.winner && match.date == "Final") {
                return match.winner;
            }
        }
    }
    return nullptr;
}

TournamentStats TournamentManager::getTournamentStats() const {
    return stats;
}

std::vector<Player*> TournamentManager::getTopRunScorers(int count) const {
    std::vector<std::pair<Player*, int>> scorers;
    
    for (const auto& pair : stats.topRunScorers) {
        scorers.push_back(pair);
    }
    
    std::sort(scorers.begin(), scorers.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<Player*> result;
    for (size_t i = 0; i < std::min(scorers.size(), (size_t)count); ++i) {
        result.push_back(scorers[i].first);
    }
    
    return result;
}

std::vector<Player*> TournamentManager::getTopWicketTakers(int count) const {
    std::vector<std::pair<Player*, int>> takers;
    
    for (const auto& pair : stats.topWicketTakers) {
        takers.push_back(pair);
    }
    
    std::sort(takers.begin(), takers.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<Player*> result;
    for (size_t i = 0; i < std::min(takers.size(), (size_t)count); ++i) {
        result.push_back(takers[i].first);
    }
    
    return result;
}

std::vector<Team*> TournamentManager::getTeamStandings() const {
    std::vector<std::pair<Team*, int>> standings;
    
    for (const auto& pair : stats.teamWins) {
        standings.push_back(pair);
    }
    
    std::sort(standings.begin(), standings.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<Team*> result;
    for (const auto& pair : standings) {
        result.push_back(pair.first);
    }
    
    return result;
}

void TournamentManager::setMatchCompletedCallback(std::function<void(const TournamentMatch&)> callback) {
    matchCompletedCallback = callback;
}

void TournamentManager::setTournamentCompletedCallback(std::function<void(Team*)> callback) {
    tournamentCompletedCallback = callback;
}

void TournamentManager::setStageCompletedCallback(std::function<void(int)> callback) {
    stageCompletedCallback = callback;
}

bool TournamentManager::isTournamentComplete() const {
    return status == TournamentStatus::Completed;
}

bool TournamentManager::isMatchScheduled(int matchId) const {
    return getMatch(matchId) != nullptr;
}

TournamentMatch* TournamentManager::getMatch(int matchId) {
    for (auto& match : matches) {
        if (match.matchId == matchId) {
            return &match;
        }
    }
    return nullptr;
}

std::string TournamentManager::getTournamentProgress() const {
    std::stringstream ss;
    ss << "Stage " << currentStage << "/" << totalStages << " - ";
    
    switch (status) {
        case TournamentStatus::NotStarted:
            ss << "Not Started";
            break;
        case TournamentStatus::GroupStage:
            ss << "Group Stage (" << stats.completedMatches << "/" << stats.totalMatches << " matches)";
            break;
        case TournamentStatus::Knockout:
            ss << "Knockout Stage";
            break;
        case TournamentStatus::Final:
            ss << "Final";
            break;
        case TournamentStatus::Completed:
            ss << "Completed";
            break;
    }
    
    return ss.str();
}

void TournamentManager::printStandings() const {
    std::cout << "\n=== " << name << " Standings ===\n";
    
    if (status == TournamentStatus::GroupStage) {
        for (const auto& group : groups) {
            std::cout << "\n" << group.name << ":\n";
            std::cout << std::setw(20) << "Team" << std::setw(10) << "Points" << std::setw(15) << "Net Run Rate\n";
            std::cout << std::string(45, '-') << "\n";
            
            for (size_t i = 0; i < group.standings.size(); ++i) {
                Team* team = group.standings[i];
                std::cout << std::setw(20) << team->getName() 
                          << std::setw(10) << group.points.at(team)
                          << std::setw(15) << std::fixed << std::setprecision(3) 
                          << group.netRunRate.at(team) << "\n";
            }
        }
    } else {
        std::cout << "Tournament not in group stage\n";
    }
}

// Private helper methods
void TournamentManager::updateStatistics(const TournamentMatch& match) {
    stats.totalRuns += match.team1Score + match.team2Score;
    stats.totalWickets += match.team1Wickets + match.team2Wickets;
    
    if (match.team1Score > stats.highestScore) {
        stats.highestScore = match.team1Score;
    }
    if (match.team2Score > stats.highestScore) {
        stats.highestScore = match.team2Score;
    }
    
    if (match.team1Score < stats.lowestScore) {
        stats.lowestScore = match.team1Score;
    }
    if (match.team2Score < stats.lowestScore) {
        stats.lowestScore = match.team2Score;
    }
    
    // Update team wins/losses
    if (match.winner) {
        stats.teamWins[match.winner]++;
        Team* loser = (match.winner == match.team1) ? match.team2 : match.team1;
        stats.teamLosses[loser]++;
    }
    
    // Update average score
    if (stats.completedMatches > 0) {
        stats.averageScore = (float)stats.totalRuns / stats.completedMatches;
    }
}

float TournamentManager::calculateNetRunRate(int runsScored, float oversFaced, 
                                           int runsConceded, float oversBowled) {
    if (oversFaced <= 0 || oversBowled <= 0) {
        return 0.0f;
    }
    
    float runRateFor = (float)runsScored / oversFaced;
    float runRateAgainst = (float)runsConceded / oversBowled;
    
    return runRateFor - runRateAgainst;
}

bool TournamentManager::allGroupMatchesComplete() const {
    for (const auto& group : groups) {
        for (const auto& match : group.matches) {
            if (!match.isCompleted) {
                return false;
            }
        }
    }
    return true;
}

bool TournamentManager::allKnockoutMatchesComplete() const {
    for (const auto& match : matches) {
        if (match.team1 && match.team2 && !match.isCompleted) {
            return false;
        }
    }
    return true;
}

void TournamentManager::assignVenues() {
    int venueIndex = 0;
    for (auto& match : matches) {
        if (venueIndex < venues.size()) {
            match.venue = venues[venueIndex];
            venueIndex = (venueIndex + 1) % venues.size();
        }
    }
}

std::string TournamentManager::generateMatchResult(const TournamentMatch& match) const {
    std::stringstream ss;
    
    if (match.winner == match.team1) {
        ss << match.team1->getName() << " beat " << match.team2->getName() 
           << " by " << (match.team1Score - match.team2Score) << " runs";
    } else {
        ss << match.team2->getName() << " beat " << match.team1->getName() 
           << " by " << (10 - match.team2Wickets) << " wickets";
    }
    
    return ss.str();
}

// Tournament-specific handlers
void TournamentManager::handleWorldCup() {
    format = TournamentFormat::GroupStage;
    totalStages = 2;
}

void TournamentManager::handleIPL() {
    format = TournamentFormat::GroupStage;
    totalStages = 2;
}

void TournamentManager::handleBilateralSeries() {
    format = TournamentFormat::RoundRobin;
    totalStages = 1;
}

void TournamentManager::handleCustomTournament() {
    // Default to round robin
    format = TournamentFormat::RoundRobin;
    totalStages = 1;
} 