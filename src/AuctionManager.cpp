#include "AuctionManager.h"
#include "AuctionVisualizer.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

AuctionManager::AuctionManager() 
    : randomGenerator(std::random_device{}())
    , aiBiddingEnabled(false)
    , biddingTimeSeconds(30)
    , remainingTimeSeconds(30)
    , timerActive(false)
    , minimumIncrement(1.0f)
    , maximumBid(1000.0f)
    , allowWithdrawals(true)
    , allowAutoBidding(true) {
}

AuctionManager::~AuctionManager() {
    cleanup();
}

bool AuctionManager::initialize() {
    // Initialize random number generator
    randomGenerator.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    
    // Initialize auction session
    currentSession.sessionName = "Default Auction";
    currentSession.type = AuctionType::IPL_STYLE;
    currentSession.isActive = false;
    currentSession.currentLotIndex = 0;
    currentSession.sessionBudget = 0.0f;
    currentSession.totalPlayers = 0;
    currentSession.soldPlayers = 0;
    currentSession.unsoldPlayers = 0;
    
    // Initialize current lot
    currentLot.player = nullptr;
    currentLot.basePrice = 0.0f;
    currentLot.reservePrice = 0.0f;
    currentLot.currentBid = 0.0f;
    currentLot.isSold = false;
    currentLot.isUnsold = false;
    currentLot.bidCount = 0;
    
    std::cout << "AuctionManager initialized successfully" << std::endl;
    return true;
}

void AuctionManager::cleanup() {
    // Cleanup auction data
    currentSession.lots.clear();
    currentSession.teamBudgets.clear();
    teamBudgets.clear();
    currentBidHistory.clear();
}

void AuctionManager::setVisualizer(std::shared_ptr<AuctionVisualizer> visualizer) {
    this->visualizer = visualizer;
}

void AuctionManager::createAuctionSession(const std::string& name, AuctionType type) {
    currentSession.sessionName = name;
    currentSession.type = type;
    currentSession.startTime = std::chrono::steady_clock::now();
    currentSession.isActive = false;
    currentSession.currentLotIndex = 0;
    currentSession.soldPlayers = 0;
    currentSession.unsoldPlayers = 0;
    
    std::cout << "Created auction session: " << name << std::endl;
}

void AuctionManager::addTeams(const std::vector<Team*>& teams) {
    for (const auto& team : teams) {
        TeamBudget budget;
        budget.teamName = team->getName();
        budget.totalBudget = team->getBudget();
        budget.spentAmount = 0.0f;
        budget.remainingBudget = team->getBudget();
        budget.playersBought = 0;
        budget.maxPlayers = 25;
        budget.strategy = BiddingStrategy::BALANCED;
        budget.aggressionLevel = 0.5f;
        
        teamBudgets[team->getName()] = budget;
        currentSession.teamBudgets.push_back(budget);
    }
    
    std::cout << "Added " << teams.size() << " teams to auction" << std::endl;
}

void AuctionManager::addPlayers(const std::vector<Player*>& players) {
    for (const auto& player : players) {
        AuctionLot lot;
        lot.player = player;
        lot.basePrice = calculateBasePrice(player);
        lot.reservePrice = lot.basePrice * 0.8f;
        lot.currentBid = 0.0f;
        lot.isSold = false;
        lot.isUnsold = false;
        lot.bidCount = 0;
        
        currentSession.lots.push_back(lot);
    }
    
    currentSession.totalPlayers = players.size();
    std::cout << "Added " << players.size() << " players to auction" << std::endl;
}

void AuctionManager::setTeamBudgets(const std::map<std::string, float>& budgets) {
    for (const auto& [teamName, budget] : budgets) {
        if (teamBudgets.find(teamName) != teamBudgets.end()) {
            teamBudgets[teamName].totalBudget = budget;
            teamBudgets[teamName].remainingBudget = budget;
        }
    }
}

