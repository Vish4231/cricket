#include "Team.h"
#include <algorithm>
#include <iostream>

Team::Team(const std::string& name, TeamType type)
    : name(name)
    , type(type)
    , captain(nullptr)
    , viceCaptain(nullptr)
    , currentFormation(TeamFormation::BALANCED)
    , teamRating(0)
{
    // Initialize finances
    finances.budget = 100000000; // 10 crore base budget
    finances.currentSpending = 0;
    finances.sponsorship = 20000000;
    finances.ticketSales = 0;
    finances.merchandise = 0;
    finances.totalRevenue = 0;
    finances.totalExpenses = 0;
    finances.profit = 0;
    
    // Initialize morale
    morale.overallMorale = 75;
    morale.teamChemistry = 70;
    morale.playerSatisfaction = 75;
    morale.managementRating = 80;
    
    // Initialize stats
    stats.matchesPlayed = 0;
    stats.matchesWon = 0;
    stats.matchesLost = 0;
    stats.matchesDrawn = 0;
    stats.matchesTied = 0;
    stats.winPercentage = 0.0;
    stats.totalRuns = 0;
    stats.totalWickets = 0;
    stats.netRunRate = 0.0;
    stats.points = 0;
}

Team::~Team() {
}

Team::Team(Team&& other) noexcept
    : name(std::move(other.name))
    , type(other.type)
    , city(std::move(other.city))
    , owner(std::move(other.owner))
    , founded(other.founded)
    , colors(std::move(other.colors))
    , logo(std::move(other.logo))
    , budget(other.budget)
    , trophyCount(other.trophyCount)
    , trophies(std::move(other.trophies))
    , settings(std::move(other.settings))
    , stats(std::move(other.stats))
    , statsMap(std::move(other.statsMap))
    , teamRating(other.teamRating)
    , squad(std::move(other.squad))
    , playingXI(std::move(other.playingXI))
    , coach(std::move(other.coach))
    , manager(std::move(other.manager))
    , captain(other.captain)
    , viceCaptain(other.viceCaptain)
    , homeVenue(std::move(other.homeVenue))
    , currentFormation(other.currentFormation)
    , battingOrder(std::move(other.battingOrder))
    , bowlingOrder(std::move(other.bowlingOrder))
    , fieldingPositions(std::move(other.fieldingPositions))
    , matchStrategy(std::move(other.matchStrategy))
    , finances(std::move(other.finances))
    , morale(std::move(other.morale))
    , youthPlayers(std::move(other.youthPlayers)) {
}

Team::Team(const Team& other)
    : name(other.name)
    , type(other.type)
    , city(other.city)
    , owner(other.owner)
    , founded(other.founded)
    , colors(other.colors)
    , logo(other.logo)
    , budget(other.budget)
    , trophyCount(other.trophyCount)
    , trophies(other.trophies)
    , settings(other.settings)
    , stats(other.stats)
    , statsMap(other.statsMap)
    , teamRating(other.teamRating)
    , coach(other.coach)
    , manager(other.manager)
    , captain(nullptr) // Will be set after copying players
    , viceCaptain(nullptr) // Will be set after copying players
    , homeVenue(other.homeVenue)
    , currentFormation(other.currentFormation)
    , fieldingPositions(other.fieldingPositions)
    , matchStrategy(other.matchStrategy)
    , finances(other.finances)
    , morale(other.morale) {
    
    // Deep copy players
    for (const auto& player : other.squad) {
        squad.push_back(std::make_unique<Player>(*player));
    }
    
    // Deep copy youth players
    for (const auto& player : other.youthPlayers) {
        youthPlayers.push_back(std::make_unique<Player>(*player));
    }
    
    // Set up player pointers after copying
    if (other.captain) {
        captain = GetPlayer(other.captain->GetName());
    }
    if (other.viceCaptain) {
        viceCaptain = GetPlayer(other.viceCaptain->GetName());
    }
    
    // Set up playing XI, batting order, and bowling order
    for (const auto& player : other.playingXI) {
        Player* newPlayer = GetPlayer(player->GetName());
        if (newPlayer) {
            playingXI.push_back(newPlayer);
        }
    }
    
    for (const auto& player : other.battingOrder) {
        Player* newPlayer = GetPlayer(player->GetName());
        if (newPlayer) {
            battingOrder.push_back(newPlayer);
        }
    }
    
    for (const auto& player : other.bowlingOrder) {
        Player* newPlayer = GetPlayer(player->GetName());
        if (newPlayer) {
            bowlingOrder.push_back(newPlayer);
        }
    }
}

