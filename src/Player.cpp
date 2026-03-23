#include "Player.hpp"

#include "Util/Input.hpp"
#include <cmath>
#include <glm/glm.hpp>

Player::Player(PlayerItem character, int spellCardIdx)
    : m_Data(character == PlayerItem::Reimu ? &REIMU_DATA : &MARISA_DATA),
      m_SpellCardIdx(spellCardIdx) {

    const Anm::Entry &entry = *m_Data->anmEntry;
    int scriptCount = m_Anm.LoadAnm(entry.folder, entry.txt, entry.offset);

    // Set up body VM
    m_Vms.resize(scriptCount);
    m_Objs.resize(scriptCount);

    m_BodyVm                = &m_Vms[0];
    m_BodyVm->spriteOffset  = entry.offset;
    m_BodyVm->pos           = m_BodyPos;
    SetScript(PlayerScript::Idle);
    
    for(int i = 0; i < scriptCount; i++) {
        m_Objs[i] = std::make_shared<Util::GameObject>(nullptr, 1.0f, glm::vec2{0, 0}, false);
        m_Renderer.AddChild(m_Objs[i]);
    }
}

void Player::Update() {
    // ── Movement ──────────────────────────────────────────────────────────────
    float speed = Util::Input::IsKeyPressed(Util::Keycode::LSHIFT)
                      ? m_Data->speedFocused
                      : m_Data->speed;

    glm::vec2 delta = {0, 0};
    if (Util::Input::IsKeyPressed(Util::Keycode::UP))    delta.y -= speed;
    if (Util::Input::IsKeyPressed(Util::Keycode::DOWN))  delta.y += speed;
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT))  delta.x -= speed;
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) delta.x += speed;

    if (delta.x != 0 && delta.y != 0) // normalize diagonal movement
        delta *= (1.0f / std::sqrt(2.0f));

    m_BodyPos += delta;
    m_BodyPos.x = glm::clamp(m_BodyPos.x, PLAY_AREA_LEFT,  PLAY_AREA_RIGHT);
    m_BodyPos.y = glm::clamp(m_BodyPos.y, PLAY_AREA_TOP,   PLAY_AREA_BOTTOM);

    // ── State machine (left/right only) ───────────────────────────────────────
    bool pressingRight = Util::Input::IsKeyPressed(Util::Keycode::RIGHT);
    bool pressingLeft  = Util::Input::IsKeyPressed(Util::Keycode::LEFT);

    if (pressingRight && !pressingLeft) {
        SetMoveState(MoveState::MoveRight);
    } else if (pressingLeft && !pressingRight) {
        SetMoveState(MoveState::MoveLeft);
    } else {
        SetMoveState(MoveState::Idle);
    }

    // ── Return animation completion ───────────────────────────────────────────
    if (m_ReturnFramesLeft > 0 && m_MoveState == MoveState::Idle) {
        if (--m_ReturnFramesLeft == 0) {
            SetScript(PlayerScript::Idle);  // resets flipX, switches to Script 0
        }
    }

    // ── Sync position to VM ───────────────────────────────────────────────────
    m_BodyVm->pos = m_BodyPos;

    // ── ANM + render ──────────────────────────────────────────────────────────
    m_Anm.UpdateObjects(m_Vms, m_Objs);
    m_Renderer.Update();
}

void Player::SetMoveState(MoveState newState) {
    if (newState == m_MoveState) return;

    if (newState == MoveState::Idle) {
        if (m_MoveState == MoveState::MoveRight) { SetScript(PlayerScript::ReturnFromRight); m_ReturnFramesLeft = 49; }
        else if (m_MoveState == MoveState::MoveLeft) { SetScript(PlayerScript::ReturnFromLeft); m_ReturnFramesLeft = 49; }
    } else if (newState == MoveState::MoveRight) {
        SetScript(PlayerScript::MoveRight);
    } else if (newState == MoveState::MoveLeft) {
        SetScript(PlayerScript::MoveLeft);
    }

    m_MoveState = newState;
}

void Player::SetScript(PlayerScript script) {
    m_BodyVm->flipX = false;
    m_Anm.SetScript(*m_BodyVm, m_Data->anmEntry->offset + static_cast<int>(script), m_Data->anmEntry->offset);
}