void AuctionManager::setTeamStrategies(const std::map<std::string, BiddingStrategy>& strategies) {
    for (const auto& [teamName, strategy] : strategies) {
        if (teamBudgets.find(teamName) != teamBudgets.end()) {
            teamBudgets[teamName].strategy = strategy;
        }
    }
}

void AuctionManager::startAuction() {
    if (currentSession.lots.empty()) {
        std::cerr << "No players in auction!" << std::endl;
        return;
    }
    
    currentSession.isActive = true;
    currentSession.startTime = std::chrono::steady_clock::now();
    currentSession.currentLotIndex = 0;
    
    // Set first lot as current
    if (!currentSession.lots.empty()) {
        currentLot = currentSession.lots[0];
        currentLot.startTime = std::chrono::steady_clock::now();
    }
    
    std::cout << "Auction started!" << std::endl;
}

void AuctionManager::pauseAuction() {
    currentSession.isActive = false;
    timerActive = false;
    std::cout << "Auction paused" << std::endl;
}

void AuctionManager::resumeAuction() {
    currentSession.isActive = true;
    timerActive = true;
    std::cout << "Auction resumed" << std::endl;
}

void AuctionManager::endAuction() {
    currentSession.isActive = false;
    currentSession.endTime = std::chrono::steady_clock::now();
    timerActive = false;
    
    std::cout << "Auction ended!" << std::endl;
    
    if (auctionEndCallback) {
        auctionEndCallback();
    }
}

void AuctionManager::resetAuction() {
    currentSession.isActive = false;
    currentSession.currentLotIndex = 0;
    currentSession.soldPlayers = 0;
    currentSession.unsoldPlayers = 0;
    
    // Reset team budgets
    for (auto& [teamName, budget] : teamBudgets) {
        budget.spentAmount = 0.0f;
        budget.remainingBudget = budget.totalBudget;
        budget.playersBought = 0;
    }
    
    // Reset lots
    for (auto& lot : currentSession.lots) {
        lot.isSold = false;
        lot.isUnsold = false;
        lot.currentBid = 0.0f;
        lot.bidCount = 0;
    }
    
    currentBidHistory.clear();
    timerActive = false;
    
    std::cout << "Auction reset" << std::endl;
}

void AuctionManager::placeBid(const std::string& teamName, float amount) {
    if (!currentSession.isActive || !timerActive) {
        return;
    }
    
    if (!validateBid(teamName, amount)) {
        std::cout << "Invalid bid from " << teamName << ": " << amount << std::endl;
        return;
    }
    
    // Update current bid
    currentLot.currentBid = amount;
    currentLot.currentBidder = teamName;
    currentLot.bidCount++;
    
    // Add to bid history
    Bid bid;
    bid.teamName = teamName;
    bid.amount = amount;
    bid.timestamp = std::chrono::steady_clock::now();
    bid.strategy = getTeamStrategy(teamName);
    bid.isWinning = true;
    bid.reason = "Manual bid";
    
    currentLot.bidHistory.push_back(bid);
    currentBidHistory.push_back(bid);
    
    // Reset timer
    remainingTimeSeconds = biddingTimeSeconds;
    lotStartTime = std::chrono::steady_clock::now();
    
    // Update visualizer
    if (visualizer) {
        visualizer->onBidPlaced(teamName, amount);
    }
    
    // Trigger callback
    if (bidPlacedCallback) {
        bidPlacedCallback(teamName, amount);
    }
    
    std::cout << "Bid placed: " << teamName << " - " << amount << " lakhs" << std::endl;
}

void AuctionManager::autoBid(const std::string& teamName) {
    if (!aiBiddingEnabled) return;
    
    float bidAmount = calculateAIBid(teamName, currentLot);
    if (bidAmount > 0) {
        placeBid(teamName, bidAmount);
    }
}

void AuctionManager::withdrawBid(const std::string& teamName) {
    if (!allowWithdrawals) return;
    
    // Implementation for bid withdrawal
    std::cout << "Bid withdrawn by " << teamName << std::endl;
}

