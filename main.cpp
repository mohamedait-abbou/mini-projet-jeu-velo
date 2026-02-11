#include "./headers/game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Create game instance
    Game game;
    
    // Initialize the game
    if (!game.initialize()) {
        std::cerr << "Failed to initialize the game!" << std::endl;
        return 1;
    }
      //so if
      
    // Run the game
    game.run();
    
    return 0;
}