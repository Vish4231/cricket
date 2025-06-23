#include "Game.h"
#include "Team.h"
#include "Player.h"
#include "Venue.h"
#include <iostream>
#include <memory>

// Demo function to showcase 3D visualization
void demo3DVisualization() {
    std::cout << "=== Cricket Manager 3D Visualization Demo ===" << std::endl;
    
    // Create the game instance
    auto game = std::make_unique<Game>();
    
    // Initialize the game
    if (!game->initialize("Cricket Manager 3D Demo", 1280, 720)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return;
    }
    
    // Create sample teams
    Team team1("Mumbai Indians");
    Team team2("Chennai Super Kings");
    
    // Add players to team 1
    team1.addPlayer(Player("Rohit Sharma", "Batsman", 34, "India", 85, 80, 75, 70, 65));
    team1.addPlayer(Player("Jasprit Bumrah", "Bowler", 28, "India", 45, 90, 85, 80, 75));
    team1.addPlayer(Player("Kieron Pollard", "All-rounder", 35, "West Indies", 80, 75, 70, 85, 80));
    team1.addPlayer(Player("Suryakumar Yadav", "Batsman", 32, "India", 82, 70, 65, 75, 70));
    team1.addPlayer(Player("Trent Boult", "Bowler", 33, "New Zealand", 40, 88, 85, 75, 70));
    
    // Add players to team 2
    team2.addPlayer(Player("MS Dhoni", "Wicket-keeper", 41, "India", 75, 85, 80, 90, 85));
    team2.addPlayer(Player("Ravindra Jadeja", "All-rounder", 33, "India", 70, 80, 85, 90, 85));
    team2.addPlayer(Player("Virat Kohli", "Batsman", 34, "India", 90, 75, 70, 80, 75));
    team2.addPlayer(Player("Dwayne Bravo", "All-rounder", 39, "West Indies", 75, 85, 80, 85, 80));
    team2.addPlayer(Player("Deepak Chahar", "Bowler", 30, "India", 45, 82, 80, 75, 70));
    
    // Create sample venue
    Venue venue("Wankhede Stadium", "Mumbai", 33108, "India", 80, 70, 60, 75);
    
    // Add teams and venue to game
    game->getTeams().push_back(team1);
    game->getTeams().push_back(team2);
    game->getVenues().push_back(venue);
    
    // Start a match
    std::cout << "Starting 3D match visualization..." << std::endl;
    game->startMatch(team1, team2, venue, MatchFormat::T20);
    
    // Demo different camera modes
    auto visualizer = game->getMatchVisualizer();
    if (visualizer) {
        std::cout << "Demoing camera modes..." << std::endl;
        
        // Broadcast view
        visualizer->setCameraMode(CameraMode::BROADCAST);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Bowler view
        visualizer->setCameraMode(CameraMode::BOWLER_VIEW);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Batsman view
        visualizer->setCameraMode(CameraMode::BATSMAN_VIEW);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Drone view
        visualizer->setCameraMode(CameraMode::DRONE_VIEW);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Back to broadcast
        visualizer->setCameraMode(CameraMode::BROADCAST);
        
        // Demo ball physics
        std::cout << "Demoing ball physics..." << std::endl;
        visualizer->setBallPosition(glm::vec3(0, 2, 0));
        visualizer->setBallVelocity(glm::vec3(0, 0, -20));
        visualizer->setBallSpin(5.0f);
        visualizer->setBallSeam(0.3f);
        visualizer->showBallTrail(true);
        
        // Demo particle effects
        std::cout << "Demoing particle effects..." << std::endl;
        visualizer->addParticleEffect(glm::vec3(0, 0, 0), "celebration");
        visualizer->addParticleEffect(glm::vec3(10, 0, 10), "dust");
        
        // Demo screen effects
        std::cout << "Demoing screen effects..." << std::endl;
        visualizer->setSlowMotion(true, 0.3f);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        visualizer->setSlowMotion(false);
        
        // Demo weather effects
        std::cout << "Demoing weather effects..." << std::endl;
        WeatherConditions weather;
        weather.isRaining = true;
        weather.rainIntensity = 0.7f;
        weather.isOvercast = true;
        weather.cloudCover = 0.8f;
        visualizer->updateWeatherConditions(weather);
        
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        // Clear weather
        weather.isRaining = false;
        weather.rainIntensity = 0.0f;
        weather.isOvercast = false;
        weather.cloudCover = 0.0f;
        visualizer->updateWeatherConditions(weather);
    }
    
    // Run the game loop for a few seconds
    std::cout << "Running game loop for 10 seconds..." << std::endl;
    auto startTime = std::chrono::steady_clock::now();
    
    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(10)) {
        game->update(1.0f / 60.0f); // 60 FPS
        game->render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "Demo completed!" << std::endl;
}

// Main function for the demo
int main() {
    try {
        demo3DVisualization();
    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 