#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "GUIManager.h"
#include "Game.h"
#include "Player.h"
#include "Team.h"
#include <iostream>
#include <algorithm>

GUIManager::GUIManager(Game* g)
    : game(g)
    , activeTab(GUITab::DASHBOARD)
    , currentTheme(GUITheme::DARK)
    , isFullscreen(false)
    , showNotifications(false)
    , showConfirmDialog(false)
    , uiRenderer(std::make_unique<UIRenderer>())
{
    // Initialize tab visibility
    tabVisibility[GUITab::DASHBOARD] = true;
    tabVisibility[GUITab::SQUAD] = true;
    tabVisibility[GUITab::TACTICS] = true;
    tabVisibility[GUITab::MATCH_DAY] = true;
    tabVisibility[GUITab::TRANSFERS] = true;
    tabVisibility[GUITab::AUCTION] = true;
    tabVisibility[GUITab::CAREER] = true;
    tabVisibility[GUITab::SETTINGS] = true;
}

GUIManager::~GUIManager() {
}

bool GUIManager::Initialize() {
    // Initialize UIRenderer
    if (!uiRenderer->initialize()) {
        std::cerr << "Failed to initialize UIRenderer" << std::endl;
        return false;
    }
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Comment out flags that may not be available in this ImGui version
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Setup ImGui style
    SetTheme(currentTheme);
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(game->GetWindow(), game->GetGLContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    
    std::cout << "GUI Manager initialized successfully" << std::endl;
    return true;
}

void GUIManager::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void GUIManager::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void GUIManager::Render() {
    // Render main menu bar
    RenderMainMenu();
    
    // Render main content area
    ImGui::Begin("Cricket Manager", nullptr, ImGuiWindowFlags_MenuBar);
    
    // Render tab bar
    if (ImGui::BeginTabBar("MainTabs")) {
        if (IsTabVisible(GUITab::DASHBOARD) && ImGui::BeginTabItem("Dashboard")) {
            RenderDashboard();
            ImGui::EndTabItem();
        }
        
        if (IsTabVisible(GUITab::SQUAD) && ImGui::BeginTabItem("Squad")) {
            RenderSquadManagement();
            ImGui::EndTabItem();
        }
        
        if (IsTabVisible(GUITab::TACTICS) && ImGui::BeginTabItem("Tactics")) {
            RenderTactics();
            ImGui::EndTabItem();
        }
        
        if (IsTabVisible(GUITab::MATCH_DAY) && ImGui::BeginTabItem("Match Day")) {
            RenderMatchDay();
            ImGui::EndTabItem();
        }
        
        if (IsTabVisible(GUITab::TRANSFERS) && ImGui::BeginTabItem("Transfers")) {
            RenderTransfers();
            ImGui::EndTabItem();
        }
        
        if (IsTabVisible(GUITab::AUCTION) && ImGui::BeginTabItem("Auction")) {
            RenderAuction();
            ImGui::EndTabItem();
        }
        
        if (IsTabVisible(GUITab::CAREER) && ImGui::BeginTabItem("Career")) {
            RenderCareer();
            ImGui::EndTabItem();
        }
        
        if (IsTabVisible(GUITab::SETTINGS) && ImGui::BeginTabItem("Settings")) {
            RenderSettings();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
    
    // Render notifications
    if (showNotifications) {
        RenderNotifications();
    }
    
    // Render confirm dialog
    if (showConfirmDialog) {
        RenderConfirmDialog();
    }
}

void GUIManager::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIManager::SetTheme(GUITheme theme) {
    currentTheme = theme;
    
    ImGuiStyle& style = ImGui::GetStyle();
    
    switch (theme) {
        case GUITheme::DARK:
            ImGui::StyleColorsDark();
            break;
        case GUITheme::LIGHT:
            ImGui::StyleColorsLight();
            break;
        case GUITheme::BLUE:
            ImGui::StyleColorsDark();
            style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.3f, 0.8f, 1.0f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.4f, 0.9f, 1.0f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.1f, 0.2f, 0.7f, 1.0f);
            break;
        case GUITheme::GREEN:
            ImGui::StyleColorsDark();
            style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.8f, 0.3f, 1.0f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.9f, 0.4f, 1.0f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.1f, 0.7f, 0.2f, 1.0f);
            break;
    }
}

void GUIManager::SetCustomStyle(const GUIStyle& style) {
    customStyle = style;
    
    ImGuiStyle& imguiStyle = ImGui::GetStyle();
    imguiStyle.Colors[ImGuiCol_Text] = style.textColor;
    imguiStyle.Colors[ImGuiCol_WindowBg] = style.backgroundColor;
    imguiStyle.Colors[ImGuiCol_Border] = style.borderColor;
    imguiStyle.FramePadding = ImVec2(style.padding, style.padding);
    imguiStyle.ItemSpacing = ImVec2(style.spacing, style.spacing);
}

void GUIManager::LoadCustomFont(const std::string& fontPath, float fontSize) {
    // TODO: Implement custom font loading
}

void GUIManager::SetActiveTab(GUITab tab) {
    activeTab = tab;
}

void GUIManager::ShowTab(GUITab tab, bool show) {
    tabVisibility[tab] = show;
}

bool GUIManager::IsTabVisible(GUITab tab) const {
    auto it = tabVisibility.find(tab);
    return (it != tabVisibility.end()) ? it->second : false;
}

void GUIManager::SetWindowSize(int width, int height) {
    // TODO: Implement window resizing
}

void GUIManager::SetFullscreen(bool fullscreen) {
    isFullscreen = fullscreen;
    // TODO: Implement fullscreen toggle
}

void GUIManager::SetPlayerData(const std::vector<Player*>& players) {
    UpdatePlayerCards();
}

void GUIManager::SetTeamData(const std::vector<Team*>& teams) {
    UpdateTeamCards();
}

void GUIManager::SetMatchData(const MatchInfo& matchInfo) {
    currentMatch = matchInfo;
}

void GUIManager::SetAuctionData(const std::vector<Player*>& auctionPlayers) {
    this->auctionPlayers = auctionPlayers;
}

void GUIManager::SetPlayerSelectedCallback(std::function<void(Player*)> callback) {
    playerSelectedCallback = callback;
}

void GUIManager::SetTeamSelectedCallback(std::function<void(Team*)> callback) {
    teamSelectedCallback = callback;
}

void GUIManager::SetMatchActionCallback(std::function<void(const std::string&)> callback) {
    matchActionCallback = callback;
}

void GUIManager::SetAuctionBidCallback(std::function<void(Player*, int)> callback) {
    auctionBidCallback = callback;
}

void GUIManager::ShowNotification(const std::string& message, const std::string& type) {
    notifications.push_back(message);
    showNotifications = true;
}

void GUIManager::ShowDialog(const std::string& title, const std::string& message) {
    // TODO: Implement dialog
}

void GUIManager::ShowConfirmDialog(const std::string& title, const std::string& message, 
                                  std::function<void(bool)> callback) {
    confirmTitle = title;
    confirmMessage = message;
    confirmCallback = callback;
    showConfirmDialog = true;
}

void GUIManager::RenderPlayerStatsChart(Player* player) {
    // TODO: Implement player stats chart
}

void GUIManager::RenderTeamStatsChart(Team* team) {
    // TODO: Implement team stats chart
}

void GUIManager::RenderMatchProgressChart() {
    // TODO: Implement match progress chart
}

void GUIManager::RenderAuctionHistoryChart() {
    // TODO: Implement auction history chart
}

void GUIManager::Update(float deltaTime) {
    // TODO: Implement GUI updates
}

// Main GUI components
void GUIManager::RenderMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Game")) {
                // TODO: Implement new game
            }
            if (ImGui::MenuItem("Load Game")) {
                // TODO: Implement load game
            }
            if (ImGui::MenuItem("Save Game")) {
                // TODO: Implement save game
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                // TODO: Implement exit
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Fullscreen", nullptr, isFullscreen)) {
                SetFullscreen(!isFullscreen);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                ShowDialog("About", "Cricket Manager v1.0.0\nA professional cricket management simulation game.");
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void GUIManager::RenderDashboard() {
    ImGui::Text("Welcome to Cricket Manager!");
    ImGui::Separator();
    
    RenderQuickStats();
    ImGui::Separator();
    RenderRecentMatches();
    ImGui::Separator();
    RenderUpcomingFixtures();
}

void GUIManager::RenderSquadManagement() {
    ImGui::Text("Squad Management");
    ImGui::Separator();
    
    RenderPlayerList();
    ImGui::Separator();
    RenderPlayerDetails();
}

void GUIManager::RenderTactics() {
    ImGui::Text("Tactics & Strategy");
    ImGui::Separator();
    
    RenderBattingOrder();
    ImGui::Separator();
    RenderBowlingOrder();
    ImGui::Separator();
    RenderFieldingPositions();
}

void GUIManager::RenderMatchDay() {
    ImGui::Text("Match Day");
    ImGui::Separator();
    
    RenderMatchOverview();
    ImGui::Separator();
    RenderLiveScorecard();
    ImGui::Separator();
    RenderCommentary();
}

void GUIManager::RenderTransfers() {
    ImGui::Text("Transfers & Contracts");
    ImGui::Separator();
    
    RenderTransferList();
    ImGui::Separator();
    RenderPlayerScouting();
}

void GUIManager::RenderAuction() {
    ImGui::Text("Player Auction");
    ImGui::Separator();
    
    RenderAuctionRoom();
    ImGui::Separator();
    RenderBiddingInterface();
}

void GUIManager::RenderCareer() {
    ImGui::Text("Career Mode");
    ImGui::Separator();
    
    RenderCareerOverview();
    ImGui::Separator();
    RenderSeasonProgress();
}

void GUIManager::RenderSettings() {
    ImGui::Text("Settings");
    ImGui::Separator();
    
    RenderGameSettings();
    ImGui::Separator();
    RenderDisplaySettings();
}

// Dashboard components
void GUIManager::RenderQuickStats() {
    ImGui::Text("Quick Stats");
    ImGui::Text("Team Rating: 85");
    ImGui::Text("Players: 25");
    ImGui::Text("Budget: ₹10,00,00,000");
    ImGui::Text("Morale: 75%");
}

void GUIManager::RenderRecentMatches() {
    ImGui::Text("Recent Matches");
    ImGui::Text("Mumbai Indians vs Chennai Super Kings - Won by 5 wickets");
    ImGui::Text("Royal Challengers vs Kolkata Knight Riders - Lost by 3 runs");
}

void GUIManager::RenderUpcomingFixtures() {
    ImGui::Text("Upcoming Fixtures");
    ImGui::Text("Next: vs Delhi Capitals (Tomorrow)");
    ImGui::Text("Venue: Wankhede Stadium");
}

void GUIManager::RenderTeamMorale() {
    ImGui::Text("Team Morale");
    RenderProgressBar("Overall", 0.75f);
    RenderProgressBar("Chemistry", 0.80f);
    RenderProgressBar("Satisfaction", 0.70f);
}

void GUIManager::RenderFinancialSummary() {
    ImGui::Text("Financial Summary");
    ImGui::Text("Revenue: ₹5,00,00,000");
    ImGui::Text("Expenses: ₹3,00,00,000");
    ImGui::Text("Profit: ₹2,00,00,000");
}

// Squad management components
void GUIManager::RenderPlayerList() {
    ImGui::Text("Player List");
    
    for (const auto& card : playerCards) {
        RenderPlayerCard(card);
    }
}

void GUIManager::RenderPlayerDetails() {
    ImGui::Text("Player Details");
    ImGui::Text("Select a player to view details");
}

void GUIManager::RenderPlayerComparison() {
    ImGui::Text("Player Comparison");
}

void GUIManager::RenderSquadAnalysis() {
    ImGui::Text("Squad Analysis");
}

void GUIManager::RenderYouthDevelopment() {
    ImGui::Text("Youth Development");
}

// Tactics components
void GUIManager::RenderBattingOrder() {
    ImGui::Text("Batting Order");
    ImGui::Text("1. Virat Kohli");
    ImGui::Text("2. Rohit Sharma");
    ImGui::Text("3. MS Dhoni");
}

void GUIManager::RenderBowlingOrder() {
    ImGui::Text("Bowling Order");
    ImGui::Text("1. Jasprit Bumrah");
    ImGui::Text("2. Ravindra Jadeja");
    ImGui::Text("3. Yuzvendra Chahal");
}

void GUIManager::RenderFieldingPositions() {
    ImGui::Text("Fielding Positions");
}

void GUIManager::RenderMatchStrategy() {
    ImGui::Text("Match Strategy");
}

void GUIManager::RenderFormationSelector() {
    ImGui::Text("Formation Selector");
}

// Match day components
void GUIManager::RenderMatchOverview() {
    ImGui::Text("Match Overview");
    ImGui::Text("Mumbai Indians vs Chennai Super Kings");
    ImGui::Text("T20 Match");
    ImGui::Text("Wankhede Stadium");
}

void GUIManager::RenderLiveScorecard() {
    ImGui::Text("Live Scorecard");
    ImGui::Text("Mumbai Indians: 120/4 (15.2 overs)");
    ImGui::Text("Run Rate: 7.85");
    ImGui::Text("Required: 45 from 28 balls");
}

void GUIManager::RenderBallByBall() {
    ImGui::Text("Ball by Ball");
}

void GUIManager::RenderCommentary() {
    ImGui::Text("Commentary");
    ImGui::Text("15.1: FOUR! Beautiful shot by Kohli!");
    ImGui::Text("15.2: Single taken, good running between the wickets.");
}

void GUIManager::RenderMatchControls() {
    ImGui::Text("Match Controls");
    if (ImGui::Button("Play Ball")) {
        if (matchActionCallback) {
            matchActionCallback("play_ball");
        }
    }
    if (ImGui::Button("Pause")) {
        if (matchActionCallback) {
            matchActionCallback("pause");
        }
    }
}

void GUIManager::RenderPlayerPerformance() {
    ImGui::Text("Player Performance");
}

// Transfer components
void GUIManager::RenderTransferList() {
    ImGui::Text("Transfer List");
}

void GUIManager::RenderPlayerScouting() {
    ImGui::Text("Player Scouting");
}

void GUIManager::RenderContractNegotiations() {
    ImGui::Text("Contract Negotiations");
}

void GUIManager::RenderTransferHistory() {
    ImGui::Text("Transfer History");
}

// Auction components
void GUIManager::RenderAuctionRoom() {
    ImGui::Text("Auction Room");
}

void GUIManager::RenderPlayerAuction() {
    ImGui::Text("Player Auction");
}

void GUIManager::RenderBiddingInterface() {
    ImGui::Text("Bidding Interface");
    static int bidAmount = 1000000;
    ImGui::InputInt("Bid Amount", &bidAmount);
    if (ImGui::Button("Place Bid")) {
        // TODO: Implement bid placement
    }
}

void GUIManager::RenderTeamBudgets() {
    ImGui::Text("Team Budgets");
}

void GUIManager::RenderAuctionHistory() {
    ImGui::Text("Auction History");
}

// Career components
void GUIManager::RenderCareerOverview() {
    ImGui::Text("Career Overview");
}

void GUIManager::RenderSeasonProgress() {
    ImGui::Text("Season Progress");
}

void GUIManager::RenderAchievements() {
    ImGui::Text("Achievements");
}

void GUIManager::RenderCareerStats() {
    ImGui::Text("Career Stats");
}

// Settings components
void GUIManager::RenderGameSettings() {
    ImGui::Text("Game Settings");
}

void GUIManager::RenderDisplaySettings() {
    ImGui::Text("Display Settings");
}

void GUIManager::RenderAudioSettings() {
    ImGui::Text("Audio Settings");
}

void GUIManager::RenderControlSettings() {
    ImGui::Text("Control Settings");
}

// Utility components
void GUIManager::RenderPlayerCard(const PlayerCard& card) {
    ImGui::BeginChild(("PlayerCard_" + card.name).c_str(), ImVec2(200, 100), true);
    ImGui::Text("%s", card.name.c_str());
    ImGui::Text("Rating: %d", card.rating);
    ImGui::Text("Role: %s", card.role.c_str());
    ImGui::Text("Team: %s", card.team.c_str());
    if (card.isInjured) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "INJURED");
    }
    ImGui::EndChild();
}

