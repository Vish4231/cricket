#include <SDL2/SDL.h>
#include <random>
#include "CommentaryManager.h"
#include <iostream>

CommentaryManager::CommentaryManager()
    : audioEnabled(true)
    , audioVolume(80)
    , currentTone(CommentaryTone::EXCITED)
    , currentLanguage("English")
    , useNicknames(true)
    , useCatchphrases(true)
    , lastCommentaryTime(0)
    , commentaryInterval(1000)
{
    // Initialize default commentator
    currentCommentator.name = "Ravi Shastri";
    currentCommentator.style = "Exciting";
    currentCommentator.preferredTone = CommentaryTone::EXCITED;
    currentCommentator.catchphrases = {"Dhoni finishes off in style!", "What a shot!", "In the air... and SIX!"};
    currentCommentator.playerNicknames = {"Kohli", "Hitman", "Captain Cool"};
    
    // Initialize commentary templates
    // InitializeTemplates(); // Commented out - not implemented
}

CommentaryManager::~CommentaryManager() {
    Shutdown();
}

bool CommentaryManager::Initialize() {
    // Initialize commentary system
    // InitializeTemplates(); // Commented out - not implemented
    
    // Set up default commentator
    currentCommentator.name = "Harsha Bhogle";
    currentCommentator.style = "Analytical";
    currentCommentator.preferredTone = CommentaryTone::ANALYTICAL;
    currentCommentator.catchphrases = {
        "That's a brilliant shot!",
        "What a delivery!",
        "Excellent fielding!",
        "The pressure is mounting!"
    };
    
    // Initialize audio system
    // TODO: Initialize audio system
    
    std::cout << "Commentary Manager initialized successfully" << std::endl;
    return true;
}

void CommentaryManager::Shutdown() {
    // Unload all audio clips
    for (auto& clip : audioClips) {
        if (clip.second.chunk) {
            Mix_FreeChunk(clip.second.chunk);
        }
    }
    audioClips.clear();
    
    Mix_CloseAudio();
}

void CommentaryManager::GenerateBallCommentary(const std::string& striker, const std::string& bowler, 
                                              int runs, bool isWicket, const std::string& wicketType) {
    std::string commentary = GenerateBallText(striker, bowler, runs, isWicket, wicketType);
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::BALL_BY_BALL;
    line.tone = DetermineTone(isWicket ? CommentaryEvent::WICKET_FALLEN : CommentaryEvent::RUNS_SCORED);
    line.event = isWicket ? CommentaryEvent::WICKET_FALLEN : CommentaryEvent::RUNS_SCORED;
    line.playerName = striker;
    line.timestamp = SDL_GetTicks();
    line.isImportant = isWicket || runs >= 4;
    
    AddCommentaryLine(line);
    
    // Play audio if enabled
    if (audioEnabled) {
        CommentaryEvent event = isWicket ? CommentaryEvent::WICKET_FALLEN : CommentaryEvent::RUNS_SCORED;
        PlayCommentaryAudio(event, striker);
    }
}

void CommentaryManager::GenerateBoundaryCommentary(const std::string& batsman, int runs) {
    std::string commentary = GenerateBoundaryText(batsman, runs);
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::HIGHLIGHT;
    line.tone = CommentaryTone::EXCITED;
    line.event = CommentaryEvent::BOUNDARY_HIT;
    line.playerName = batsman;
    line.timestamp = SDL_GetTicks();
    line.isImportant = true;
    
    AddCommentaryLine(line);
    
    if (audioEnabled) {
        PlayCommentaryAudio(CommentaryEvent::BOUNDARY_HIT, batsman);
    }
}

void CommentaryManager::GenerateWicketCommentary(const std::string& batsman, const std::string& bowler, 
                                                const std::string& wicketType) {
    std::string commentary = GenerateWicketText(batsman, bowler, wicketType);
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::HIGHLIGHT;
    line.tone = CommentaryTone::DRAMATIC;
    line.event = CommentaryEvent::WICKET_FALLEN;
    line.playerName = batsman;
    line.timestamp = SDL_GetTicks();
    line.isImportant = true;
    
    AddCommentaryLine(line);
    
    if (audioEnabled) {
        PlayCommentaryAudio(CommentaryEvent::WICKET_FALLEN, batsman);
    }
}

