#include "../headers/entity.hpp"
#include "../headers/game.hpp"
#include <iostream>
#include <string>

/* 
Constructeur de la classe Entity
Initialise un objet Entity représentant le vélo du joueur
Utilise une liste d'initialisation pour optimiser la création des attributs 
*/
Entity::Entity(Game* game) : 
    game(game), 
    texture(nullptr),
    lane(LANES / 2),  // Position de départ au milieu
    width(70),
    height(80),
    moveSpeed(10),
    verticalSpeed(0),  // Vélo fixe verticalement
    speed(3),         // Vitesse initiale
    minSpeed(1),      // Vitesse minimale
    maxSpeed(10)      // Vitesse maximale
{
    // Calcul de la position X centrée dans la voie
    x = lane * LANE_WIDTH + (LANE_WIDTH - width) / 2;
    targetX = x;
    
    // Position Y fixe en bas de l'écran
    y = WINDOW_HEIGHT - height - 50;
    
    // Chargement de la texture
    loadTexture();
}

/* 
Destructeur de la classe Entity
Libère la mémoire allouée pour la texture SDL
Évite les fuites de mémoire 
*/
Entity::~Entity() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;  // Bonne pratique pour éviter les double-free
    }
}

/* 
Gère les événements SDL pour l'entité
Traite les entrées clavier pour contrôler le vélo 
*/
void Entity::handleEvents(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
            case SDLK_a:  // Support des contrôles WASD et flèches
                moveLeft();
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                moveRight();
                break;
            case SDLK_UP:
            case SDLK_w:
                increaseSpeed();
                break;
            case SDLK_DOWN:
            case SDLK_s:
                decreaseSpeed();
                break;
        }
    }
}

/* 
Met à jour l'état de l'entité
Gère le mouvement fluide entre la position actuelle et la position cible 
*/
void Entity::update() {
    // Animation fluide du mouvement horizontal
    if (x < targetX) {
        x += moveSpeed;
        if (x > targetX) x = targetX;  // Évite le dépassement
    } else if (x > targetX) {
        x -= moveSpeed;
        if (x < targetX) x = targetX;  // Évite le dépassement
    }
    
    // Position Y fixe (le vélo ne se déplace pas verticalement)
}

/* 
Affiche l'entité à l'écran
Gère le rendu de la texture et des indicateurs visuels 
*/
void Entity::render() {
    if (!texture || !game) return;  // Protection contre les pointeurs nuls
    
    // Rendu de la texture principale
    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopy(game->getRenderer(), texture, nullptr, &destRect);
    
    // Affichage de la boîte de collision en mode debug
    #ifdef DEBUG_COLLISION
    SDL_SetRenderDrawColor(game->getRenderer(), 0, 255, 0, 128);
    SDL_Rect collisionBox = getCollisionBox();
    SDL_RenderDrawRect(game->getRenderer(), &collisionBox);
    #endif
    
    // Affichage de l'indicateur de vitesse
    renderSpeedIndicator();
}

/* 
Affiche l'indicateur de vitesse du vélo
Nouvelle méthode pour respecter le principe de responsabilité unique 
*/
void Entity::renderSpeedIndicator() {
    SDL_SetRenderDrawColor(game->getRenderer(), 255, 0, 0, 255);
    for (int i = 0; i < speed; i++) {
        SDL_Rect speedBar = {x + i * 5, y - 10, 4, 8};
        SDL_RenderFillRect(game->getRenderer(), &speedBar);
    }
}

/* 
Retourne la boîte de collision de l'entité
Utilisée pour la détection de collision 
*/
SDL_Rect Entity::getCollisionBox() const {
    // Boîte légèrement plus petite que l'entité pour collision plus précise
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
Réinitialise l'entité à son état initial
Appelée après une collision ou au début d'une nouvelle partie 
*/
void Entity::reset() {
    lane = LANES / 2;  // Retour à la voie du milieu
    speed = 3;         // Réinitialisation de la vitesse
    
    // Recalcul des positions
    x = lane * LANE_WIDTH + (LANE_WIDTH - width) / 2;
    targetX = x;
    y = WINDOW_HEIGHT - height - 50;
}

/* 
Déplace l'entité vers la voie de gauche
Vérifie les limites pour éviter de sortir de l'écran 
*/
void Entity::moveLeft() {
    if (lane > 0) {
        lane--;
        targetX = lane * LANE_WIDTH + (LANE_WIDTH - width) / 2;
    }
}

/* 
Déplace l'entité vers la voie de droite
Vérifie les limites pour éviter de sortir de l'écran 
*/
void Entity::moveRight() {
    if (lane < LANES - 1) {
        lane++;
        targetX = lane * LANE_WIDTH + (LANE_WIDTH - width) / 2;
    }
}

/* 
Augmente la vitesse de l'entité
Respecte la vitesse maximale définie 
*/
void Entity::increaseSpeed() {
    if (speed < maxSpeed) {
        speed++;
    }
}

/* 
Diminue la vitesse de l'entité
Respecte la vitesse minimale définie 
*/
void Entity::decreaseSpeed() {
    if (speed > minSpeed) {
        speed--;
    }
}

/* 
Retourne la vitesse actuelle de l'entité 
*/
int Entity::getSpeed() const {
    return speed;
}

/* 
Charge la texture de l'entité depuis un fichier
Gère les erreurs de chargement avec une texture de secours 
*/
void Entity::loadTexture() {
    SDL_Surface* surface = IMG_Load("assets/bike.png");
    
    if (!surface) {
        std::cerr << "Erreur de chargement de l'image du vélo: " << IMG_GetError() << std::endl;
        
        // Création d'une texture de secours (rectangle vert)
        surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        if (surface) {
            SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0, 255, 0));
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