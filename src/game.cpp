/* game.cpp
Implémentation de la classe Game pour le jeu de course à vélo
Ce fichier contient l'implémentation des méthodes de la classe Game
responsable de la gestion globale du jeu: initialisation, boucle principale,
gestion des entités, rendu graphique et logique de jeu. */

#include "../headers/game.hpp"
#include "../headers/menu.hpp"
#include "../headers/entity.hpp"
#include "../headers/object.hpp"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <random>
   
   /* Constructeur de la classe Game
      Initialise tous les membres à leurs valeurs par défaut */
   Game::Game() :
       window(nullptr),
       renderer(nullptr),
       font(nullptr),
       smallFont(nullptr),
       roadTexture(nullptr),
       menuMusic(nullptr),
       gameMusic(nullptr),
       isRunning(false),
       currentState(GameState::MENU),
       gameWon(false),
       tutorialState(TUTORIAL_NONE),
       tutorialStartTime(0),
       tutorialsCompleted(false),
       lastObstacleTime(0) 
   {
       // Utilisation de listes d'initialisation pour optimiser la création d'objets
   }
   
   /* Destructeur de la classe Game
      Appelle la méthode cleanup pour libérer les ressources */
   Game::~Game() {
       cleanup();
   }
   
   /* Initialise les ressources SDL et du jeu */
   bool Game::initialize() {
       // Initialisation de SDL et ses extensions
       if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
           std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
           return false;
       }
   
       if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
           std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
           return false;
       }
   
       if (TTF_Init() < 0) {
           std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
           return false;
       }
   
       if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
           std::cerr << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
           return false;
       }
   
       // Création de la fenêtre
       window = SDL_CreateWindow("Jeu de course a velo :)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
       if (!window) {
           std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
           return false;
       }
   
       // Création du renderer
       renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
       if (!renderer) {
           std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
           return false;
       }
   
       SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   
       // Chargement des polices
       font = TTF_OpenFont("assets/OpenSans-Regular.ttf", 28);
       smallFont = TTF_OpenFont("assets/OpenSans-Regular.ttf", 18);
       if (!font || !smallFont) {
           std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
           return false;
       }
   
       // Chargement de la texture de route avec gestion d'erreur
       SDL_Surface* loadedSurface = IMG_Load("assets/road.png");
       if (!loadedSurface) {
           // Création d'une texture de secours
           SDL_Surface* fallbackSurface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);
           SDL_FillRect(fallbackSurface, NULL, SDL_MapRGB(fallbackSurface->format, 80, 80, 80));
           
           // Ajout des lignes de voie
           for (int i = 1; i < LANES; i++) {
               SDL_Rect lineRect = {i * LANE_WIDTH - 2, 0, 4, WINDOW_HEIGHT};
               SDL_FillRect(fallbackSurface, &lineRect, SDL_MapRGB(fallbackSurface->format, 255, 255, 0));
           }
           
           roadTexture = SDL_CreateTextureFromSurface(renderer, fallbackSurface);
           SDL_FreeSurface(fallbackSurface);
       } else {
           roadTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
           SDL_FreeSurface(loadedSurface);
       }
   
       // Chargement des fichiers audio
       if (!loadAudio()) {
           std::cerr << "Failed to load audio files" << std::endl;
           return false;
       }
   
       // Création des objets du jeu
       menu = std::make_unique<Menu>(this);
       velo = std::make_unique<Entity>(this); // Plus besoin de spécifier EntityType
       
       // Initialisation du générateur de nombres aléatoires
       std::srand(static_cast<unsigned int>(std::time(nullptr)));
   
       // Démarrage de la musique du menu
       playMusic(menuMusic);
       isRunning = true;
       return true;
   }
   
   /* Charge les fichiers audio du jeu */
   bool Game::loadAudio() {
       menuMusic = Mix_LoadMUS("assets/menu_music.wav");
       if (!menuMusic) {
           std::cerr << "Failed to load menu music: " << Mix_GetError() << std::endl;
           return false;
       }
   
       gameMusic = Mix_LoadMUS("assets/game1_music.mp3");
       if (!gameMusic) {
           std::cerr << "Failed to load game music: " << Mix_GetError() << std::endl;
           Mix_FreeMusic(menuMusic);
           return false;
       }
   
       return true;
   }
   
   /* Joue une musique en boucle */
   void Game::playMusic(Mix_Music* music) {
       if (music) {
           Mix_HaltMusic();
           if (Mix_PlayingMusic() == 0) {
               Mix_PlayMusic(music, -1);
           }
       }
   }
   
   /* Boucle principale du jeu
      Gère le timing, les événements, les mises à jour et le rendu */
   void Game::run() {
       while (isRunning) {
           frameStart = SDL_GetTicks();
   
           handleEvents();
           update();
           render();
           
           // Gestion du framerate constant
           frameTime = SDL_GetTicks() - frameStart;
           if (FRAME_DELAY > frameTime) {
               SDL_Delay(FRAME_DELAY - frameTime);
           }
       }
   }
   
   /* Passe à l'étape suivante du tutoriel */
   void Game::advanceTutorial() {
       switch (tutorialState) {
           case TUTORIAL_CONTROLS:
               tutorialState = TUTORIAL_OBSTACLES;
               spawnObstacle();
               break;
           case TUTORIAL_OBSTACLES:
               tutorialState = TUTORIAL_SPEED;
               break;
           case TUTORIAL_SPEED:
               tutorialState = TUTORIAL_NONE;
               tutorialsCompleted = true;
               break;
           default:
               tutorialState = TUTORIAL_NONE;
               break;
       }
       tutorialStartTime = SDL_GetTicks();
   }
   
   /* Gère les événements utilisateur */
   void Game::handleEvents() {
       SDL_Event event;
       while (SDL_PollEvent(&event)) {
           if (event.type == SDL_QUIT) {
               isRunning = false;
           }
   
           // Traitement des événements selon l'état du jeu
           switch (currentState) {
               case GameState::MENU:
                   menu->handleEvents(event);
                   break;
               case GameState::PLAYING:
                   velo->handleEvents(event);
                   // Gestion des touches pour le tutoriel
                   if (tutorialState != TUTORIAL_NONE && event.type == SDL_KEYDOWN) {
                       switch (tutorialState) {
                           case TUTORIAL_CONTROLS:
                               if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
                                   advanceTutorial();
                               }
                               break;
                           case TUTORIAL_OBSTACLES:
                               break;
                           case TUTORIAL_SPEED:
                               if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN) {
                                   advanceTutorial();
                               }
                               break;
                           default:
                               break;
                       }
                   }
                   break;
               case GameState::GAME_OVER:
                   if (event.type == SDL_KEYDOWN) {
                       if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
                           changeState(GameState::MENU);
                       }
                   }
                   break;
               default:
                   break;
           }
       }
   }
   
   /* Met à jour l'état du jeu */
   void Game::update() {
       Uint32 currentTime = SDL_GetTicks();
       
       switch (currentState) {
           case GameState::MENU:
               menu->update();
               break;
               
           case GameState::PLAYING:
               // Initialisation du tutoriel si nécessaire
               if (!tutorialsCompleted && tutorialState == TUTORIAL_NONE) {
                   tutorialState = TUTORIAL_CONTROLS;
                   tutorialStartTime = currentTime;
               }
   
               // Mise à jour du vélo
               velo->update();
   
               // Mise à jour des obstacles
               for (auto& obstacle : obstacles) {
                   obstacle->update();
               }
               
               // Suppression des obstacles sortis de l'écran
               obstacles.erase(
                   std::remove_if(obstacles.begin(), obstacles.end(),
                       [this](const std::unique_ptr<Object>& o) {
                           if (o->isOffScreen()) {
                               if (tutorialState == TUTORIAL_OBSTACLES) {
                                   advanceTutorial();
                               }
                               return true;
                           }
                           return false;
                       }),
                   obstacles.end()
               );
               
               // Génération de nouveaux obstacles
               if (tutorialsCompleted && currentTime - lastObstacleTime > 2000) {
                   spawnObstacle();
                   lastObstacleTime = currentTime;
               }
               
               // Vérification des collisions
               checkCollisions();
               break;
       }
   }
   
   /* Effectue le rendu graphique du jeu */
   void Game::render() {
       SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
       SDL_RenderClear(renderer);
   
       switch (currentState) {
           case GameState::MENU:
               menu->render();
               break;
               
           case GameState::PLAYING:
               // Affichage de la route
               SDL_RenderCopy(renderer, roadTexture, NULL, NULL);
               renderLanes();
               
               // Affichage des obstacles
               for (auto& obstacle : obstacles) {
                   obstacle->render();
               }
               
               // Affichage du vélo et des informations
               velo->render();
               renderTimer();
               renderSpeedIndicator();
               
               // Affichage du tutoriel si nécessaire
               if (!tutorialsCompleted) {
                   renderTutorial();
               }
               break;
               
           case GameState::GAME_OVER:
               renderGameOver();
               break;
       }
   
       SDL_RenderPresent(renderer);
   }
   
   /* Affiche les instructions du tutoriel */
   void Game::renderTutorial() {
       if (!smallFont) return;
   
       SDL_Color textColor = {255, 255, 255, 255};
       SDL_Color bgColor = {0, 0, 0, 180};
       std::string tutorialText;
   
       // Sélection du texte selon l'état du tutoriel
       switch (tutorialState) {
           case TUTORIAL_CONTROLS:
               tutorialText = "Utilisez les touches FLECHES pour changer de voie";
               break;
           case TUTORIAL_OBSTACLES:
               tutorialText = "Evitez les obstacles qui arrivent!";
               break;
           case TUTORIAL_SPEED:
               tutorialText = "Maintenez la touche HAUT ou BAS pour accelerer";
               break;
           default:
               return;
       }
   
       // Rendu du texte du tutoriel
       SDL_Surface* surface = TTF_RenderText_Solid(smallFont, tutorialText.c_str(), textColor);
       SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
   
       // Configuration de la boîte de tutoriel
       int boxWidth = surface->w + 40;
       int boxHeight = surface->h + 20;
       int boxX = (WINDOW_WIDTH - boxWidth) / 2;
       int boxY = 100;
   
       // Rendu de l'arrière-plan
       SDL_Rect bgRect = {boxX, boxY, boxWidth, boxHeight};
       SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
       SDL_RenderFillRect(renderer, &bgRect);
   
       // Rendu du texte
       SDL_Rect textRect = {boxX + 20, boxY + 10, surface->w, surface->h};
       SDL_RenderCopy(renderer, texture, NULL, &textRect);
   
       // Libération des ressources
       SDL_FreeSurface(surface);
       SDL_DestroyTexture(texture);
   }
   
   /* Affiche les lignes de voie sur la route */
   void Game::renderLanes() {
       SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
       
       // Dessin des lignes de voie en pointillés
       for (int i = 1; i < LANES; i++) {
           int x = i * (WINDOW_WIDTH / LANES);
           
           for (int y = 0; y < WINDOW_HEIGHT; y += 40) {
               SDL_Rect lineRect = {x - 2, y, 4, 20};
               SDL_RenderFillRect(renderer, &lineRect);
           }
       }
   }
   
   /* Affiche l'indicateur de vitesse */
   void Game::renderSpeedIndicator() {
       if (smallFont) {
           SDL_Color textColor = {255, 255, 255, 255};
           std::string speedText = "Vitesse: " + std::to_string(velo->getSpeed());
           
           // Rendu du texte
           SDL_Surface* surface = TTF_RenderText_Solid(smallFont, speedText.c_str(), textColor);
           SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
           
           // Positionnement en bas à droite
           SDL_Rect destRect = {WINDOW_WIDTH - surface->w - 20, WINDOW_HEIGHT - surface->h - 20, surface->w, surface->h};
           SDL_RenderCopy(renderer, texture, NULL, &destRect);
           
           // Libération des ressources
           SDL_FreeSurface(surface);
           SDL_DestroyTexture(texture);
       }
   }
   
   /* Libère toutes les ressources utilisées par le jeu */
   void Game::cleanup() {
       // Libération des ressources audio
       if (menuMusic) Mix_FreeMusic(menuMusic);
       if (gameMusic) Mix_FreeMusic(gameMusic);
       Mix_CloseAudio();
   
       // Libération des textures et polices
       if (roadTexture) SDL_DestroyTexture(roadTexture);
       if (font) TTF_CloseFont(font);
       if (smallFont) TTF_CloseFont(smallFont);
       if (renderer) SDL_DestroyRenderer(renderer);
       if (window) SDL_DestroyWindow(window);
   
       // Fermeture des bibliothèques SDL
       TTF_Quit();
       IMG_Quit();
       SDL_Quit();
   }
   
   /* Change l'état du jeu et effectue les actions appropriées */
   void Game::changeState(int newState) {
       if (newState == GameState::PLAYING) {
           // Réinitialisation pour une nouvelle partie
           obstacles.clear();
           velo->reset();
           gameWon = false;
           lastObstacleTime = SDL_GetTicks();
           gameTimer.start(GAME_TIME);
           
           // Configuration du tutoriel
           tutorialsCompleted = false;
           tutorialState = TUTORIAL_CONTROLS;
           tutorialStartTime = SDL_GetTicks();
           playMusic(gameMusic);
       }
       else if (newState == GameState::MENU) {
           Mix_HaltMusic();
           menu->playMenuMusic();
       }
       else if (newState == GameState::GAME_OVER) {
           Mix_HaltMusic();
       }
       else if (newState == GameState::EXIT) {
           isRunning = false;
           return;
       }
       
       currentState = newState;
   }
   
   /* Vérifie s'il y a collision entre deux rectangles */
   bool Game::checkCollision(const SDL_Rect& a, const SDL_Rect& b) const {
       return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
   }
   
   /* Démarre le compte à rebours du jeu */
   void Game::startTimer(int seconds) {
       gameTimer.start(seconds);
   }
   
   /* Obtient le temps restant du compte à rebours */
   int Game::getRemainingTime() const {
       return gameTimer.getRemainingTime();
   }
   
   /* Vérifie si le temps est écoulé */
   bool Game::isTimeUp() const {
       return gameTimer.isTimeUp();
   }
   
   /* Génère de nouveaux obstacles */
   void Game::spawnObstacle() {
       const int numLanes = 3;
   
       if (tutorialState == TUTORIAL_OBSTACLES) {
           // Un seul obstacle aléatoire pour le tutoriel
           int lane = rand() % numLanes;
           int verticalOffset = -70;
           obstacles.push_back(std::make_unique<Object>(this, lane, verticalOffset));
       } else {
           // Génération multiple d'obstacles avec positions décalées
           for (int lane = 0; lane < numLanes; ++lane) {
               int verticalOffset = -70 - (rand() % 100) - (lane * 100);
               obstacles.push_back(std::make_unique<Object>(this, lane, verticalOffset));
           }
           // Mélange des obstacles pour plus d'aléatoire
           std::random_device rd;
           std::mt19937 g(rd());
           std::shuffle(obstacles.begin(), obstacles.end(), g);
       }
   }
   
   /* Vérifie les collisions entre le vélo et les obstacles */
   void Game::checkCollisions() {
       SDL_Rect veloRect = velo->getCollisionBox();
   
       // Vérification des collisions avec tous les obstacles
       for (auto& obstacle : obstacles) {
           SDL_Rect obstacleRect = obstacle->getCollisionBox();
           
           if (checkCollision(veloRect, obstacleRect)) {
               currentState = GameState::GAME_OVER;
               return;
           }
       }
   
       // Fin de jeu si le temps est écoulé (victoire)
       if (getRemainingTime() <= 0) {
           gameWon = true;
           currentState = GameState::GAME_OVER;
       }
   }
   
   /* Affiche le temps restant */
   void Game::renderTimer() {
       if (font) {
           SDL_Color textColor = {255, 255, 255, 255};
           int totalSeconds = getRemainingTime();
           int minutes = totalSeconds / 60;
           int seconds = totalSeconds % 60;
           
           // Formatage du texte MM:SS avec padding de zéros
           std::string timeText = "Temps: ";
           timeText += (minutes < 10) ? "0" : "";
           timeText += std::to_string(minutes);
           timeText += ":";
           timeText += (seconds < 10) ? "0" : "";
           timeText += std::to_string(seconds);
           
           // Rendu du texte
           SDL_Surface* surface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
           SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
           
           SDL_Rect destRect = {20, 20, surface->w, surface->h};
           SDL_RenderCopy(renderer, texture, NULL, &destRect);
           
           // Libération des ressources
           SDL_FreeSurface(surface);
           SDL_DestroyTexture(texture);
       }
   }
   
   /* Affiche l'écran de fin de jeu */
   void Game::renderGameOver() {
       // Affichage du fond de jeu
       SDL_RenderCopy(renderer, roadTexture, NULL, NULL);
       renderLanes();
   
       for (auto& obstacle : obstacles) {
           obstacle->render();
       }
   
       velo->render();
   
       // Overlay semi-transparent
       SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
       SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
       SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
       SDL_RenderFillRect(renderer, &overlay);
   
       if (font) {
           // Message différent selon victoire ou défaite
           std::string message = gameWon ? "VICTOIRE!" : "PARTIE TERMINEE";
           SDL_Color messageColor = gameWon ? SDL_Color{255, 215, 0, 255} : SDL_Color{255, 0, 0, 255};
           
           // Rendu du message principal
           SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), messageColor);
           SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
           
           SDL_Rect destRect = {
               (WINDOW_WIDTH - surface->w) / 2, 
               (WINDOW_HEIGHT - surface->h) / 2 - 40, 
               surface->w, 
               surface->h
           };
           
           // Effet de lueur autour du texte
           for (int i = 5; i > 0; i--) {
               SDL_SetRenderDrawColor(renderer, messageColor.r, messageColor.g, messageColor.b, 50);
               SDL_Rect glowRect = {
                   destRect.x - i * 2, 
                   destRect.y - i * 2, 
                   destRect.w + i * 4, 
                   destRect.h + i * 4
               };
               SDL_RenderFillRect(renderer, &glowRect);
           }
           
           SDL_RenderCopy(renderer, texture, NULL, &destRect);
           
           // Instructions pour revenir au menu
           SDL_Color instructionColor = {192, 192, 192, 255};
           std::string instruction = "Appuyez sur ESPACE pour revenir au menu";
           
           SDL_Surface* instrSurface = TTF_RenderText_Solid(smallFont, instruction.c_str(), instructionColor);
           SDL_Texture* instrTexture = SDL_CreateTextureFromSurface(renderer, instrSurface);
           
           SDL_Rect instrRect = {
               (WINDOW_WIDTH - instrSurface->w) / 2, 
               destRect.y + destRect.h + 40, 
               instrSurface->w, 
               instrSurface->h
           };
           
           SDL_RenderCopy(renderer, instrTexture, NULL, &instrRect);
           
           // Libération des ressources
           SDL_FreeSurface(surface);
           SDL_DestroyTexture(texture);
           SDL_FreeSurface(instrSurface);
           SDL_DestroyTexture(instrTexture);
       }
   }
   
   /* Retourne la vitesse actuelle du vélo */
   int Game::getVeloSpeed() const {
       return velo->getSpeed();
   }