void CommentaryManager::GenerateMilestoneCommentary(const std::string& player, const std::string& milestone) {
    std::string commentary = GenerateMilestoneText(player, milestone);
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::HIGHLIGHT;
    line.tone = CommentaryTone::EXCITED;
    line.event = CommentaryEvent::MILESTONE_REACHED;
    line.playerName = player;
    line.timestamp = SDL_GetTicks();
    line.isImportant = true;
    
    AddCommentaryLine(line);
    
    if (audioEnabled) {
        PlayCommentaryAudio(CommentaryEvent::MILESTONE_REACHED, player);
    }
}

void CommentaryManager::GenerateMatchEndCommentary(const std::string& winner, const std::string& result) {
    std::string commentary = GenerateMatchEndText(winner, result);
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::HIGHLIGHT;
    line.tone = CommentaryTone::DRAMATIC;
    line.event = CommentaryEvent::MATCH_END;
    line.playerName = winner;
    line.timestamp = SDL_GetTicks();
    line.isImportant = true;
    
    AddCommentaryLine(line);
    
    if (audioEnabled) {
        PlayCommentaryAudio(CommentaryEvent::MATCH_END, winner);
    }
}

void CommentaryManager::SetCommentator(const std::string& name) {
    // TODO: Implement commentator selection
    currentCommentator.name = name;
}

void CommentaryManager::AddCommentator(const CommentaryProfile& profile) {
    commentators[profile.name] = profile;
}

void CommentaryManager::AddCommentaryLine(const CommentaryLine& line) {
    commentaryHistory.push_back(line);
    
    // Keep only last 1000 lines
    if (commentaryHistory.size() > 1000) {
        commentaryHistory.erase(commentaryHistory.begin());
    }
    
    if (commentaryCallback) {
        commentaryCallback(line);
    }
}

void CommentaryManager::ClearCommentary() {
    commentaryHistory.clear();
    commentaryQueue.clear();
}

void CommentaryManager::SaveCommentary(const std::string& filename) {
    // TODO: Implement commentary saving
}

void CommentaryManager::LoadCommentary(const std::string& filename) {
    // TODO: Implement commentary loading
}

void CommentaryManager::PlayCommentaryAudio(CommentaryEvent event, const std::string& playerName) {
    if (!audioEnabled) return;
    
    // Look for appropriate audio clip
    std::string clipKey = std::to_string(static_cast<int>(event)) + "_" + playerName;
    auto it = audioClips.find(clipKey);
    
    if (it != audioClips.end() && it->second.isLoaded) {
        PlayAudioClip(it->second);
    } else {
        // Try generic event clip
        std::string genericKey = std::to_string(static_cast<int>(event));
        auto genericIt = audioClips.find(genericKey);
        if (genericIt != audioClips.end() && genericIt->second.isLoaded) {
            PlayAudioClip(genericIt->second);
        }
    }
}

void CommentaryManager::SetAudioVolume(int volume) {
    audioVolume = std::max(0, std::min(128, volume));
    Mix_Volume(-1, audioVolume);
}

void CommentaryManager::SetCommentaryStyle(CommentaryTone tone) {
    currentTone = tone;
}

void CommentaryManager::SetCommentaryLanguage(const std::string& language) {
    currentLanguage = language;
}

void CommentaryManager::UpdateCommentary() {
    // TODO: Update commentary system
}

void CommentaryManager::Update(float deltaTime) {
    // Update commentary timing
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastCommentaryTime > commentaryInterval) {
        ProcessCommentaryQueue();
        lastCommentaryTime = currentTime;
    }
}

void CommentaryManager::ProcessCommentaryQueue() {
    while (!commentaryQueue.empty()) {
        CommentaryLine line = commentaryQueue.front();
        commentaryQueue.erase(commentaryQueue.begin());
        
        AddCommentaryLine(line);
    }
}

void CommentaryManager::AddStatisticalCommentary(const std::string& stat, const std::string& value) {
    std::string commentary = stat + ": " + value;
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::STATISTICAL;
    line.tone = CommentaryTone::ANALYTICAL;
    line.event = CommentaryEvent::PLAYER_ACHIEVEMENT;
    line.timestamp = SDL_GetTicks();
    line.isImportant = false;
    
    AddCommentaryLine(line);
}

