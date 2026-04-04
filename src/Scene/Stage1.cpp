#include "Scene/Stage1.hpp"

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard) : m_Player(character, spellCard) {}

void Stage1::Update() {
    m_EnemyManager.Update(m_Player.GetPos());
    m_Player.Update();

    m_EnemyManager.ApplyPlayerBulletDamage(m_Player);

    if (m_Player.IsVulnerable() &&
        m_EnemyManager.CheckPlayerHit(m_Player.GetPos(), {PLAYER_HITBOX_X, PLAYER_HITBOX_Y})) {
        m_Player.Die();
    }

    if (m_Player.JustEnteredSpawning()) {
        m_EnemyManager.ClearAllBullets();
    }

    m_Gui.Update(m_GameManager);
}
