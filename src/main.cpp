#include "Game.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== Cricket Manager Simulation Game ===" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << "Platform: macOS" << std::endl;
        std::cout << "=======================================" << std::endl;
        
        // Create and initialize the game
        Game game;
        
        if (!game.Initialize()) {
            std::cerr << "Failed to initialize Cricket Manager!" << std::endl;
            return -1;
        }
        
        // Run the game
        game.Run();
        
        std::cout << "Cricket Manager exited successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return -1;
    }
} 