void AuctionManager::setBiddingTime(int seconds) {
    biddingTimeSeconds = seconds;
    remainingTimeSeconds = seconds;
}

void AuctionManager::setMinimumIncrement(float increment) {
    minimumIncrement = increment;
}

void AuctionManager::setCurrentLot(int lotIndex) {
    if (lotIndex >= 0 && lotIndex < currentSession.lots.size()) {
        currentSession.currentLotIndex = lotIndex;
        currentLot = currentSession.lots[lotIndex];
        currentLot.startTime = std::chrono::steady_clock::now();
        remainingTimeSeconds = biddingTimeSeconds;
        timerActive = true;
        
        if (lotChangedCallback) {
            lotChangedCallback(lotIndex);
        }
    }
}

void AuctionManager::nextLot() {
    if (currentSession.currentLotIndex < currentSession.lots.size() - 1) {
        setCurrentLot(currentSession.currentLotIndex + 1);
    }
}

void AuctionManager::previousLot() {
    if (currentSession.currentLotIndex > 0) {
        setCurrentLot(currentSession.currentLotIndex - 1);
    }
}

void AuctionManager::skipLot() {
    currentLot.isUnsold = true;
    currentSession.unsoldPlayers++;
    
    std::cout << "Lot skipped: " << currentLot.player->getName() << std::endl;
    
    nextLot();
}

void AuctionManager::unsoldLot() {
    currentLot.isUnsold = true;
    currentSession.unsoldPlayers++;
    
    std::cout << "Lot unsold: " << currentLot.player->getName() << std::endl;
    
    nextLot();
}

void AuctionManager::enableAIBidding(bool enabled) {
    aiBiddingEnabled = enabled;
}

void AuctionManager::setAIStrategy(const std::string& teamName, BiddingStrategy strategy) {
    aiStrategies[teamName] = strategy;
}

void AuctionManager::setAIAggression(const std::string& teamName, float aggression) {
    aiAggression[teamName] = glm::clamp(aggression, 0.0f, 1.0f);
}

void AuctionManager::simulateAIBidding() {
    if (!aiBiddingEnabled || !currentSession.isActive) return;
    
    for (const auto& [teamName, budget] : teamBudgets) {
        if (shouldAIBid(teamName, currentLot)) {
            autoBid(teamName);
        }
    }
}

std::map<std::string, float> AuctionManager::getTeamSpending() const {
    std::map<std::string, float> spending;
    for (const auto& [teamName, budget] : teamBudgets) {
        spending[teamName] = budget.spentAmount;
    }
    return spending;
}

std::map<std::string, int> AuctionManager::getTeamPlayerCount() const {
    std::map<std::string, int> counts;
    for (const auto& [teamName, budget] : teamBudgets) {
        counts[teamName] = budget.playersBought;
    }
    return counts;
}

std::vector<AuctionLot> AuctionManager::getSoldLots() const {
    std::vector<AuctionLot> soldLots;
    for (const auto& lot : currentSession.lots) {
        if (lot.isSold) {
            soldLots.push_back(lot);
        }
    }
    return soldLots;
}

std::vector<AuctionLot> AuctionManager::getUnsoldLots() const {
    std::vector<AuctionLot> unsoldLots;
    for (const auto& lot : currentSession.lots) {
        if (lot.isUnsold) {
            unsoldLots.push_back(lot);
        }
    }
    return unsoldLots;
}

float AuctionManager::getTotalRevenue() const {
    float total = 0.0f;
    for (const auto& lot : currentSession.lots) {
        if (lot.isSold) {
            total += lot.finalPrice;
        }
    }
    return total;
}

float AuctionManager::getAveragePrice() const {
    auto soldLots = getSoldLots();
    if (soldLots.empty()) return 0.0f;
    
    float total = getTotalRevenue();
    return total / soldLots.size();
}

