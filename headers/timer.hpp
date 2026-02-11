#ifndef TIMER_HPP
#define TIMER_HPP

#include <SDL2/SDL.h>

class Timer {
public:
    Timer();
    
    void start(int seconds);
    void stop();
    void pause();
    void resume();
    
    int getRemainingTime() const;
    bool isTimeUp() const;
    bool isPaused() const;
    bool isStarted() const;
    
private:
    int startTicks;      // Le temps au démarrage du timer
    int pausedTicks;     // Le temps lors de la mise en pause
    int totalDuration;   // Durée totale en millisecondes
    
    bool paused;
    bool started;
};

#endif // TIMER_HPP