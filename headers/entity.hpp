#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "GameConstants.hpp"

// Déclaration anticipée pour éviter la dépendance circulaire
class Game;

/*
Classe Entity - Représente le vélo du joueur
Cette classe gère toutes les fonctionnalités liées au vélo :
- Mouvement entre les voies
- Contrôle de la vitesse
- Rendu graphique
- Détection de collision
*/
class Entity {
    private:
    // Référence au jeu parent
    Game* game;
    
    // Ressources graphiques
    SDL_Texture* texture;
    
    // Positionnement et dimensions
    int lane;       // Voie actuelle (0 à LANES-1)
    int x, y;       // Position actuelle
    int targetX;    // Position cible pour animation fluide
    int width, height;
    
    // Attributs de mouvement
    int moveSpeed;     // Vitesse de déplacement horizontal
    int verticalSpeed; // Vitesse verticale (non utilisée pour le vélo)
    
    // Attributs de vitesse pour le gameplay
    int speed;      // Vitesse actuelle
    int minSpeed;   // Vitesse minimale
    int maxSpeed;   // Vitesse maximale
    
    /*
    Charge la texture de l'entité
    */
    void loadTexture();
    
    /*
    Affiche l'indicateur de vitesse
    Nouvelle méthode privée
    */
    void renderSpeedIndicator();
public:
    /*
    Constructeur
    game Pointeur vers l'instance du jeu
     */
    Entity(Game* game);
    
    /*
    Destructeur - Libère les ressources
    */
    ~Entity();
    
    /*
    Traite les événements d'entrée pour l'entité
    event Événement SDL à traiter
     */
    void handleEvents(SDL_Event& event);
    
    /*
    Met à jour l'état de l'entité
    */
    void update();
    
    /*
    Affiche l'entité à l'écran
    */
    void render();
    
    /*
    Réinitialise l'entité à son état initial
    */
    void reset();
    
    /*
    Retourne la zone de collision de l'entité
    return Rectangle de collision
    */
    SDL_Rect getCollisionBox() const;
    
    /*
    Déplace l'entité vers la voie de gauche
    */
    void moveLeft();
    
    /*
    Déplace l'entité vers la voie de droite
    */
    void moveRight();
    
    /*
    Augmente la vitesse de l'entité
    */
    void increaseSpeed();
    
    /*
    Diminue la vitesse de l'entité
    */
    void decreaseSpeed();
    
    /*
    Retourne la vitesse actuelle de l'entité
    return Vitesse actuelle
     */
    int getSpeed() const;
};

#endif // ENTITY_HPP