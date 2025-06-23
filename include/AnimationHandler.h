#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <glad/glad.h>

// Forward declarations
class Player;
class Team;

enum class AnimationType {
    IDLE,
    WALKING,
    RUNNING,
    BATTING_STANCE,
    BATTING_SHOT,
    BOWLING_RUN_UP,
    BOWLING_DELIVERY,
    FIELDING_CATCH,
    FIELDING_THROW,
    CELEBRATION,
    INJURY,
    BATTING,
    FIELDING,
    DISAPPOINTMENT
};

enum class PlayerState {
    IDLE,
    BATTING,
    BOWLING,
    FIELDING,
    RUNNING,
    CELEBRATING,
    INJURED
};

enum class AnimationState {
    STOPPED,
    PLAYING,
    PAUSED,
    LOOPING
};

struct Bone {
    std::string name;
    int parentIndex;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::mat4 transform;
};

struct AnimationFrame {
    int frameNumber;
    std::map<std::string, glm::mat4> boneTransforms;
    float duration;
};

struct Animation {
    std::string name;
    AnimationType type;
    float duration;
    int frameCount;
    std::vector<AnimationFrame> frames;
    bool isLooping;
    std::vector<std::string> keyframes;
    std::map<std::string, std::function<void()>> events;
};

struct AnimationInstance {
    std::string animationName;
    AnimationState state;
    float currentTime;
    float speed;
    bool reverse;
    std::function<void()> onComplete;
};

struct Model {
    std::string name;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<Bone> bones;
    std::map<std::string, Animation> animations;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture;
    bool isLoaded;
};

struct PlayerModel {
    std::string playerName;
    Model* model;
    PlayerState currentState;
    AnimationType currentAnimation;
    float animationTime;
    int currentFrame;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool isVisible;
    bool isAnimating;
};

class AnimationHandler {
public:
    AnimationHandler();
    ~AnimationHandler();
    
    // Initialization
    bool Initialize();
    void Shutdown();
    
    // Model management
    bool LoadModel(const std::string& filename, const std::string& modelName);
    void UnloadModel(const std::string& modelName);
    Model* GetModel(const std::string& modelName);
    bool IsModelLoaded(const std::string& modelName) const;
    
    // Player model management
    void CreatePlayerModel(const std::string& playerName, const std::string& modelName);
    void RemovePlayerModel(const std::string& playerName);
    PlayerModel* GetPlayerModel(const std::string& playerName);
    void SetPlayerModel(const std::string& playerName, const std::string& modelName);
    
    // Animation control
    void PlayAnimation(const std::string& playerName, AnimationType type);
    void StopAnimation(const std::string& playerName);
    void PauseAnimation(const std::string& playerName);
    void ResumeAnimation(const std::string& playerName);
    bool IsAnimationPlaying(const std::string& playerName) const;
    
    // Animation blending
    void BlendToAnimation(const std::string& playerName, AnimationType type, float blendTime);
    void SetAnimationSpeed(const std::string& playerName, float speed);
    void SetAnimationLooping(const std::string& playerName, bool loop);
    
    // Player positioning
    void SetPlayerPosition(const std::string& playerName, const glm::vec3& position);
    void SetPlayerRotation(const std::string& playerName, const glm::vec3& rotation);
    void SetPlayerScale(const std::string& playerName, const glm::vec3& scale);
    glm::vec3 GetPlayerPosition(const std::string& playerName) const;
    glm::vec3 GetPlayerRotation(const std::string& playerName) const;
    
    // Player state management
    void SetPlayerState(const std::string& playerName, PlayerState state);
    PlayerState GetPlayerState(const std::string& playerName) const;
    void UpdatePlayerState(const std::string& playerName, PlayerState newState);
    
    // Rendering
    void RenderPlayer(const std::string& playerName);
    void RenderAllPlayers();
    void SetPlayerVisibility(const std::string& playerName, bool visible);
    bool IsPlayerVisible(const std::string& playerName) const;
    
    // Camera control
    void SetCameraPosition(const glm::vec3& position);
    void SetCameraTarget(const glm::vec3& target);
    void SetCameraUp(const glm::vec3& up);
    void UpdateCamera();
    
    // Lighting
    void SetAmbientLight(const glm::vec3& color, float intensity);
    void SetDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
    void SetPointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range);
    
    // Shader management
    bool LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& shaderName);
    void UseShader(const std::string& shaderName);
    void SetShaderUniform(const std::string& uniformName, const glm::mat4& value);
    void SetShaderUniform(const std::string& uniformName, const glm::vec3& value);
    void SetShaderUniform(const std::string& uniformName, float value);
    void SetShaderUniform(const std::string& uniformName, int value);
    
    // Animation events
    void SetAnimationEventCallback(const std::string& playerName, std::function<void(AnimationType, int)> callback);
    void TriggerAnimationEvent(const std::string& playerName, AnimationType type, int frame);
    
    // Performance optimization
    void EnableFrustumCulling(bool enable) { frustumCulling = enable; }
    void EnableLOD(bool enable) { levelOfDetail = enable; }
    void SetMaxVisiblePlayers(int max) { maxVisiblePlayers = max; }
    
    // Update and timing
    void Update(float deltaTime);
    void UpdateAnimation(const std::string& playerName, float deltaTime);
    void UpdateAllAnimations(float deltaTime);
    
    // Animation management
    void addAnimation(const Animation& animation);
    void removeAnimation(const std::string& name);
    Animation* getAnimation(const std::string& name);
    
    // Instance management
    std::string playAnimation(const std::string& animationName, bool loop = false, float speed = 1.0f);
    void stopAnimation(const std::string& instanceId);
    void pauseAnimation(const std::string& instanceId);
    void resumeAnimation(const std::string& instanceId);
    
    // Getters
    const std::vector<AnimationInstance>& getActiveAnimations() const { return activeAnimations; }
    bool isAnimationPlaying(const std::string& instanceId) const;
    
    // Callbacks
    void setAnimationEventCallback(const std::string& eventName, std::function<void()> callback);
    
private:
    std::map<std::string, Animation> animations;
    std::vector<AnimationInstance> activeAnimations;
    std::map<std::string, std::function<void()>> eventCallbacks;
    int nextInstanceId;
    
    // Model loading helpers
    GLuint defaultVAO;
    GLuint defaultVBO;
    bool isInitialized;
    
    // Performance optimization
    bool frustumCulling;
    bool levelOfDetail;
    int maxVisiblePlayers;
    
    // Models and players
    std::map<std::string, Model> models;
    std::map<std::string, PlayerModel> playerModels;
    
    // Camera and lighting
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    glm::vec3 cameraUp;
    glm::vec3 ambientLight;
    float ambientIntensity;
    glm::vec3 directionalLightDir;
    glm::vec3 directionalLightColor;
    float directionalIntensity;
    
    // Shaders
    std::map<std::string, GLuint> shaders;
    std::string currentShader;
    
    void updateAnimationInstance(AnimationInstance& instance, float deltaTime);
    void triggerAnimationEvent(const std::string& eventName);
    std::string generateInstanceId();
}; 