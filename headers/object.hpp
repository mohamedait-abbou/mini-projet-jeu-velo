#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "GameConstants.hpp"

// Déclaration anticipée pour éviter la dépendance circulaire
class Game;

/*
Classe Object - Représente les obstacles du jeu (uniquement des murs)
Cette classe gère les obstacles qui descendent à l'écran :
- Mouvement vertical
- Rendu graphique
- Détection de collision
*/
class Object {
    private:
    // Référence au jeu parent
    Game* game;
    
    // Ressources graphiques
    SDL_Texture* texture;

    // Positionnement et dimensions
    int lane;         
    int x, y;
    int width, height;

    // Vitesse de déplacement vertical
    int speed;
    /*
    Charge la texture de l'obstacle
     */
     void loadTexture();
public:
    /*
    Constructeur
    game Pointeur vers l'instance du jeu
    lane Voie sur laquelle placer l'obstacle
    startY Position Y initiale
     */
    Object(Game* game, int lane, int startY);
    
    /*
    Destructeur - Libère les ressources
    */
    ~Object();
    
    /*
    Met à jour l'état de l'obstacle
    Gère le mouvement vertical
    */
    void update();
    
    /*
    Affiche l'obstacle à l'écran
    */
    void render();
    
    /*
    Retourne la zone de collision de l'obstacle
    return Rectangle de collision
    */
    SDL_Rect getCollisionBox() const;
    
    /*
    Vérifie si l'obstacle est sorti de l'écran
    return true si l'obstacle est complètement hors écran
    */
    bool isOffScreen() const;
    
    /*
    Retourne la voie sur laquelle se trouve l'obstacle
    return Numéro de voie
    */
    int getLane() const;
    
    /*
    Retourne la position Y actuelle de l'obstacle
    return Position Y
    */
    int getY() const;
};

#endif // OBJECT_HPP