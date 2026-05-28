#ifndef ENEMY_LASER_MANAGER_HPP
#define ENEMY_LASER_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>

#include "Enemy/EnemyLaser.hpp"
#include "Util/Renderer.hpp"

class ItemManager;

class EnemyLaserManager {
   public:
    void SpawnAimed(glm::vec2 pos, glm::vec2 playerPos, float length, float maxWidth, int startTime,
                    int duration, int endTime, int hitboxStart, int hitboxEnd, float speed = 0.0f);
    void SpawnAtAngle(glm::vec2 pos, float angle, float length, float maxWidth, int startTime,
                      int duration, int endTime, int hitboxStart, int hitboxEnd,
                      float angularVelocity = 0.0f, float speed = 0.0f,
                      int angularVelocityFrames = -1, float startOffset = 0.0f);
    void Update();
    void Render();
    bool CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize);
    void ClearAll();
    void TurnAllLasersIntoPointItems(ItemManager& items);
    void TurnLasersIntoPointItemsInRadiusRange(ItemManager& items, glm::vec2 center,
                                               float innerRadius, float outerRadius);
    void SetTimeStopped(bool stopped) { m_TimeStopped = stopped; }

    static constexpr int MAX_LASERS = 128;

   private:
    EnemyLaser* AllocLaser();

    std::array<EnemyLaser, MAX_LASERS> m_Lasers{};
    int                                m_NextIdx = 0;
    bool                               m_TimeStopped = false;
    Util::Renderer                     m_Renderer;
};

#endif  // ENEMY_LASER_MANAGER_HPP
