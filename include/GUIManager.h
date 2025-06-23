#pragma once

#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include "Game.h"

// Forward declarations
class Game;
class Player;
class Team;
class MatchEngine;
class AuctionManager;

enum class GUITab {
    DASHBOARD,
    SQUAD,
    TACTICS,
    MATCH_DAY,
    TRANSFERS,
    AUCTION,
    CAREER,
    SETTINGS
};

enum class GUITheme {
    DARK,
    LIGHT,
    BLUE,
    GREEN
};

struct GUIStyle {
    ImVec4 primaryColor;
    ImVec4 secondaryColor;
    ImVec4 accentColor;
    ImVec4 backgroundColor;
    ImVec4 textColor;
    ImVec4 borderColor;
    float borderRadius;
    float padding;
    float spacing;
};

struct PlayerCard {
    std::string name;
    int rating;
    std::string role;
    std::string team;
    bool isSelected;
    bool isInjured;
    int form;
    int morale;
};

struct TeamCard {
    std::string name;
    int rating;
    int players;
    int budget;
    std::string coach;
    bool isSelected;
};

struct MatchInfo {
    std::string team1;
    std::string team2;
    std::string venue;
    std::string format;
    int score1;
    int score2;
    int overs1;
    int overs2;
    std::string status;
};

class GUIManager {
public:
    GUIManager(Game* game);
    ~GUIManager();
    
    // Initialization
    bool Initialize();
    void Shutdown();
    
    // Main GUI loop
    void BeginFrame();
    void Render();
    void EndFrame();
    
    // Theme and styling
    void SetTheme(GUITheme theme);
    void SetCustomStyle(const GUIStyle& style);
    void LoadCustomFont(const std::string& fontPath, float fontSize);
    
    // Tab management
    void SetActiveTab(GUITab tab);
    GUITab GetActiveTab() const { return activeTab; }
    void ShowTab(GUITab tab, bool show);
    bool IsTabVisible(GUITab tab) const;
    
    // Window management
    void SetWindowSize(int width, int height);
    void SetFullscreen(bool fullscreen);
    bool IsFullscreen() const { return isFullscreen; }
    
    // Data binding
    void SetPlayerData(const std::vector<Player*>& players);
    void SetTeamData(const std::vector<Team*>& teams);
    void SetMatchData(const MatchInfo& matchInfo);
    void SetAuctionData(const std::vector<Player*>& auctionPlayers);
    
    // Callbacks
    void SetPlayerSelectedCallback(std::function<void(Player*)> callback);
    void SetTeamSelectedCallback(std::function<void(Team*)> callback);
    void SetMatchActionCallback(std::function<void(const std::string&)> callback);
    void SetAuctionBidCallback(std::function<void(Player*, int)> callback);
    void SetConfirmDialogCallback(std::function<void(bool)> callback);
    
    // Notifications
    void ShowNotification(const std::string& message, const std::string& type = "info");
    void ShowDialog(const std::string& title, const std::string& message);
    void ShowConfirmDialog(const std::string& title, const std::string& message, 
                          std::function<void(bool)> callback);
    
    // Charts and statistics
    void RenderPlayerStatsChart(Player* player);
    void RenderTeamStatsChart(Team* team);
    void RenderMatchProgressChart();
    void RenderAuctionHistoryChart();
    
    // Update method
    void Update(float deltaTime);
    
    // Rendering methods
    void RenderNotifications();
    void RenderConfirmDialog();
    
private:
    // Main GUI components
    void RenderMainMenu();
    void RenderDashboard();
    void RenderSquadManagement();
    void RenderTactics();
    void RenderMatchDay();
    void RenderTransfers();
    void RenderAuction();
    void RenderCareer();
    void RenderSettings();
    
    // Dashboard components
    void RenderQuickStats();
    void RenderRecentMatches();
    void RenderUpcomingFixtures();
    void RenderTeamMorale();
    void RenderFinancialSummary();
    
    // Squad management components
    void RenderPlayerList();
    void RenderPlayerDetails();
    void RenderPlayerComparison();
    void RenderSquadAnalysis();
    void RenderYouthDevelopment();
    
    // Tactics components
    void RenderBattingOrder();
    void RenderBowlingOrder();
    void RenderFieldingPositions();
    void RenderMatchStrategy();
    void RenderFormationSelector();
    
    // Match day components
    void RenderMatchOverview();
    void RenderLiveScorecard();
    void RenderBallByBall();
    void RenderCommentary();
    void RenderMatchControls();
    void RenderPlayerPerformance();
    
    // Transfer components
    void RenderTransferList();
    void RenderPlayerScouting();
    void RenderContractNegotiations();
    void RenderTransferHistory();
    
    // Auction components
    void RenderAuctionRoom();
    void RenderPlayerAuction();
    void RenderBiddingInterface();
    void RenderTeamBudgets();
    void RenderAuctionHistory();
    
    // Career components
    void RenderCareerOverview();
    void RenderSeasonProgress();
    void RenderAchievements();
    void RenderCareerStats();
    
    // Settings components
    void RenderGameSettings();
    void RenderDisplaySettings();
    void RenderAudioSettings();
    void RenderControlSettings();
    
    // Utility components
    void RenderPlayerCard(const PlayerCard& card);
    void RenderTeamCard(const TeamCard& card);
    void RenderStatBar(const std::string& label, float value, float maxValue);
    void RenderProgressBar(const std::string& label, float progress);
    void RenderButton(const std::string& label, std::function<void()> callback);
    void RenderDropdown(const std::string& label, const std::vector<std::string>& options, int& selected);
    
    // Data management
    void UpdatePlayerCards();
    void UpdateTeamCards();
    void UpdateMatchInfo();
    void UpdateAuctionData();
    
    // Helper methods
    std::string FormatCurrency(int amount);
    std::string FormatPercentage(float value);
    std::string FormatTime(int seconds);
    ImVec4 GetColorForRating(int rating);
    ImVec4 GetColorForForm(int form);
    ImVec4 GetColorForMorale(int morale);
    
    // Game reference
    Game* game;
    
    // GUI state
    GUITab activeTab;
    GUITheme currentTheme;
    GUIStyle customStyle;
    bool isFullscreen;
    
    // Data
    std::vector<PlayerCard> playerCards;
    std::vector<TeamCard> teamCards;
    MatchInfo currentMatch;
    std::vector<Player*> auctionPlayers;
    
    // Callbacks
    std::function<void(Player*)> playerSelectedCallback;
    std::function<void(Team*)> teamSelectedCallback;
    std::function<void(const std::string&)> matchActionCallback;
    std::function<void(Player*, int)> auctionBidCallback;
    std::function<void(bool)> confirmCallback;
    
    // UI state
    bool showNotifications;
    std::vector<std::string> notifications;
    bool showConfirmDialog;
    std::string confirmTitle;
    std::string confirmMessage;
    
    // Tab visibility
    std::map<GUITab, bool> tabVisibility;
}; 