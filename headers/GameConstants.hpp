#ifndef GAME_CONSTANTS_HPP
#define GAME_CONSTANTS_HPP

// Dimensions de la fenêtre
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Nombre de voies
const int LANES = 3; // Modifié pour avoir 3 voies
const int LANE_WIDTH = WINDOW_WIDTH / LANES;

// FPS cible
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

// Durée du jeu en secondes
const int GAME_TIME = 60;

// Probabilité de génération d'obstacles (pourcentage)
const int OBSTACLE_SPAWN_RATE = 40;

// Couleurs prédéfinies (format SDL_Color = {r, g, b, a})
const SDL_Color NOIR = {0, 0, 0, 255};
const SDL_Color BLANC = {255, 255, 255, 255};
const SDL_Color ROUGE = {255, 0, 0, 255};
const SDL_Color VERT = {0, 255, 0, 255};
const SDL_Color BLEU = {0, 0, 255, 255};
const SDL_Color JAUNE = {255, 255, 0, 255};
const SDL_Color ARGENT = {192, 192, 192, 255};
const SDL_Color OR = {255, 215, 0, 255};

#endif // GAME_CONSTANTS_HPP