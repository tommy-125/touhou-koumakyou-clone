#ifndef ENEMY_LASER_MANAGER_HPP
#define ENEMY_LASER_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>

#include "Enemy/EnemyLaser.hpp"
#include "Util/Renderer.hpp"

class EnemyLaserManager {
   public:
    void SpawnAimed(glm::vec2 pos, glm::vec2 playerPos, float length, float maxWidth, int startTime,
                    int duration, int endTime, int hitboxStart, int hitboxEnd);
    void SpawnAtAngle(glm::vec2 pos, float angle, float length, float maxWidth, int startTime,
                      int duration, int endTime, int hitboxStart, int hitboxEnd,
                      float angularVelocity = 0.0f);
    void Update();
    bool CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize);
    void ClearAll();

    static constexpr int MAX_LASERS = 32;

   private:
    EnemyLaser* AllocLaser();

    std::array<EnemyLaser, MAX_LASERS> m_Lasers{};
    Util::Renderer                     m_Renderer;
};

#endif  // ENEMY_LASER_MANAGER_HPP