std::string AuctionManager::getTopBidder() const {
    std::string topBidder;
    float highestBid = 0.0f;
    
    for (const auto& [teamName, budget] : teamBudgets) {
        if (budget.spentAmount > highestBid) {
            highestBid = budget.spentAmount;
            topBidder = teamName;
        }
    }
    
    return topBidder;
}

void AuctionManager::update(float deltaTime) {
    if (!currentSession.isActive) return;
    
    updateTimer();
    processBids();
    checkAuctionEnd();
    
    if (aiBiddingEnabled) {
        simulateAIBidding();
    }
}

void AuctionManager::processBids() {
    // Process any pending bids or AI bids
}

void AuctionManager::updateTimer() {
    if (!timerActive) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lotStartTime);
    remainingTimeSeconds = biddingTimeSeconds - elapsed.count();
    
    if (remainingTimeSeconds <= 0) {
        // Time's up - sell to current bidder or mark as unsold
        if (currentLot.currentBid > 0) {
            onBidWon(currentLot.currentBidder, currentLot.currentBid);
        } else {
            unsoldLot();
        }
    }
}

void AuctionManager::checkAuctionEnd() {
    if (currentSession.currentLotIndex >= currentSession.lots.size()) {
        endAuction();
    }
}

const AuctionLot& AuctionManager::getCurrentLot() const {
    return currentLot;
}

const std::vector<Bid>& AuctionManager::getCurrentBidHistory() const {
    return currentBidHistory;
}

int AuctionManager::getRemainingTime() const {
    return remainingTimeSeconds;
}

bool AuctionManager::isBiddingActive() const {
    return currentSession.isActive && timerActive && remainingTimeSeconds > 0;
}

void AuctionManager::setBidPlacedCallback(std::function<void(const std::string&, float)> callback) {
    bidPlacedCallback = callback;
}

void AuctionManager::setLotSoldCallback(std::function<void(const std::string&, const std::string&, float)> callback) {
    lotSoldCallback = callback;
}

void AuctionManager::setAuctionEndCallback(std::function<void()> callback) {
    auctionEndCallback = callback;
}

void AuctionManager::setLotChangedCallback(std::function<void(int)> callback) {
    lotChangedCallback = callback;
}

void AuctionManager::exportAuctionResults(const std::string& filename) const {
    // Implementation for exporting auction results
}

void AuctionManager::importAuctionData(const std::string& filename) {
    // Implementation for importing auction data
}

void AuctionManager::generateAuctionReport(const std::string& filename) const {
    // Implementation for generating auction report
}

// Private helper methods
void AuctionManager::updateTeamBudget(const std::string& teamName, float amount) {
    if (teamBudgets.find(teamName) != teamBudgets.end()) {
        teamBudgets[teamName].spentAmount += amount;
        teamBudgets[teamName].remainingBudget -= amount;
        teamBudgets[teamName].playersBought++;
    }
}

bool AuctionManager::validateBid(const std::string& teamName, float amount) {
    if (teamBudgets.find(teamName) == teamBudgets.end()) {
        return false;
    }
    
    const auto& budget = teamBudgets[teamName];
    
    // Check if team has enough budget
    if (amount > budget.remainingBudget) {
        return false;
    }
    
    // Check if bid is higher than current bid
    if (amount <= currentLot.currentBid) {
        return false;
    }
    
    // Check minimum increment
    if (amount < currentLot.currentBid + minimumIncrement) {
        return false;
    }
    
    return true;
}

float AuctionManager::calculateNextBid(float currentBid) {
    return currentBid + minimumIncrement;
}

float AuctionManager::calculateAIBid(const std::string& teamName, const AuctionLot& lot) {
    if (teamBudgets.find(teamName) == teamBudgets.end()) {
        return 0.0f;
    }
    
    const auto& budget = teamBudgets[teamName];
    float playerValue = calculatePlayerValue(lot.player);
    float marketDemand = calculateMarketDemand(lot.player);
    float aggression = getTeamAggression(teamName);
    
    // Calculate bid based on strategy and aggression
    float baseBid = lot.currentBid + minimumIncrement;
    float maxBid = playerValue * marketDemand * (1.0f + aggression);
    
    if (baseBid > maxBid || baseBid > budget.remainingBudget) {
        return 0.0f; // Don't bid
    }
    
    return baseBid;
}