void GUIManager::RenderTeamCard(const TeamCard& card) {
    ImGui::BeginChild(("TeamCard_" + card.name).c_str(), ImVec2(200, 100), true);
    ImGui::Text("%s", card.name.c_str());
    ImGui::Text("Rating: %d", card.rating);
    ImGui::Text("Players: %d", card.players);
    ImGui::Text("Budget: ₹%d", card.budget);
    ImGui::EndChild();
}

void GUIManager::RenderStatBar(const std::string& label, float value, float maxValue) {
    ImGui::Text("%s: %.1f/%.1f", label.c_str(), value, maxValue);
    ImGui::ProgressBar(value / maxValue);
}

void GUIManager::RenderProgressBar(const std::string& label, float progress) {
    ImGui::Text("%s", label.c_str());
    ImGui::ProgressBar(progress);
}

void GUIManager::RenderButton(const std::string& label, std::function<void()> callback) {
    if (ImGui::Button(label.c_str())) {
        if (callback) {
            callback();
        }
    }
}

void GUIManager::RenderDropdown(const std::string& label, const std::vector<std::string>& options, int& selected) {
    if (ImGui::BeginCombo(label.c_str(), options[selected].c_str())) {
        for (int i = 0; i < static_cast<int>(options.size()); i++) {
            const bool isSelected = (selected == i);
            if (ImGui::Selectable(options[i].c_str(), isSelected)) {
                selected = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void GUIManager::UpdatePlayerCards() {
    // TODO: Update player cards from game data
}

void GUIManager::UpdateTeamCards() {
    // TODO: Update team cards from game data
}

void GUIManager::UpdateMatchInfo() {
    // TODO: Update match info from game data
}

void GUIManager::UpdateAuctionData() {
    // TODO: Update auction data from game data
}

std::string GUIManager::FormatCurrency(int amount) {
    // TODO: Implement currency formatting
    return "₹" + std::to_string(amount);
}

std::string GUIManager::FormatPercentage(float value) {
    return std::to_string(static_cast<int>(value * 100)) + "%";
}

std::string GUIManager::FormatTime(int seconds) {
    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    return std::to_string(minutes) + ":" + (remainingSeconds < 10 ? "0" : "") + std::to_string(remainingSeconds);
}

ImVec4 GUIManager::GetColorForRating(int rating) {
    if (rating >= 90) return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
    if (rating >= 80) return ImVec4(0.0f, 0.8f, 0.0f, 1.0f); // Light Green
    if (rating >= 70) return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    if (rating >= 60) return ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // Orange
    return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
}

ImVec4 GUIManager::GetColorForForm(int form) {
    if (form >= 80) return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
    if (form >= 60) return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
}

ImVec4 GUIManager::GetColorForMorale(int morale) {
    if (morale >= 80) return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
    if (morale >= 60) return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
}

void GUIManager::RenderNotifications() {
    // TODO: Implement notification rendering
}

void GUIManager::RenderConfirmDialog() {
    if (showConfirmDialog) {
        ImGui::OpenPopup("Confirm");
        if (ImGui::BeginPopupModal("Confirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", confirmTitle.c_str());
            ImGui::Text("%s", confirmMessage.c_str());
            ImGui::Separator();
            
            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                if (confirmCallback) {
                    confirmCallback(true);
                }
                showConfirmDialog = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) {
                if (confirmCallback) {
                    confirmCallback(false);
                }
                showConfirmDialog = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
} 