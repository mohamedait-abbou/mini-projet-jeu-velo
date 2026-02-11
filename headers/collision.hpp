#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <SDL2/SDL.h>

/*
Collision :
Fournit des méthodes statiques pour la détection de collision dans un environnement 2D
Cette classe utilitaire implémente différentes méthodes de détection de collision
entre rectangles et points en utilisant les structures SDL_Rect de la bibliothèque SDL2.
Elle est conçue pour être utilisée sans instanciation, avec uniquement des méthodes statiques.
*/
class Collision {
public:
    /*
    Vérifie si deux rectangles entrent en collision
    a Premier rectangle
    b Second rectangle
    return true si les rectangles se chevauchent, false sinon
    */
    static bool checkRectCollision(const SDL_Rect& a, const SDL_Rect& b);
    
    /*
    Vérifie si un point est à l'intérieur d'un rectangle
    x Coordonnée X du point
    y Coordonnée Y du point
    rect Rectangle à vérifier
    return true si le point est dans le rectangle, false sinon
    */
    static bool pointInRect(int x, int y, const SDL_Rect& rect);
    
    /*
    Vérifie si un rectangle est entièrement contenu dans un autre
    inner Rectangle intérieur
    outer Rectangle extérieur
    return true si inner est entièrement dans outer, false sinon
    */
    static bool rectInRect(const SDL_Rect& inner, const SDL_Rect& outer);
    
    /*
    Vérifie si deux rectangles sont proches l'un de l'autre
    a Premier rectangle
    b Second rectangle
    distance Distance maximale entre les rectangles
    return true si les rectangles sont à une distance inférieure ou égale à la distance spécifiée
    */
    static bool rectsAreNear(const SDL_Rect& a, const SDL_Rect& b, int distance);
};

#endif // COLLISION_HPP