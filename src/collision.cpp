#include "../headers/collision.hpp"
#include <cmath>
/*
Système de détection de collision pour jeu de course à vélo
Ce fichier implémente les méthodes de détection de collision définies dans collision.hpp.
Ces fonctions sont utilisées pour détecter les interactions entre les différentes entités
du jeu, comme les vélos, les obstacles et les limites de la piste.
*/

bool Collision::checkRectCollision(const SDL_Rect& a, const SDL_Rect& b) {
    // Algorithme de détection de collision AABB (Axis-Aligned Bounding Box)
    // Vérifie si les projections des rectangles sur les axes X et Y se chevauchent
    // Cette méthode est efficace car elle n'utilise que des comparaisons simples
    /*
    typedef struct SDL_Rect {
    int x;    // Position X du coin supérieur gauche du rectangle
    int y;    // Position Y du coin supérieur gauche du rectangle
    int w;    // Largeur (width) du rectangle
    int h;    // Hauteur (height) du rectangle
    } SDL_Rect;
    */
    return (a.x < b.x + b.w && 
            a.x + a.w > b.x && 
            a.y < b.y + b.h && 
            a.y + a.h > b.y);
}

bool Collision::pointInRect(int x, int y, const SDL_Rect& rect) {
    // Vérifie si les coordonnées du point sont comprises dans les limites du rectangle
    // Les comparaisons sont inclusives pour la limite inférieure et exclusives pour la limite supérieure
    // pour éviter les détections de collision aux bordures adjacentes
    return (x >= rect.x && 
            x < rect.x + rect.w && 
            y >= rect.y && 
            y < rect.y + rect.h);
}

bool Collision::rectInRect(const SDL_Rect& inner, const SDL_Rect& outer) {
    // Vérifie si le rectangle inner est entièrement contenu dans le rectangle outer
    // Tous les coins du rectangle intérieur doivent être à l'intérieur du rectangle extérieur
    return (inner.x >= outer.x && 
            inner.y >= outer.y && 
            inner.x + inner.w <= outer.x + outer.w && 
            inner.y + inner.h <= outer.y + outer.h);
}

bool Collision::rectsAreNear(const SDL_Rect& a, const SDL_Rect& b, int distance) {
    // Crée un rectangle étendu autour de a avec une marge égale à distance
    // Cette approche est plus efficace que de calculer la distance euclidienne
    // entre chaque point des rectangles
    
    // Utilisation d'un rectangle temporaire plutôt qu'une modification de a
    SDL_Rect extendedA = {
        a.x - distance,
        a.y - distance,
        a.w + 2 * distance,
        a.h + 2 * distance
    };
    
    // Réutilise la fonction checkRectCollision pour vérifier si le rectangle étendu
    // entre en collision avec le rectangle b
    return checkRectCollision(extendedA, b);
}
// Exemples :

// checkRectCollision :
/*
Créons un vélo et un obstacle
SDL_Rect velo = {100, 200, 40, 20};     // Position (100,200), largeur 40, hauteur 20
SDL_Rect obstacle = {120, 205, 30, 15}; // Position (120,205), largeur 30, hauteur 15
Vérifions s'il y a collision
bool collision = Collision::checkRectCollision(velo, obstacle);
Résultat: true (les rectangles se chevauchent)
*/

// pointInRect :
/*
Créons un bouton dans notre interface
SDL_Rect bouton = {200, 150, 100, 50}; // Position (200,150), largeur 100, hauteur 50
Coordonnées d'un clic de souris
int mouseX = 225;
int mouseY = 160;
Vérifions si le clic est sur le bouton
bool clicSurBouton = Collision::pointInRect(mouseX, mouseY, bouton);
Résultat: true (le point est dans le rectangle)
*/

// rectInRect:
/*
Créons la zone de jeu et un vélo
SDL_Rect zoneDeJeu = {50, 50, 700, 500}; // Zone de jeu de 700x500 à la position (50,50)
SDL_Rect velo = {100, 200, 40, 20};      // Vélo de 40x20 à la position (100,200)
Vérifions si le vélo est entièrement dans la zone de jeu
bool veloEnZone = Collision::rectInRect(velo, zoneDeJeu);
Résultat: true (le vélo est entièrement dans la zone)
*/

// rectsAreNear :
/*
Créons deux vélos
SDL_Rect velo1 = {100, 200, 40, 20};  // Premier vélo
SDL_Rect velo2 = {200, 210, 40, 20};  // Second vélo
Vérifions s'ils sont proches (à moins de 50 pixels)
bool velosProches = Collision::rectsAreNear(velo1, velo2, 50);
Résultat: true (les vélos sont à une distance inférieure à 50 pixels)
*/