void CommentaryManager::AddPlayerAnalysis(const std::string& player, const std::string& analysis) {
    std::string commentary = player + ": " + analysis;
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::PLAYER_PROFILE;
    line.tone = CommentaryTone::ANALYTICAL;
    line.event = CommentaryEvent::PLAYER_ACHIEVEMENT;
    line.playerName = player;
    line.timestamp = SDL_GetTicks();
    line.isImportant = false;
    
    AddCommentaryLine(line);
}

void CommentaryManager::AddTeamAnalysis(const std::string& team, const std::string& analysis) {
    std::string commentary = team + ": " + analysis;
    
    CommentaryLine line;
    line.text = commentary;
    line.type = CommentaryType::STATISTICAL;
    line.tone = CommentaryTone::ANALYTICAL;
    line.event = CommentaryEvent::PLAYER_ACHIEVEMENT;
    line.teamName = team;
    line.timestamp = SDL_GetTicks();
    line.isImportant = false;
    
    AddCommentaryLine(line);
}

void CommentaryManager::AddCrowdReaction(const std::string& reaction) {
    CommentaryLine line;
    line.text = "Crowd: " + reaction;
    line.type = CommentaryType::ATMOSPHERIC;
    line.tone = CommentaryTone::EXCITED;
    line.event = CommentaryEvent::BALL_DELIVERED;
    line.timestamp = SDL_GetTicks();
    line.isImportant = false;
    
    AddCommentaryLine(line);
}

void CommentaryManager::AddWeatherCommentary(const std::string& weather) {
    CommentaryLine line;
    line.text = "Weather: " + weather;
    line.type = CommentaryType::ATMOSPHERIC;
    line.tone = CommentaryTone::CALM;
    line.event = CommentaryEvent::BALL_DELIVERED;
    line.timestamp = SDL_GetTicks();
    line.isImportant = false;
    
    AddCommentaryLine(line);
}

void CommentaryManager::AddPitchCommentary(const std::string& pitchCondition) {
    CommentaryLine line;
    line.text = "Pitch: " + pitchCondition;
    line.type = CommentaryType::ATMOSPHERIC;
    line.tone = CommentaryTone::ANALYTICAL;
    line.event = CommentaryEvent::BALL_DELIVERED;
    line.timestamp = SDL_GetTicks();
    line.isImportant = false;
    
    AddCommentaryLine(line);
}

void CommentaryManager::AddVenueCommentary(const std::string& venueInfo) {
    CommentaryLine line;
    line.text = "Venue: " + venueInfo;
    line.type = CommentaryType::ATMOSPHERIC;
    line.tone = CommentaryTone::CALM;
    line.event = CommentaryEvent::BALL_DELIVERED;
    line.timestamp = SDL_GetTicks();
    line.isImportant = false;
    
    AddCommentaryLine(line);
}

void CommentaryManager::SetCommentaryCallback(std::function<void(const CommentaryLine&)> callback) {
    commentaryCallback = callback;
}

void CommentaryManager::SetAudioCallback(std::function<void(const std::string&)> callback) {
    audioCallback = callback;
}

void CommentaryManager::ExportCommentary(const std::string& filename) const {
    // TODO: Implement commentary export
}

void CommentaryManager::ImportCommentary(const std::string& filename) {
    // TODO: Implement commentary import
}

// Private helper methods
std::string CommentaryManager::GenerateBallText(const std::string& striker, const std::string& bowler, 
                                               int runs, bool isWicket, const std::string& wicketType) {
    if (isWicket) {
        std::string template_ = GetRandomTemplate("wicket");
        std::map<std::string, std::string> variables = {
            {"striker", GetPlayerNickname(striker)},
            {"bowler", GetPlayerNickname(bowler)},
            {"wicket_type", wicketType}
        };
        return SubstituteVariables(template_, variables);
    } else if (runs == 0) {
        return GetRandomTemplate("dot_ball");
    } else if (runs == 1) {
        return GetRandomTemplate("single");
    } else if (runs == 4) {
        return GetRandomTemplate("four");
    } else if (runs == 6) {
        return GetRandomTemplate("six");
    } else {
        return striker + " scores " + std::to_string(runs) + " runs";
    }
}

