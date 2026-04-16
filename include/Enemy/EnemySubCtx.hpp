#ifndef ENEMY_SUB_CTX_HPP
#define ENEMY_SUB_CTX_HPP

#include <glm/glm.hpp>

struct Enemy;
class EnemyBulletManager;
class EnemyLaserManager;
class ItemManager;
namespace Anm {
class Manager;
}
namespace Util {
class Renderer;
}

struct EnemySubCtx {
    Anm::Manager&       anm;
    EnemyBulletManager& bullets;
    EnemyLaserManager&  lasers;
    ItemManager&        items;
    Util::Renderer&     renderer;
    glm::vec2           playerPos;

    void TransitionToSub(Enemy& e, int newSub) const;
    void StartLerpTo(Enemy& e, float targetX, float targetY, int frames) const;
    void StartLerpDir(Enemy& e, float speed, int frames) const;
    void MoveRandInBounds(Enemy& e) const;
};

#endif  // ENEMY_SUB_CTX_HPP
