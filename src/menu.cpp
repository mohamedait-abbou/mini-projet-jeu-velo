#include "../headers/menu.hpp"
#include "../headers/game.hpp"
#include <iostream>

Menu::Menu(Game* game) : 
    game(game), 
    backgroundTexture(nullptr),
    selectedOption(0),
    showingAbout(false),
    highlightPulse(0),
    pulseDirection(1),
    aboutTexture(nullptr),
    isMusicPlaying(false),
    initialMusicVolume(MIX_MAX_VOLUME) // Stocker le volume initial (128 par défaut)
{
    // Options du menu
    optionTexts = {
        "Commencer le jeu",
        "A propos",
        "Quitter"
    };
    
    // Charger l'arrière-plan
    loadBackgroundTexture();
    
    // Créer les textures pour les options
    createOptionTextures();
    
    // Stocker le volume initial actuel
    initialMusicVolume = Mix_VolumeMusic(-1); // Récupérer le volume actuel
}

Menu::~Menu() {
    // Nettoyer les textures des options
    for (auto& texture : optionTextures) {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
    
    // Nettoyer la texture d'arrière-plan
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    
    // Nettoyer la texture À propos
    if (aboutTexture) {
        SDL_DestroyTexture(aboutTexture);
        aboutTexture = nullptr;
    }
}

void Menu::handleEvents(SDL_Event& event) {
    if (showingAbout) {
        // Si on affiche l'écran À propos, n'importe quelle touche le ferme, sauf la touche Print Screen
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym != SDLK_PRINTSCREEN) {
            hideAboutScreen();
        }
        return;
    }
    
    
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                selectedOption = (selectedOption - 1 + optionTexts.size()) % optionTexts.size();
                break;
            case SDLK_DOWN:
                selectedOption = (selectedOption + 1) % optionTexts.size();
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                // Arrêter la musique avant de changer d'état
                Mix_HaltMusic();
                isMusicPlaying = false;
                // Action en fonction de l'option sélectionnée
                switch (static_cast<MenuOption>(selectedOption)) {
                    case MenuOption::START:
                        game->changeState(GameState::PLAYING);
                        break;
                    case MenuOption::ABOUT:
                        showAboutScreen();
                        break;
                    case MenuOption::EXIT:
                        game->changeState(GameState::EXIT);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void Menu::update() {
    // Mise à jour de l'effet de pulsation pour l'option sélectionnée
    highlightPulse += pulseDirection;
    
    if (highlightPulse >= 30) {
        pulseDirection = -1;
    } else if (highlightPulse <= 0) {
        pulseDirection = 1;
    }
}

void Menu::render() {
    if (showingAbout) {
        renderAboutScreen();
        return;
    }
    
    // Jouer la musique du menu principal seulement si elle n'est pas déjà en cours
    if (!isMusicPlaying) {
        playMenuMusic();
        isMusicPlaying = true;
    }
    
    // Rendre l'arrière-plan
    if (backgroundTexture) {
        SDL_RenderCopy(game->getRenderer(), backgroundTexture, NULL, NULL);
    } else {
        // Arrière-plan de secours si la texture n'est pas chargée
        SDL_SetRenderDrawColor(game->getRenderer(), 50, 50, 80, 255);
        SDL_RenderClear(game->getRenderer());
    }
    
    // Rendre les options
    for (size_t i = 0; i < optionTextures.size(); i++) {
        renderOption(i);
    }
}

void Menu::playMenuMusic() {
    game->playMusic(game->getMenuMusic()); // Utiliser le getter
}

void Menu::loadBackgroundTexture() {
    SDL_Surface* surface = IMG_Load("assets/menubackg.png");
    
    if (!surface) {
        std::cerr << "Failed to load menu background! Error: " << IMG_GetError() << std::endl;
        
        // Créer une surface de secours avec un dégradé
        surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);
        
        // Créer un dégradé de couleurs
        for (int y = 0; y < WINDOW_HEIGHT; y++) {
            // Calculer les composantes de couleur en fonction de la position y
            Uint8 r = 50 + static_cast<Uint8>((y * 50) / WINDOW_HEIGHT);
            Uint8 g = 50;
            Uint8 b = 80 + static_cast<Uint8>((y * 100) / WINDOW_HEIGHT);
            
            SDL_Rect lineRect = {0, y, WINDOW_WIDTH, 1};
            SDL_FillRect(surface, &lineRect, SDL_MapRGB(surface->format, r, g, b));
        }
    }
    
    backgroundTexture = SDL_CreateTextureFromSurface(game->getRenderer(), surface);
    SDL_FreeSurface(surface);
}

void Menu::createOptionTextures() {
    // Libérer les textures existantes
    for (auto& texture : optionTextures) {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
    optionTextures.clear();
    optionRects.clear();
    
    TTF_Font* font = game->getFont();
    if (!font) return;
    
    // Créer une texture pour chaque option
    for (size_t i = 0; i < optionTexts.size(); i++) {
        SDL_Color textColor = {255, 255, 255, 255}; // Blanc par défaut
        SDL_Surface* surface = TTF_RenderText_Solid(font, optionTexts[i].c_str(), textColor);
        
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(game->getRenderer(), surface);
            optionTextures.push_back(texture);
            
            // Calculer la position de l'option (centre horizontalement et espace verticalement)
            // Moved lower on screen as requested
            SDL_Rect rect = {
                (WINDOW_WIDTH - surface->w) / 2,
                WINDOW_HEIGHT / 2 + static_cast<int>(i * 70) + 50, // Moved options down by 100 pixels
                surface->w,
                surface->h
            };
            optionRects.push_back(rect);
            
            SDL_FreeSurface(surface);
        }
    }

    // Add a title at the top of the menu
    SDL_Color titleColor = {255, 215, 0, 255}; // Gold color
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "   ", titleColor);
    
    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(game->getRenderer(), titleSurface);
        optionTextures.push_back(titleTexture);
        
        // Position the title at the top center
        SDL_Rect titleRect = {
            (WINDOW_WIDTH - titleSurface->w) / 2,
            WINDOW_HEIGHT / 4, // Upper part of the screen
            titleSurface->w,
            titleSurface->h
        };
        optionRects.push_back(titleRect);
        
        SDL_FreeSurface(titleSurface);
    }
}

