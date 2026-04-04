#include "Enemy/EnemyBulletManager.hpp"

#include <cmath>

#include "Util/Math.hpp"

EnemyBulletManager::EnemyBulletManager() {
    m_Anm.LoadAnm(Anm::ETAMA3.folder, Anm::ETAMA3.txt, Anm::ETAMA3.offset);
}

EnemyBullet* EnemyBulletManager::AllocBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        int idx = (m_NextIdx + i) % MAX_BULLETS;
        if (!m_Bullets[idx].m_Alive) {
            m_NextIdx = (idx + 1) % MAX_BULLETS;
            return &m_Bullets[idx];
        }
    }
    // Pool full: overwrite oldest
    EnemyBullet* b = &m_Bullets[m_NextIdx];
    if (b->m_Vm.obj) {
        m_Renderer.RemoveChild(b->m_Vm.obj);
        b->m_Vm.obj = nullptr;
    }
    m_NextIdx = (m_NextIdx + 1) % MAX_BULLETS;
    return b;
}

void EnemyBulletManager::SpawnFanAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                       EBulletColor color, int count, float speed, float aimOffset,
                                       float spread) {
    int scriptIdx = Anm::ETAMA3.offset + static_cast<int>(type);
    int sprOffset = Anm::ETAMA3.offset + static_cast<int>(color);

    float aimAngle = std::atan2(playerPos.y - pos.y, playerPos.x - pos.x) + aimOffset;

    for (int i = 0; i < count; i++) {
        float delta;
        if (count & 1) {
            delta = ((i + 1) / 2.0f) * spread;
        } else {
            delta = (i / 2.0f) * spread + spread * 0.5f;
        }
        if (i & 1) delta = -delta;

        EnemyBullet* b = AllocBullet();
        *b             = EnemyBullet{};
        b->m_Alive     = true;
        b->m_Pos       = pos;
        b->m_Angle     = aimAngle + delta;
        b->m_Speed     = speed;

        m_Anm.SetScript(b->m_Vm, scriptIdx, sprOffset);
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
    }
}

void EnemyBulletManager::Update() {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;

        b.m_Pos.x += std::cos(b.m_Angle) * b.m_Speed;
        b.m_Pos.y += std::sin(b.m_Angle) * b.m_Speed;

        b.m_Vm.pos = b.m_Pos;
        m_Anm.UpdateObjects(b.m_Vm);

        if (!Util::IsInGameBounds(b.m_Pos.x, b.m_Pos.y, 0, 0)) {
            b.m_Alive = false;
            if (b.m_Vm.obj) {
                m_Renderer.RemoveChild(b.m_Vm.obj);
                b.m_Vm.obj = nullptr;
            }
        }
    }

    m_Renderer.Update();
}
