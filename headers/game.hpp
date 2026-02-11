/* game.hpp
   Classe principale gérant le cycle de vie et la logique du jeu de course à vélo

   Ce fichier d'en-tête définit la classe Game qui gère l'ensemble du jeu:
   - Initialisation de SDL2 et chargement des ressources
   - Boucle de jeu principale
   - Gestion des états du jeu (menu, jeu actif, game over)
   - Gestion des événements utilisateur
   - Rendu graphique et gestion des collisions */

   #ifndef GAME_HPP
   #define GAME_HPP
   
   #include <SDL2/SDL.h>
   #include <SDL2/SDL_image.h>
   #include <SDL2/SDL_ttf.h>
   #include <SDL2/SDL_mixer.h>
   #include <vector>
   #include <memory>
   #include <string>
   #include "GameConstants.hpp"
   #include "timer.hpp"
   #include "entity.hpp"
   #include "object.hpp"
   
   // Déclarations anticipées
   class Menu;
   
   /* États possibles du jeu */
   enum GameState {
       MENU = 0,      /* Menu principal */
       PLAYING = 1,   /* Jeu en cours */
       GAME_OVER = 2, /* Jeu terminé */
       EXIT = 3       /* Sortie du jeu */
   };
   
   /* États du tutoriel */
   enum TutorialState {
       TUTORIAL_NONE = 0,     /* Pas de tutoriel actif */
       TUTORIAL_CONTROLS = 1, /* Tutoriel sur les contrôles */
       TUTORIAL_OBSTACLES = 2, /* Tutoriel sur les obstacles */
       TUTORIAL_SPEED = 3     /* Tutoriel sur la vitesse */
   };
   
   /* Classe principale gérant l'ensemble du jeu */
   class Game {
    private:
       // Ressources SDL
       
       SDL_Window* window;
       SDL_Renderer* renderer;
       TTF_Font* font;
       TTF_Font* smallFont;
       SDL_Texture* roadTexture;
       Mix_Music* menuMusic;
       Mix_Music* gameMusic;
   
       // État du jeu
       bool isRunning;
       int currentState;
       bool gameWon;
       TutorialState tutorialState;
       Uint32 tutorialStartTime;
       bool tutorialsCompleted;
   
       // Entités du jeu
       std::unique_ptr<Menu> menu;
       std::unique_ptr<Entity> velo;
       std::vector<std::unique_ptr<Object>> obstacles;
   
       // Gestion du temps
       Timer gameTimer;
       Uint32 frameStart;
       int frameTime;
       Uint32 lastObstacleTime;
   
       /* Gère les événements utilisateur */
       void handleEvents();
       
       /* Met à jour l'état du jeu */
       void update();
       
       /* Effectue le rendu graphique */
       void render();
       
       /* Dessine les voies de la route */
       void renderLanes();
       
       /* Affiche le temps restant */
       void renderTimer();
       
       /* Affiche l'indicateur de vitesse */
       void renderSpeedIndicator();
       
       /* Affiche le tutoriel */
       void renderTutorial();
       
       /* Affiche l'écran de fin de jeu */
       void renderGameOver();
       
       /* Génère un obstacle */
       void spawnObstacle();
       
       /* Vérifie les collisions */
       void checkCollisions();
       
       /* Détecte une collision entre deux rectangles
           a Premier rectangle
           b Deuxième rectangle
           return true si collision, false sinon */
       bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) const;
       
       /* Avance à l'étape suivante du tutoriel */
       void advanceTutorial();
    public:
       /* Constructeur */
       Game();
       
       /* Destructeur */
       ~Game();
   
       /* Initialise les ressources SDL et du jeu
          return true si l'initialisation est réussie, false sinon */
       bool initialize();
       
       /* Lance la boucle principale du jeu */
       void run();
       
       /* Libère les ressources */
       void cleanup();
       
       /* Change l'état du jeu
          newState Nouvel état */
       void changeState(int newState);
   
       // Accesseurs
       SDL_Renderer* getRenderer() const { return renderer; }
       TTF_Font* getFont() const { return font; }
       TTF_Font* getSmallFont() const { return smallFont; }
       Mix_Music* getMenuMusic() const { return menuMusic; }
   
       /* Démarre le compte à rebours
          seconds Durée en secondes */
       void startTimer(int seconds);
       
       /* Obtient le temps restant
          return Temps restant en secondes */
       int getRemainingTime() const;
       
       /* Vérifie si le temps est écoulé
          return true si le temps est écoulé, false sinon */
       bool isTimeUp() const;
       
       /* Retourne la vitesse du vélo
          return Vitesse actuelle du vélo */
       int getVeloSpeed() const;
       
       /* Charge les fichiers audio
          return true si le chargement est réussi, false sinon */
       bool loadAudio();
       
       /* Joue une musique
         music Pointeur vers la musique à jouer */
       void playMusic(Mix_Music* music);
   };
   
   #endif // GAME_HPP