#include "../headers/object.hpp"
#include "../headers/game.hpp"
#include <iostream>
#include <string>



/*
Constructeur de la classe Object
Initialise un objet représentant un mur dans le jeu
game Pointeur vers l'objet Game parent
lane Voie sur laquelle l'obstacle est positionné
startY Position Y initiale de l'obstacle
*/
Object::Object(Game* game, int lane, int startY) : 
    game(game), 
    texture(nullptr),
    lane(lane),
    y(startY) {
    
    // Configuration des propriétés du mur
    width = 120;
    height = 30;
    speed = 3;
    
    // Calcul de la position X centrée dans la voie
    x = lane * LANE_WIDTH + (LANE_WIDTH - width) / 2;
    
    // Chargement de la texture
    loadTexture();
}

/*
Destructeur de la classe Object
Libère la mémoire allouée pour la texture SDL
*/
Object::~Object() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;  // Évite les problèmes de double-free
    }
}

/*
Met à jour l'état de l'objet
Gère le mouvement vertical de l'obstacle en fonction de la vitesse du vélo
*/
void Object::update() {
    // Obtenir la vitesse actuelle du vélo du joueur
    int veloSpeed = game->getVeloSpeed();
    
    // La vitesse de déplacement des obstacles dépend de la vitesse du vélo
    // Crée l'illusion d'accélération du joueur
    y += speed + (veloSpeed / 2);
}

/*
Affiche l'objet à l'écran
Gère le rendu de la texture et des éléments de debug
*/
void Object::render() {
    if (!texture || !game) return;  // Protection contre les pointeurs nuls
    
    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopy(game->getRenderer(), texture, nullptr, &destRect);
    
    // Affichage de la boîte de collision en mode debug
    #ifdef DEBUG_COLLISION
    SDL_SetRenderDrawColor(game->getRenderer(), 255, 0, 0, 128);
    SDL_Rect collisionBox = getCollisionBox();
    SDL_RenderDrawRect(game->getRenderer(), &collisionBox);
    #endif
}

/*
Retourne la boîte de collision de l'objet
Utilisée pour la détection de collision
return SDL_Rect représentant la zone de collision
*/
SDL_Rect Object::getCollisionBox() const {
    // Boîte légèrement plus petite que l'objet pour collision plus réaliste
    const int padding = 5;
    SDL_Rect collisionBox = {
        x + padding,
        y + padding,
        width - 2 * padding,
        height - 2 * padding
    };
    return collisionBox;
}

/*
Vérifie si l'objet est sorti de l'écran
Permet de supprimer les objets qui ne sont plus visibles
return bool true si l'objet est complètement sorti de l'écran
*/
bool Object::isOffScreen() const {
    return (y > WINDOW_HEIGHT);
}

/*
Retourne la voie sur laquelle l'objet est positionné
return int Numéro de la voie
*/
int Object::getLane() const {
    return lane;
}

/*
Retourne la position Y actuelle de l'objet
return int Position Y
*/
int Object::getY() const {
    return y;
}

/*
Charge la texture de l'objet depuis un fichier
Gère les erreurs de chargement avec une texture de secours
*/
void Object::loadTexture() {
    SDL_Surface* surface = IMG_Load("assets/wall.png");
    
    if (!surface) {
        std::cerr << "Erreur de chargement de l'image du mur: " << IMG_GetError() << std::endl;
        
        // Création d'une texture de secours (rectangle gris)
        surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        if (surface) {
            SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 128, 128, 128));
        } else {
            std::cerr << "Échec de création de la surface de secours!" << std::endl;
            return;
        }
    }
    
    // Création de la texture à partir de la surface
    if (surface) {
        texture = SDL_CreateTextureFromSurface(game->getRenderer(), surface);
        SDL_FreeSurface(surface);  // Libération de la surface qui n'est plus nécessaire
        
        if (!texture) {
            std::cerr << "Échec de création de la texture: " << SDL_GetError() << std::endl;
        }
    }
}