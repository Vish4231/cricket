#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <queue>
#include <functional>
#include <random>
#include <chrono>
#include "Player.h"
#include "Team.h"

// Forward declaration
class AuctionVisualizer;

enum class AuctionType {
    IPL_STYLE,
    ENGLISH_AUCTION,
    DUTCH_AUCTION,
    SEALED_BID,
    REVERSE_AUCTION
};

enum class BiddingStrategy {
    AGGRESSIVE,
    CONSERVATIVE,
    BALANCED,
    DESPERATE,
    STRATEGIC
};

struct Bid {
    std::string teamName;
    float amount;
    std::chrono::steady_clock::time_point timestamp;
    BiddingStrategy strategy;
    bool isWinning;
    std::string reason;
};

struct AuctionLot {
    Player* player;
    float basePrice;
    float reservePrice;
    float currentBid;
    std::string currentBidder;
    std::vector<Bid> bidHistory;
    bool isSold;
    bool isUnsold;
    std::string soldTo;
    float finalPrice;
    int bidCount;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
};

struct TeamBudget {
    std::string teamName;
    float totalBudget;
    float spentAmount;
    float remainingBudget;
    int playersBought;
    int maxPlayers;
    std::vector<std::string> requirements; // "Batsman", "Bowler", etc.
    BiddingStrategy strategy;
    float aggressionLevel; // 0.0 to 1.0
};

struct AuctionSession {
    std::string sessionName;
    AuctionType type;
    std::vector<AuctionLot> lots;
    std::vector<TeamBudget> teamBudgets;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    bool isActive;
    int currentLotIndex;
    float sessionBudget;
    int totalPlayers;
    int soldPlayers;
    int unsoldPlayers;
};

class AuctionManager {
public:
    AuctionManager();
    ~AuctionManager();
    
    // Initialization
    bool initialize();
    void cleanup();
    void setVisualizer(std::shared_ptr<AuctionVisualizer> visualizer);
    
    // Auction setup
    void createAuctionSession(const std::string& name, AuctionType type);
    void addTeams(const std::vector<Team*>& teams);
    void addPlayers(const std::vector<Player*>& players);
    void setTeamBudgets(const std::map<std::string, float>& budgets);
    void setTeamStrategies(const std::map<std::string, BiddingStrategy>& strategies);
    
    // Auction control
    void startAuction();
    void pauseAuction();
    void resumeAuction();
    void endAuction();
    void resetAuction();
    
    // Bidding mechanics
    void placeBid(const std::string& teamName, float amount);
    void autoBid(const std::string& teamName);
    void withdrawBid(const std::string& teamName);
    void setBiddingTime(int seconds);
    void setMinimumIncrement(float increment);
    
    // Lot management
    void setCurrentLot(int lotIndex);
    void nextLot();
    void previousLot();
    void skipLot();
    void unsoldLot();
    
    // AI bidding
    void enableAIBidding(bool enabled);
    void setAIStrategy(const std::string& teamName, BiddingStrategy strategy);
    void setAIAggression(const std::string& teamName, float aggression);
    void simulateAIBidding();
    
    // Statistics and analysis
    std::map<std::string, float> getTeamSpending() const;
    std::map<std::string, int> getTeamPlayerCount() const;
    std::vector<AuctionLot> getSoldLots() const;
    std::vector<AuctionLot> getUnsoldLots() const;
    float getTotalRevenue() const;
    float getAveragePrice() const;
    std::string getTopBidder() const;
    
    // Real-time updates
    void update(float deltaTime);
    void processBids();
    void updateTimer();
    void checkAuctionEnd();
    
    // Getters
    AuctionSession& getCurrentSession() { return currentSession; }
    const AuctionLot& getCurrentLot() const;
    const std::vector<Bid>& getCurrentBidHistory() const;
    float getCurrentBid() const { return currentLot.currentBid; }
    std::string getCurrentBidder() const { return currentLot.currentBidder; }
    int getRemainingTime() const;
    bool isAuctionActive() const { return currentSession.isActive; }
    bool isBiddingActive() const;
    
    // Callbacks
    void setBidPlacedCallback(std::function<void(const std::string&, float)> callback);
    void setLotSoldCallback(std::function<void(const std::string&, const std::string&, float)> callback);
    void setAuctionEndCallback(std::function<void()> callback);
    void setLotChangedCallback(std::function<void(int)> callback);
    
    // Export and import
    void exportAuctionResults(const std::string& filename) const;
    void importAuctionData(const std::string& filename);
    void generateAuctionReport(const std::string& filename) const;

private:
    // Core auction data
    AuctionSession currentSession;
    AuctionLot currentLot;
    std::map<std::string, TeamBudget> teamBudgets;
    std::vector<Bid> currentBidHistory;
    
    // Timing
    std::chrono::steady_clock::time_point auctionStartTime;
    std::chrono::steady_clock::time_point lotStartTime;
    int biddingTimeSeconds;
    int remainingTimeSeconds;
    bool timerActive;
    
    // Bidding rules
    float minimumIncrement;
    float maximumBid;
    bool allowWithdrawals;
    bool allowAutoBidding;
    
    // AI settings
    bool aiBiddingEnabled;
    std::map<std::string, BiddingStrategy> aiStrategies;
    std::map<std::string, float> aiAggression;
    std::random_device randomDevice;
    std::mt19937 randomGenerator;
    
    // Visualization
    std::shared_ptr<AuctionVisualizer> visualizer;
    
    // Callbacks
    std::function<void(const std::string&, float)> bidPlacedCallback;
    std::function<void(const std::string&, const std::string&, float)> lotSoldCallback;
    std::function<void()> auctionEndCallback;
    std::function<void(int)> lotChangedCallback;
    
    // Helper methods
    void initializeLot(AuctionLot& lot);
    void processLotEnd();
    void updateTeamBudget(const std::string& teamName, float amount);
    bool validateBid(const std::string& teamName, float amount);
    float calculateNextBid(float currentBid);
    
    // AI methods
    float calculateAIBid(const std::string& teamName, const AuctionLot& lot);
    bool shouldAIBid(const std::string& teamName, const AuctionLot& lot);
    BiddingStrategy getTeamStrategy(const std::string& teamName) const;
    float getTeamAggression(const std::string& teamName) const;
    
    // Analysis methods
    float calculatePlayerValue(const Player* player) const;
    float calculateMarketDemand(const Player* player) const;
    std::vector<std::string> getInterestedTeams(const Player* player) const;
    
    // Utility methods
    void logBid(const std::string& teamName, float amount, const std::string& reason = "");
    void updateVisualizer();
    void triggerCallbacks();
    std::string formatCurrency(float amount) const;
    std::string formatTime(int seconds) const;
}; 