std::string CommentaryManager::GenerateBoundaryText(const std::string& batsman, int runs) {
    if (runs == 4) {
        return "FOUR! " + GetPlayerNickname(batsman) + " with a beautiful shot!";
    } else if (runs == 6) {
        return "SIX! " + GetPlayerNickname(batsman) + " with a massive hit!";
    }
    return batsman + " hits a boundary!";
}

std::string CommentaryManager::GenerateWicketText(const std::string& batsman, const std::string& bowler, 
                                                 const std::string& wicketType) {
    return "WICKET! " + GetPlayerNickname(batsman) + " " + wicketType + " by " + GetPlayerNickname(bowler) + "!";
}

std::string CommentaryManager::GenerateMilestoneText(const std::string& player, const std::string& milestone) {
    return GetPlayerNickname(player) + " reaches " + milestone + "! " + GetRandomCatchphrase();
}

std::string CommentaryManager::GenerateMatchEndText(const std::string& winner, const std::string& result) {
    return winner + " wins! " + result + ". " + GetRandomCatchphrase();
}

bool CommentaryManager::LoadAudioClip(const std::string& filename, CommentaryEvent event) {
    // TODO: Implement audio clip loading
    return false;
}

void CommentaryManager::UnloadAudioClip(const std::string& filename) {
    auto it = audioClips.find(filename);
    if (it != audioClips.end()) {
        if (it->second.chunk) {
            Mix_FreeChunk(it->second.chunk);
        }
        audioClips.erase(it);
    }
}

void CommentaryManager::PlayAudioClip(const AudioClip& clip) {
    if (clip.chunk && clip.isLoaded) {
        Mix_PlayChannel(-1, clip.chunk, 0);
        
        if (audioCallback) {
            audioCallback(clip.filename);
        }
    }
}

std::string CommentaryManager::GetRandomTemplate(const std::string& category) {
    // TODO: Implement template system
    if (category == "wicket") {
        return "{striker} is {wicket_type} by {bowler}!";
    } else if (category == "four") {
        return "FOUR! Beautiful shot by {striker}!";
    } else if (category == "six") {
        return "SIX! Massive hit by {striker}!";
    } else if (category == "single") {
        return "Single taken by {striker}.";
    } else if (category == "dot_ball") {
        return "Dot ball! Good bowling by {bowler}.";
    }
    return "Ball delivered.";
}

std::string CommentaryManager::SubstituteVariables(const std::string& template_, 
                                                  const std::map<std::string, std::string>& variables) {
    std::string result = template_;
    for (const auto& var : variables) {
        std::string placeholder = "{" + var.first + "}";
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos = result.find(placeholder);
        }
    }
    return result;
}

std::string CommentaryManager::GetPlayerNickname(const std::string& playerName) {
    if (!useNicknames) return playerName;
    
    auto it = playerNicknames.find(playerName);
    return (it != playerNicknames.end()) ? it->second : playerName;
}

std::string CommentaryManager::GetRandomCatchphrase() {
    if (!useCatchphrases || currentCommentator.catchphrases.empty()) {
        return "";
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, currentCommentator.catchphrases.size() - 1);
    
    return currentCommentator.catchphrases[dis(gen)];
}

CommentaryTone CommentaryManager::DetermineTone(CommentaryEvent event) {
    switch (event) {
        case CommentaryEvent::WICKET_FALLEN:
        case CommentaryEvent::BOUNDARY_HIT:
        case CommentaryEvent::MILESTONE_REACHED:
            return CommentaryTone::EXCITED;
        case CommentaryEvent::MATCH_END:
            return CommentaryTone::DRAMATIC;
        case CommentaryEvent::PLAYER_ACHIEVEMENT:
            return CommentaryTone::ANALYTICAL;
        default:
            return currentTone;
    }
}

bool CommentaryManager::IsImportantEvent(CommentaryEvent event) {
    return event == CommentaryEvent::WICKET_FALLEN || 
           event == CommentaryEvent::BOUNDARY_HIT || 
           event == CommentaryEvent::MILESTONE_REACHED ||
           event == CommentaryEvent::MATCH_END;
}

// void CommentaryManager::InitializeTemplates() {
//     // TODO: Initialize commentary templates
//     // This function is not declared in the header, so commenting it out
// }

void CommentaryManager::StartMatch(const std::string& team1, const std::string& team2, const std::string& venue) {
    // TODO: Implement or leave empty if not needed
}