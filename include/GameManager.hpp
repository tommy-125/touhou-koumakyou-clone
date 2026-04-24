#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

struct GameManager {
    int livesRemaining = 2;
    int bombsRemaining = 3;
    int score          = 0;
    int highScore      = 0;
    int power          = 0;  // 0–128
    int graze          = 0;
    int pointItems     = 0;
};

#endif  // GAME_MANAGER_HPP
