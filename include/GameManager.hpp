#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <algorithm>
#include <array>

struct GameManager {
    static constexpr int MAX_LIVES = 8;
    static constexpr std::array<int, 4> SCORE_EXTENDS = {
        10000000,
        20000000,
        40000000,
        60000000,
    };

    int livesRemaining = 2;
    int bombsRemaining = 3;
    int score          = 0;
    int highScore      = 0;
    int power          = 0;  // 0..128
    int graze          = 0;
    int pointItems     = 0;
    int stageGraze      = 0;
    int stagePointItems = 0;
    int scoreExtendIdx = 0;
    bool timeStopped   = false;
    bool bombActive    = false;

    int AddScore(int points) {
        if (points <= 0) return 0;

        score += points;
        if (score > highScore) highScore = score;
        return CheckScoreExtends();
    }

    int CheckScoreExtends() {
        int extendCount = 0;
        while (scoreExtendIdx < static_cast<int>(SCORE_EXTENDS.size()) &&
               score >= SCORE_EXTENDS[scoreExtendIdx]) {
            if (livesRemaining < MAX_LIVES) {
                ++livesRemaining;
                ++extendCount;
            }
            ++scoreExtendIdx;
        }
        return extendCount;
    }

    void ResetStageStats() {
        stageGraze      = 0;
        stagePointItems = 0;
    }

    void AddGraze(int count) {
        if (count <= 0) return;

        stageGraze = std::min(9999, stageGraze + count);
        graze      = std::min(999999, graze + count);
    }

    void AddPointItem() {
        ++stagePointItems;
        ++pointItems;
    }

    int PointBulletScore() const {
        return std::max(100, (stageGraze / 3) * 10 + 500);
    }
};

#endif  // GAME_MANAGER_HPP
