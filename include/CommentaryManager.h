#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <SDL2/SDL_mixer.h>

enum class CommentaryType {
    BALL_BY_BALL,
    HIGHLIGHT,
    STATISTICAL,
    ATMOSPHERIC,
    PLAYER_PROFILE
};

enum class CommentaryTone {
    EXCITED,
    CALM,
    ANALYTICAL,
    DRAMATIC,
    HUMOROUS
};

enum class CommentaryEvent {
    BALL_DELIVERED,
    RUNS_SCORED,
    WICKET_FALLEN,
    BOUNDARY_HIT,
    MILESTONE_REACHED,
    MATCH_END,
    PLAYER_ACHIEVEMENT
};

struct CommentaryLine {
    std::string text;
    CommentaryType type;
    CommentaryTone tone;
    CommentaryEvent event;
    std::string playerName;
    std::string teamName;
    int timestamp;
    bool isImportant;
};

struct CommentaryProfile {
    std::string name;
    std::string style;
    CommentaryTone preferredTone;
    std::vector<std::string> catchphrases;
    std::vector<std::string> playerNicknames;
};

struct AudioClip {
    std::string filename;
    Mix_Chunk* chunk;
    CommentaryEvent event;
    std::string playerName;
    bool isLoaded;
};

class CommentaryManager {
public:
    CommentaryManager();
    ~CommentaryManager();
    
    // Initialization
    bool Initialize();
    void Shutdown();
    
    // Commentary generation
    void GenerateBallCommentary(const std::string& striker, const std::string& bowler, 
                               int runs, bool isWicket, const std::string& wicketType);
    void GenerateBoundaryCommentary(const std::string& batsman, int runs);
    void GenerateWicketCommentary(const std::string& batsman, const std::string& bowler, 
                                 const std::string& wicketType);
    void GenerateMilestoneCommentary(const std::string& player, const std::string& milestone);
    void GenerateMatchEndCommentary(const std::string& winner, const std::string& result);
    
    // Commentary profiles
    void SetCommentator(const std::string& name);
    void AddCommentator(const CommentaryProfile& profile);
    const CommentaryProfile& GetCurrentCommentator() const { return currentCommentator; }
    
    // Commentary history
    void AddCommentaryLine(const CommentaryLine& line);
    const std::vector<CommentaryLine>& GetCommentaryHistory() const { return commentaryHistory; }
    void ClearCommentary();
    void SaveCommentary(const std::string& filename);
    void LoadCommentary(const std::string& filename);
    
    // Audio commentary
    void EnableAudioCommentary(bool enable) { audioEnabled = enable; }
    bool IsAudioEnabled() const { return audioEnabled; }
    void PlayCommentaryAudio(CommentaryEvent event, const std::string& playerName = "");
    void SetAudioVolume(int volume);
    int GetAudioVolume() const { return audioVolume; }
    
    // Commentary customization
    void SetCommentaryStyle(CommentaryTone tone);
    void SetCommentaryLanguage(const std::string& language);
    void EnablePlayerNicknames(bool enable) { useNicknames = enable; }
    void EnableCatchphrases(bool enable) { useCatchphrases = enable; }
    
    // Real-time commentary
    void UpdateCommentary();
    void ProcessCommentaryQueue();
    bool HasPendingCommentary() const { return !commentaryQueue.empty(); }
    
    // Update method
    void Update(float deltaTime);
    
    // Statistics and analysis
    void AddStatisticalCommentary(const std::string& stat, const std::string& value);
    void AddPlayerAnalysis(const std::string& player, const std::string& analysis);
    void AddTeamAnalysis(const std::string& team, const std::string& analysis);
    
    // Atmospheric commentary
    void AddCrowdReaction(const std::string& reaction);
    void AddWeatherCommentary(const std::string& weather);
    void AddPitchCommentary(const std::string& pitchCondition);
    void AddVenueCommentary(const std::string& venueInfo);
    
    // Callbacks
    void SetCommentaryCallback(std::function<void(const CommentaryLine&)> callback);
    void SetAudioCallback(std::function<void(const std::string&)> callback);
    
    // Export and import
    void ExportCommentary(const std::string& filename) const;
    void ImportCommentary(const std::string& filename);
    
private:
    // Commentary generation helpers
    std::string GenerateBallText(const std::string& striker, const std::string& bowler, 
                                int runs, bool isWicket, const std::string& wicketType);
    std::string GenerateBoundaryText(const std::string& batsman, int runs);
    std::string GenerateWicketText(const std::string& batsman, const std::string& bowler, 
                                  const std::string& wicketType);
    std::string GenerateMilestoneText(const std::string& player, const std::string& milestone);
    std::string GenerateMatchEndText(const std::string& winner, const std::string& result);
    
    // Audio management
    bool LoadAudioClip(const std::string& filename, CommentaryEvent event);
    void UnloadAudioClip(const std::string& filename);
    void PlayAudioClip(const AudioClip& clip);
    
    // Commentary templates
    std::string GetRandomTemplate(const std::string& category);
    std::string SubstituteVariables(const std::string& tmplStr, 
                                   const std::map<std::string, std::string>& variables);
    
    // Helper methods
    std::string GetPlayerNickname(const std::string& playerName);
    std::string GetRandomCatchphrase();
    CommentaryTone DetermineTone(CommentaryEvent event);
    bool IsImportantEvent(CommentaryEvent event);
    
    // Commentary data
    std::vector<CommentaryLine> commentaryHistory;
    std::vector<CommentaryLine> commentaryQueue;
    std::map<std::string, CommentaryProfile> commentators;
    CommentaryProfile currentCommentator;
    
    // Audio data
    std::map<std::string, AudioClip> audioClips;
    bool audioEnabled;
    int audioVolume;
    
    // Settings
    CommentaryTone currentTone;
    std::string currentLanguage;
    bool useNicknames;
    bool useCatchphrases;
    
    // Callbacks
    std::function<void(const CommentaryLine&)> commentaryCallback;
    std::function<void(const std::string&)> audioCallback;
    
    // Templates and phrases
    std::map<std::string, std::vector<std::string>> commentaryTemplates;
    std::map<std::string, std::string> playerNicknames;
    std::vector<std::string> catchphrases;
    
    // Timing
    int lastCommentaryTime;
    int commentaryInterval;
}; 