#include "../headers/timer.hpp"

Timer::Timer() : 
    startTicks(0), 
    pausedTicks(0), 
    totalDuration(0),
    paused(false), 
    started(false) {}

void Timer::start(int seconds) {
    started = true;
    paused = false;
    
    // Conversion en millisecondes
    totalDuration = seconds * 1000;
    
    // Obtenir le temps actuel
    startTicks = SDL_GetTicks();
}

void Timer::stop() {
    started = false;
    paused = false;
    startTicks = 0;
    pausedTicks = 0;
}

void Timer::pause() {
    if (started && !paused) {
        paused = true;
        
        // Calculer le temps écoulé et le sauvegarder
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

void Timer::resume() {
    if (started && paused) {
        paused = false;
        
        // Réinitialiser le point de départ
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

int Timer::getRemainingTime() const {
    int elapsedMs;
    
    if (started) {
        if (paused) {
            elapsedMs = pausedTicks;
        } else {
            elapsedMs = SDL_GetTicks() - startTicks;
        }
        
        // Calculer le temps restant en secondes
        int remainingMs = totalDuration - elapsedMs;
        
        // Si le temps est écoulé, retourner 0
        if (remainingMs < 0) {
            return 0;
        }
        
        // Convertir en secondes et retourner
        return remainingMs / 1000;
    }
    
    return 0;
}

bool Timer::isTimeUp() const {
    if (started) {
        int elapsedMs;
        
        if (paused) {
            elapsedMs = pausedTicks;
        } else {
            elapsedMs = SDL_GetTicks() - startTicks;
        }
        
        return elapsedMs >= totalDuration;
    }
    
    return false;
}

bool Timer::isPaused() const {
    return paused && started;
}


bool Timer::isStarted() const {
    return started;
    
}