Team& Team::operator=(const Team& other) {
    if (this != &other) {
        name = other.name;
        type = other.type;
        city = other.city;
        owner = other.owner;
        founded = other.founded;
        colors = other.colors;
        logo = other.logo;
        budget = other.budget;
        trophyCount = other.trophyCount;
        trophies = other.trophies;
        settings = other.settings;
        stats = other.stats;
        statsMap = other.statsMap;
        teamRating = other.teamRating;
        coach = other.coach;
        manager = other.manager;
        captain = nullptr;
        viceCaptain = nullptr;
        homeVenue = other.homeVenue;
        currentFormation = other.currentFormation;
        fieldingPositions = other.fieldingPositions;
        matchStrategy = other.matchStrategy;
        finances = other.finances;
        morale = other.morale;
        
        // Clear existing players
        squad.clear();
        youthPlayers.clear();
        playingXI.clear();
        battingOrder.clear();
        bowlingOrder.clear();
        
        // Deep copy players
        for (const auto& player : other.squad) {
            squad.push_back(std::make_unique<Player>(*player));
        }
        
        // Deep copy youth players
        for (const auto& player : other.youthPlayers) {
            youthPlayers.push_back(std::make_unique<Player>(*player));
        }
        
        // Set up player pointers after copying
        if (other.captain) {
            captain = GetPlayer(other.captain->GetName());
        }
        if (other.viceCaptain) {
            viceCaptain = GetPlayer(other.viceCaptain->GetName());
        }
        
        // Set up playing XI, batting order, and bowling order
        for (const auto& player : other.playingXI) {
            Player* newPlayer = GetPlayer(player->GetName());
            if (newPlayer) {
                playingXI.push_back(newPlayer);
            }
        }
        
        for (const auto& player : other.battingOrder) {
            Player* newPlayer = GetPlayer(player->GetName());
            if (newPlayer) {
                battingOrder.push_back(newPlayer);
            }
        }
        
        for (const auto& player : other.bowlingOrder) {
            Player* newPlayer = GetPlayer(player->GetName());
            if (newPlayer) {
                bowlingOrder.push_back(newPlayer);
            }
        }
    }
    return *this;
}

Team& Team::operator=(Team&& other) noexcept {
    if (this != &other) {
        name = std::move(other.name);
        type = other.type;
        city = std::move(other.city);
        owner = std::move(other.owner);
        founded = other.founded;
        colors = std::move(other.colors);
        logo = std::move(other.logo);
        budget = other.budget;
        trophyCount = other.trophyCount;
        trophies = std::move(other.trophies);
        settings = std::move(other.settings);
        stats = std::move(other.stats);
        statsMap = std::move(other.statsMap);
        teamRating = other.teamRating;
        squad = std::move(other.squad);
        playingXI = std::move(other.playingXI);
        coach = std::move(other.coach);
        manager = std::move(other.manager);
        captain = other.captain;
        viceCaptain = other.viceCaptain;
        homeVenue = std::move(other.homeVenue);
        currentFormation = other.currentFormation;
        battingOrder = std::move(other.battingOrder);
        bowlingOrder = std::move(other.bowlingOrder);
        fieldingPositions = std::move(other.fieldingPositions);
        matchStrategy = std::move(other.matchStrategy);
        finances = std::move(other.finances);
        morale = std::move(other.morale);
        youthPlayers = std::move(other.youthPlayers);
    }
    return *this;
}

void Team::AddPlayer(std::unique_ptr<Player> player) {
    if (player) {
        player->SetCurrentTeam(name);
        squad.push_back(std::move(player));
        CalculateTeamRating();
    }
}

void Team::RemovePlayer(const std::string& playerName) {
    auto it = std::find_if(squad.begin(), squad.end(),
                          [&playerName](const std::unique_ptr<Player>& player) {
                              return player->GetName() == playerName;
                          });
    
    if (it != squad.end()) {
        // Remove from playing XI if present
        auto playingIt = std::find(playingXI.begin(), playingXI.end(), it->get());
        if (playingIt != playingXI.end()) {
            playingXI.erase(playingIt);
        }
        
        // Remove from batting order
        auto battingIt = std::find(battingOrder.begin(), battingOrder.end(), it->get());
        if (battingIt != battingOrder.end()) {
            battingOrder.erase(battingIt);
        }
        
        // Remove from bowling order
        auto bowlingIt = std::find(bowlingOrder.begin(), bowlingOrder.end(), it->get());
        if (bowlingIt != bowlingOrder.end()) {
            bowlingOrder.erase(bowlingIt);
        }
        
        // Remove from fielding positions
        fieldingPositions.erase(playerName);
        
        // Update captain/vice captain if needed
        if (captain == it->get()) {
            captain = nullptr;
        }
        if (viceCaptain == it->get()) {
            viceCaptain = nullptr;
        }
        
        squad.erase(it);
        CalculateTeamRating();
    }
}