void Menu::renderOption(int index) {
    if (index < 0 || index >= static_cast<int>(optionTextures.size())) return;
    if (!optionTextures[index]) return;
    
    SDL_Rect destRect = optionRects[index];
    
    // If it's the title (last element), just render it without effects
    if (index >= static_cast<int>(optionTexts.size())) {
        SDL_RenderCopy(game->getRenderer(), optionTextures[index], NULL, &destRect);
        return;
    }
    
    // Si c'est l'option sélectionnée, ajouter des effets
    if (index == selectedOption) {
        // Effet de pulsation
        int pulse = highlightPulse / 3;
        destRect.x -= pulse;
        destRect.y -= pulse / 2;
        destRect.w += pulse * 2;
        destRect.h += pulse;
        
        // Dessiner un contour
        SDL_SetRenderDrawColor(game->getRenderer(), 255, 215, 0, 255); // Or
        SDL_Rect outlineRect = {
            destRect.x - 5,
            destRect.y - 5,
            destRect.w + 10,
            destRect.h + 10
        };
        SDL_RenderDrawRect(game->getRenderer(), &outlineRect);
    }
    
    SDL_RenderCopy(game->getRenderer(), optionTextures[index], NULL, &destRect);
}

void Menu::showAboutScreen() {
    showingAbout = true;
    // S'assurer que la musique est en cours avant de réduire le volume
    if (!isMusicPlaying) {
        playMenuMusic();
        isMusicPlaying = true;
    }
    // Réduire le volume à 50% au lieu de 20% pour s'assurer qu'il est audible
    Mix_VolumeMusic(initialMusicVolume / 2); // 50% du volume initial
    
    // Créer la texture de l'écran À propos si elle n'existe pas
    if (!aboutTexture) {
        TTF_Font* font = game->getSmallFont();
        if (!font) return;
        
        std::string aboutText = 
            "Course de Velo\n\n"
            "Un jeu developpe avec C++ et SDL2\n\n"
            "Instructions:\n"
            "- Utilisez les fleches gauche/droite pour changer de voie\n"
            "- Utilisez les fleches Haut/Bas ou W/S: Augmenter/Diminuer la vitesse\n"
            "- Evitez les obstacles pour atteindre la fin de la course\n"
            "- Obtenez le meilleur score possible!\n\n"
            "Appuyez sur n'importe quelle touche pour revenir au menu";

        // Rendu du texte avec support des sauts de ligne
        int lineHeight = TTF_FontLineSkip(font);
        int y = 0;
        
        SDL_Surface* combinedSurface = SDL_CreateRGBSurface(0, WINDOW_WIDTH - 100, lineHeight * 15, 32, 0, 0, 0, 0);
        SDL_SetColorKey(combinedSurface, SDL_TRUE, SDL_MapRGB(combinedSurface->format, 0, 0, 0));
        
        size_t start = 0, end = 0;
        while ((end = aboutText.find('\n', start)) != std::string::npos) {
            std::string line = aboutText.substr(start, end - start);
            if (!line.empty()) {
                SDL_Color textColor = {255, 255, 255, 255};
                SDL_Surface* lineSurface = TTF_RenderText_Blended(font, line.c_str(), textColor);
                if (lineSurface) {
                    SDL_Rect destRect = {
                        (combinedSurface->w - lineSurface->w) / 2,
                        y,
                        lineSurface->w,
                        lineSurface->h
                    };
                    SDL_BlitSurface(lineSurface, NULL, combinedSurface, &destRect);
                    y += lineHeight;
                    SDL_FreeSurface(lineSurface);
                }
            } else {
                y += lineHeight; // Ligne vide
            }
            start = end + 1;
        }
        
        // Dernier segment
        std::string line = aboutText.substr(start);
        if (!line.empty()) {
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Surface* lineSurface = TTF_RenderText_Blended(font, line.c_str(), textColor);
            if (lineSurface) {
                SDL_Rect destRect = {
                    (combinedSurface->w - lineSurface->w) / 2,
                    y,
                    lineSurface->w,
                    lineSurface->h
                };
                SDL_BlitSurface(lineSurface, NULL, combinedSurface, &destRect);
                SDL_FreeSurface(lineSurface);
            }
        }
        
        aboutTexture = SDL_CreateTextureFromSurface(game->getRenderer(), combinedSurface);
        SDL_FreeSurface(combinedSurface);
    }
}

void Menu::hideAboutScreen() {
    showingAbout = false;
    // Restaurer le volume initial
    Mix_VolumeMusic(initialMusicVolume);
}

void Menu::renderAboutScreen() {
    // Rendre l'arrière-plan
    if (backgroundTexture) {
        SDL_RenderCopy(game->getRenderer(), backgroundTexture, NULL, NULL);
    } else {
        // Arrière-plan de secours
        SDL_SetRenderDrawColor(game->getRenderer(), 50, 50, 80, 255);
        SDL_RenderClear(game->getRenderer());
    }
    
    // Rendre un fond semi-transparent pour le texte
    SDL_SetRenderDrawColor(game->getRenderer(), 0, 0, 0, 180);
    SDL_SetRenderDrawBlendMode(game->getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_Rect panel = {50, 50, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100};
    SDL_RenderFillRect(game->getRenderer(), &panel);
    
    // Rendre le texte À propos
    if (aboutTexture) {
        SDL_Rect destRect = {
            panel.x + 20,
            panel.y + 20,
            panel.w - 40,
            panel.h - 40
        };
        SDL_RenderCopy(game->getRenderer(), aboutTexture, NULL, &destRect);
    }
}