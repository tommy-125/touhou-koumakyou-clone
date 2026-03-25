#include "Scene/Stage1.hpp"

Stage1::Stage1(CharacterItem character, int spellCardIdx)
    : m_Player(character, spellCardIdx) {}

void Stage1::Update() {
    m_Player.Update();
}