Player* Team::GetPlayer(const std::string& playerName) {
    auto it = std::find_if(squad.begin(), squad.end(),
                          [&playerName](const std::unique_ptr<Player>& player) {
                              return player->GetName() == playerName;
                          });
    
    return (it != squad.end()) ? it->get() : nullptr;
}

std::vector<Player*> Team::GetAvailablePlayers() const {
    std::vector<Player*> available;
    for (const auto& player : squad) {
        if (!player->IsInjured() && player->GetFitnessLevel() > 50) {
            available.push_back(player.get());
        }
    }
    return available;
}

std::vector<Player*> Team::GetInjuredPlayers() const {
    std::vector<Player*> injured;
    for (const auto& player : squad) {
        if (player->IsInjured()) {
            injured.push_back(player.get());
        }
    }
    return injured;
}

void Team::SetPlayingXI(const std::vector<std::string>& playerNames) {
    playingXI.clear();
    for (const auto& playerName : playerNames) {
        Player* player = GetPlayer(playerName);
        if (player) {
            playingXI.push_back(player);
        }
    }
}

void Team::SetCaptain(const std::string& playerName) {
    Player* player = GetPlayer(playerName);
    if (player) {
        captain = player;
        player->AddSpecialty(PlayerSpecialty::ALL_ROUNDER); // Captains are typically all-rounders
    }
}

void Team::SetViceCaptain(const std::string& playerName) {
    Player* player = GetPlayer(playerName);
    if (player) {
        viceCaptain = player;
    }
}

void Team::SetBattingOrder(const std::vector<std::string>& order) {
    battingOrder.clear();
    for (const auto& playerName : order) {
        Player* player = GetPlayer(playerName);
        if (player) {
            battingOrder.push_back(player);
        }
    }
}

void Team::SetBowlingOrder(const std::vector<std::string>& order) {
    bowlingOrder.clear();
    for (const auto& playerName : order) {
        Player* player = GetPlayer(playerName);
        if (player) {
            bowlingOrder.push_back(player);
        }
    }
}

void Team::SetFieldingPositions(const std::map<std::string, std::string>& positions) {
    fieldingPositions = positions;
}

std::string Team::GetPlayerFieldingPosition(const std::string& playerName) const {
    auto it = fieldingPositions.find(playerName);
    return (it != fieldingPositions.end()) ? it->second : "Not Set";
}

void Team::UpdateStats(const TeamStats& newStats) {
    stats = newStats;
    
    // Recalculate win percentage
    if (stats.matchesPlayed > 0) {
        stats.winPercentage = static_cast<double>(stats.matchesWon) / stats.matchesPlayed * 100.0;
    }
}

void Team::CalculateTeamRating() {
    if (squad.empty()) {
        teamRating = 0;
        return;
    }
    
    int totalRating = 0;
    int playerCount = 0;
    
    for (const auto& player : squad) {
        totalRating += player->GetOverallRating();
        playerCount++;
    }
    
    teamRating = totalRating / playerCount;
    
    // Bonus for team chemistry
    teamRating += morale.teamChemistry / 10;
    
    // Bonus for captain
    if (captain) {
        teamRating += captain->GetPlayerAttributes().leadership / 20;
    }
}

void Team::UpdateFinances(const TeamFinances& newFinances) {
    finances = newFinances;
}

bool Team::CanAffordPlayer(int playerSalary) const {
    return (finances.budget - finances.currentSpending) >= playerSalary;
}

void Team::SpendMoney(int amount) {
    finances.currentSpending += amount;
    finances.totalExpenses += amount;
    finances.profit = finances.totalRevenue - finances.totalExpenses;
}

void Team::AddRevenue(int amount) {
    finances.totalRevenue += amount;
    finances.profit = finances.totalRevenue - finances.totalExpenses;
}

