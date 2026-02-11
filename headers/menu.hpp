#ifndef MENU_HPP
#define MENU_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

class Game;

enum class MenuOption {
    START,
    ABOUT,
    EXIT
};

class Menu {
public:
    Menu(Game* game);
    ~Menu();
    
    void handleEvents(SDL_Event& event);
    void update();
    void render();
    void playMenuMusic();
    
    void showAboutScreen();
    void hideAboutScreen();
    
private:
    Game* game;
    SDL_Texture* backgroundTexture;
    
    // Options du menu
    std::vector<std::string> optionTexts;
    std::vector<SDL_Texture*> optionTextures;
    std::vector<SDL_Rect> optionRects;
    
    int selectedOption;
    bool showingAbout;
    bool isMusicPlaying; // Suivre l'état de la musique
    
    // Variables pour les effets visuels
    int highlightPulse;
    int pulseDirection;
    
    // Texte à propos
    SDL_Texture* aboutTexture;
    
    int initialMusicVolume; // Stocker le volume initial
    
    void loadBackgroundTexture();
    void createOptionTextures();
    void renderOption(int index);
    void renderAboutScreen();
};

#endif // MENU_HPP