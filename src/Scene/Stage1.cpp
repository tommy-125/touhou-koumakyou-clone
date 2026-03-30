#include "Scene/Stage1.hpp"

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard)
    : m_Player(character, spellCard) {}

void Stage1::Update() {
    // Update enemies first so they render behind the player
    m_EnemyManager.Update(m_Player.GetPos());
    m_Player.Update();
}