void Team::UpdateMorale() {
    // Calculate overall morale based on individual player morale
    int totalMorale = 0;
    int playerCount = 0;
    
    for (const auto& player : squad) {
        totalMorale += player->GetMorale();
        playerCount++;
    }
    
    if (playerCount > 0) {
        morale.overallMorale = totalMorale / playerCount;
    }
    
    // Update team chemistry based on player relationships
    CalculateTeamChemistry();
    
    // Update player satisfaction based on playing time and performance
    morale.playerSatisfaction = 75; // Base value, should be calculated based on actual data
}

void Team::UpdatePlayerMorale(const std::string& playerName, int change) {
    morale.individualMorale[playerName] += change;
    morale.individualMorale[playerName] = std::max(1, std::min(100, morale.individualMorale[playerName]));
    
    // Update overall morale
    UpdateMorale();
}

void Team::AddYouthPlayer(std::unique_ptr<Player> player) {
    if (player) {
        youthPlayers.push_back(std::move(player));
    }
}

std::vector<Player*> Team::GetYouthPlayers() const {
    std::vector<Player*> youth;
    for (const auto& player : youthPlayers) {
        youth.push_back(player.get());
    }
    return youth;
}

void Team::PromoteYouthPlayer(const std::string& playerName) {
    auto it = std::find_if(youthPlayers.begin(), youthPlayers.end(),
                          [&playerName](const std::unique_ptr<Player>& player) {
                              return player->GetName() == playerName;
                          });
    
    if (it != youthPlayers.end()) {
        AddPlayer(std::move(*it));
        youthPlayers.erase(it);
    }
}

void Team::RenewPlayerContract(const std::string& playerName, const Contract& newContract) {
    Player* player = GetPlayer(playerName);
    if (player) {
        player->SetContract(newContract);
        SpendMoney(newContract.salary);
    }
}

void Team::ReleasePlayer(const std::string& playerName) {
    Player* player = GetPlayer(playerName);
    if (player) {
        // Refund some of the contract money
        int refund = player->GetContract().salary / 2;
        AddRevenue(refund);
        RemovePlayer(playerName);
    }
}

std::vector<Player*> Team::GetPlayersWithExpiringContracts() const {
    std::vector<Player*> expiring;
    // TODO: Implement contract expiration logic
    return expiring;
}

void Team::PrepareForMatch() {
    // Update player fitness
    UpdatePlayerFitness();
    
    // Set default batting order if not set
    if (battingOrder.empty() && !playingXI.empty()) {
        battingOrder = playingXI;
    }
    
    // Set default bowling order if not set
    if (bowlingOrder.empty() && !playingXI.empty()) {
        // Find bowlers in playing XI
        for (Player* player : playingXI) {
            if (player->GetPlayerRole() == PlayerRole::BOWLER || 
                player->GetPlayerRole() == PlayerRole::ALL_ROUNDER) {
                bowlingOrder.push_back(player);
            }
        }
    }
}

void Team::UpdatePlayerFitness() {
    for (auto& player : squad) {
        player->UpdateInjuries();
        
        // Simulate fitness recovery
        if (!player->IsInjured()) {
            auto& attributes = player->GetPlayerAttributes();
            attributes.fitness = std::min(100, attributes.fitness + 5);
            attributes.stamina = std::min(100, attributes.stamina + 3);
        }
    }
}

void Team::SetMatchStrategy(const std::string& strategy) {
    matchStrategy = strategy;
}

void Team::CalculateTeamChemistry() {
    // Base chemistry value
    int baseChemistry = 70;
    
    // Bonus for players who have been together longer
    // Bonus for captain's leadership
    if (captain) {
        baseChemistry += captain->GetPlayerAttributes().leadership / 10;
    }
    
    // Penalty for too many new players
    int newPlayers = 0;
    for (const auto& player : squad) {
        if (player->GetPlayerAttributes().experience < 30) {
            newPlayers++;
        }
    }
    
    if (newPlayers > squad.size() / 3) {
        baseChemistry -= 10;
    }
    
    morale.teamChemistry = std::max(1, std::min(100, baseChemistry));
}

bool Team::IsPlayerInSquad(const std::string& playerName) const {
    return std::find_if(squad.begin(), squad.end(),
                       [&playerName](const std::unique_ptr<Player>& player) {
                           return player->GetName() == playerName;
                       }) != squad.end();
}

std::string Team::ToJSON() const {
    // TODO: Implement JSON serialization
    return "{\"name\":\"" + name + "\",\"type\":" + std::to_string(static_cast<int>(type)) + "}";
}

std::unique_ptr<Team> Team::FromJSON(const std::string& json) {
    // TODO: Implement JSON deserialization
    return std::make_unique<Team>("Unknown Team", TeamType::CLUB);
} 