bool AuctionManager::shouldAIBid(const std::string& teamName, const AuctionLot& lot) {
    if (teamBudgets.find(teamName) == teamBudgets.end()) {
        return false;
    }
    
    const auto& budget = teamBudgets[teamName];
    
    // Check if team needs this type of player
    // Check if team has budget
    // Check if player fits team strategy
    
    return true; // Simplified for now
}

BiddingStrategy AuctionManager::getTeamStrategy(const std::string& teamName) const {
    if (teamBudgets.find(teamName) != teamBudgets.end()) {
        return teamBudgets.at(teamName).strategy;
    }
    return BiddingStrategy::BALANCED;
}

float AuctionManager::getTeamAggression(const std::string& teamName) const {
    if (aiAggression.find(teamName) != aiAggression.end()) {
        return aiAggression.at(teamName);
    }
    return 0.5f;
}

float AuctionManager::calculatePlayerValue(const Player* player) const {
    if (!player) return 0.0f;
    
    float value = 50.0f; // Base value
    
    // Add value based on skills
    value += player->getBattingSkill() * 0.5f;
    value += player->getBowlingSkill() * 0.5f;
    value += player->getFieldingSkill() * 0.3f;
    value += player->getExperience() * 0.2f;
    
    // Adjust for age
    if (player->getAge() < 25) value *= 1.2f;
    else if (player->getAge() > 35) value *= 0.8f;
    
    return value;
}

float AuctionManager::calculateMarketDemand(const Player* player) const {
    if (!player) return 1.0f;
    
    // Simple demand calculation based on player role
    std::string role = player->getRole();
    if (role == "All-rounder") return 1.5f;
    if (role == "Batsman") return 1.3f;
    if (role == "Bowler") return 1.2f;
    if (role == "Wicketkeeper") return 1.4f;
    
    return 1.0f;
}

std::vector<std::string> AuctionManager::getInterestedTeams(const Player* player) const {
    std::vector<std::string> interested;
    
    for (const auto& [teamName, budget] : teamBudgets) {
        // Simple logic: teams with budget are interested
        if (budget.remainingBudget > 50.0f) {
            interested.push_back(teamName);
        }
    }
    
    return interested;
}

void AuctionManager::logBid(const std::string& teamName, float amount, const std::string& reason) {
    std::cout << "Bid logged: " << teamName << " - " << amount << " lakhs (" << reason << ")" << std::endl;
}

void AuctionManager::updateVisualizer() {
    if (visualizer) {
        // Update visualizer with current auction state
    }
}

void AuctionManager::triggerCallbacks() {
    // Trigger any pending callbacks
}

std::string AuctionManager::formatCurrency(float amount) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount << " lakhs";
    return oss.str();
}

std::string AuctionManager::formatTime(int seconds) const {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << secs;
    return oss.str();
}

void AuctionManager::onBidWon(const std::string& teamName, float finalBid) {
    currentLot.isSold = true;
    currentLot.soldTo = teamName;
    currentLot.finalPrice = finalBid;
    currentLot.endTime = std::chrono::steady_clock::now();
    
    currentSession.soldPlayers++;
    
    // Update team budget
    updateTeamBudget(teamName, finalBid);
    
    // Update visualizer
    if (visualizer) {
        visualizer->onBidWon(teamName, finalBid);
    }
    
    // Trigger callbacks
    if (lotSoldCallback) {
        lotSoldCallback(currentLot.player->getName(), teamName, finalBid);
    }
    
    std::cout << "Player sold: " << currentLot.player->getName() << " to " << teamName << " for " << finalBid << " lakhs" << std::endl;
    
    // Move to next lot
    nextLot();
}

