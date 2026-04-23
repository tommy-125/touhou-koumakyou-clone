#include "Player.hpp"

#include <cmath>
#include <glm/glm.hpp>

#include "BulletData.hpp"
#include "GameManager.hpp"
#include "Util/Input.hpp"
#include "Util/Math.hpp"

static constexpr float PLAYER_LASER_DEFAULT_LENGTH = 176.0f;
static constexpr float PLAYER_LASER_GROWTH_RATE    = 24.0f;
static constexpr int   PLAYER_LASER_HOLD_FRAMES    = 2;

Player::Player(CharacterItem character, SpellCardItem spellCard)
    : m_Data(character == CharacterItem::Reimu ? &REIMU_DATA : &MARISA_DATA),
      m_SpellCard(spellCard) {
    const Anm::Entry& entry = *m_Data->m_AnmEntry;
    // Load ANM
    m_Anm.LoadAnm(entry.folder, entry.txt, entry.offset);

    // Set up body VM
    m_Vms.resize(3);

    m_BodyVm               = &m_Vms[0];
    m_BodyVm->spriteOffset = entry.offset;
    m_BodyVm->pos          = m_BodyPos;
    SetMoveScript(PlayerMovementScript::Idle);

    m_OrbVms[0] = &m_Vms[1];
    m_Anm.SetScript(*m_OrbVms[0], Anm::SCRIPT_REIMU_LEFT_ORB, entry.offset);
    m_OrbVms[1] = &m_Vms[2];
    m_Anm.SetScript(*m_OrbVms[1], Anm::SCRIPT_REIMU_RIGHT_ORB, entry.offset);

    for (int i = 0; i < static_cast<int>(m_Vms.size()); i++) {
        if (m_Vms[i].obj) {
            m_Renderer.AddChild(m_Vms[i].obj);
        }
    }

    m_FireBulletCallback = [this](PlayerBullet* bullet, int bulletIdx) {
        return FireSingleBullet(bullet, bulletIdx, CharacterPowerDataReimuB);
    };
}

void Player::Die() {
    if (m_PlayerState != PlayerState::ALIVE) return;
    m_PlayerState     = PlayerState::DEAD;
    m_DeadTimer       = 0;
    m_FireBulletTimer = -1;
}

int Player::CalcDamageToEnemy(glm::vec2 enemyPos, glm::vec2 enemyHitboxSize) {
    int total = 0;
    for (auto& b : m_Bullets) {
        if (b.m_BulletState != BulletState::FIRED) continue;
        if (b.m_BulletType == BulletType::LASER) {
            const float dx = std::abs(b.m_Vm.pos.x - enemyPos.x);
            const float dy = std::abs(b.m_Vm.pos.y - enemyPos.y);
            if (dx < (b.m_Size.x + enemyHitboxSize.x) * 0.5f &&
                dy < (b.m_Size.y + enemyHitboxSize.y) * 0.5f) {
                total += b.m_Damage;
            }
            continue;
        }

        float dx = std::abs(b.m_Vm.pos.x - enemyPos.x);
        float dy = std::abs(b.m_Vm.pos.y - enemyPos.y);
        // Both m_Size and enemyHitboxSize are full widths (matches TH6 SetVecCorners).
        if (dx < (b.m_Size.x + enemyHitboxSize.x) * 0.5f &&
            dy < (b.m_Size.y + enemyHitboxSize.y) * 0.5f) {
            b.m_BulletState = BulletState::COLLIDED;
            total += b.m_Damage;
        }
    }
    return total;
}

void Player::UpdateState() {
    m_JustEnteredSpawning = false;
    switch (m_PlayerState) {
        case PlayerState::DEAD:
            if (++m_DeadTimer >= 30) {
                m_PlayerState         = PlayerState::SPAWNING;
                m_SpawnTimer          = 0;
                m_JustEnteredSpawning = true;
            }
            break;
        case PlayerState::SPAWNING:
            if (++m_SpawnTimer >= 30) {
                m_PlayerState = PlayerState::INVULNERABLE;
                m_InvulTimer  = 0;
                m_BodyPos     = Util::GameFieldToScreen(PLAYER_SPAWN_FIELD_X, PLAYER_SPAWN_FIELD_Y);
            }
            break;
        case PlayerState::INVULNERABLE:
            if (++m_InvulTimer >= 240) {
                m_PlayerState = PlayerState::ALIVE;
            }
            break;
        default:
            break;
    }
}

void Player::Update(GameManager& gm) {
    m_Power = gm.power;
    UpdateState();
    HandlePlayerInput();
    m_HitboxTopLeft     = m_BodyPos - m_HitboxSize;
    m_HitboxBottomRight = m_BodyPos + m_HitboxSize;

    m_GrabItemTopLeft     = m_BodyPos - m_GrabItemSize;
    m_GrabItemBottomRight = m_BodyPos + m_GrabItemSize;

    m_OrbVms[0]->pos = m_BodyPos;
    m_OrbVms[1]->pos = m_BodyPos;

    float horizontalOrbOffset = 0.0f;
    float verticalOrbOffset   = 0.0f;

    float intermediateRatio;

    if (m_Power < 8) {
        m_OrbVisible = false;
    } else {
        m_OrbVisible = true;
        if (m_IsFocus && m_FocusTimer < 8) {
            m_FocusTimer++;
        } else if (!m_IsFocus && m_FocusTimer > 0) {
            m_FocusTimer--;
        }
        intermediateRatio   = static_cast<float>(m_FocusTimer) / 8.0f;
        verticalOrbOffset   = -32.0f * intermediateRatio;
        horizontalOrbOffset = -16.0f * (intermediateRatio * intermediateRatio) + 24.0f;
    }
    m_OrbVms[0]->pos.x += horizontalOrbOffset;
    m_OrbVms[1]->pos.x -= horizontalOrbOffset;
    m_OrbVms[0]->pos.y += verticalOrbOffset;
    m_OrbVms[1]->pos.y += verticalOrbOffset;

    UpdateFireBulletsTimer();
    UpdatePlayerBullets();
    // ── ANM + render ──────────────────────────────────────────────────────────
    m_Anm.UpdateObjects(m_Vms);

    // Override visibility after ANM update
    bool bodyVisible = true;
    if (m_PlayerState == PlayerState::DEAD || m_PlayerState == PlayerState::SPAWNING) {
        bodyVisible = false;
    } else if (m_PlayerState == PlayerState::INVULNERABLE) {
        bodyVisible = (m_InvulTimer / 4) % 2 == 0;
    }
    m_BodyVm->obj->SetVisible(bodyVisible);

    bool orbsVisible = m_OrbVisible && bodyVisible;
    m_OrbVms[0]->obj->SetVisible(orbsVisible);
    m_OrbVms[1]->obj->SetVisible(orbsVisible);

    m_Renderer.Update();
    gm.power = m_Power;
}

void Player::SetMoveScript(PlayerMovementScript script) {
    m_BodyVm->flipX = false;
    m_Anm.SetScript(*m_BodyVm, m_Data->m_AnmEntry->offset + static_cast<int>(script),
                    m_Data->m_AnmEntry->offset);
}

void Player::SetMoveState(MoveState newState) {
    if (newState == m_MoveState) return;

    if (newState == MoveState::Idle) {
        if (m_MoveState == MoveState::MoveRight) {
            SetMoveScript(PlayerMovementScript::ReturnFromRight);
            m_ReturnFramesLeft = 49;
        } else if (m_MoveState == MoveState::MoveLeft) {
            SetMoveScript(PlayerMovementScript::ReturnFromLeft);
            m_ReturnFramesLeft = 49;
        }
    } else if (newState == MoveState::MoveRight) {
        SetMoveScript(PlayerMovementScript::MoveRight);
    } else if (newState == MoveState::MoveLeft) {
        SetMoveScript(PlayerMovementScript::MoveLeft);
    }

    m_MoveState = newState;
}

void Player::HandleMovement() {  // TODO: add spell card specific movement behavior
    // ── Movement ──────────────────────────────────────────────────────────────
    bool focused    = Util::Input::IsKeyPressed(Util::Keycode::LSHIFT);
    bool movingDiag = (Util::Input::IsKeyPressed(Util::Keycode::UP) ||
                       Util::Input::IsKeyPressed(Util::Keycode::DOWN)) &&
                      (Util::Input::IsKeyPressed(Util::Keycode::LEFT) ||
                       Util::Input::IsKeyPressed(Util::Keycode::RIGHT));

    float speed;
    if (movingDiag)
        speed = focused ? m_Data->m_DiagonalMovementSpeedFocus : m_Data->m_DiagonalMovementSpeed;
    else
        speed =
            focused ? m_Data->m_OrthogonalMovementSpeedFocus : m_Data->m_OrthogonalMovementSpeed;

    glm::vec2 delta = {0, 0};
    if (Util::Input::IsKeyPressed(Util::Keycode::UP)) delta.y -= speed;
    if (Util::Input::IsKeyPressed(Util::Keycode::DOWN)) delta.y += speed;
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) delta.x -= speed;
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) delta.x += speed;

    m_BodyPos += delta;
    auto areaMin = Util::GameFieldToScreen(PLAY_AREA_FIELD_LEFT, PLAY_AREA_FIELD_TOP);
    auto areaMax = Util::GameFieldToScreen(PLAY_AREA_FIELD_RIGHT, PLAY_AREA_FIELD_BOTTOM);
    m_BodyPos.x  = glm::clamp(m_BodyPos.x, areaMin.x, areaMax.x);
    m_BodyPos.y  = glm::clamp(m_BodyPos.y, areaMin.y, areaMax.y);

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
            SetMoveScript(PlayerMovementScript::Idle);  // resets flipX, switches to Script 0
        }
    }

    // ── Sync position to VM ───────────────────────────────────────────────────
    m_BodyVm->pos = m_BodyPos;
}
void Player::HandlePlayerInput() {
    if (m_PlayerState == PlayerState::DEAD || m_PlayerState == PlayerState::SPAWNING) return;
    HandleMovement();
    m_IsFocus = Util::Input::IsKeyPressed(Util::Keycode::LSHIFT) ? true : false;

    if (Util::Input::IsKeyPressed(Util::Keycode::Z)) {
        // Initzialize bullet firing timer
        if (m_FireBulletTimer < 0) {
            m_FireBulletTimer = 0;
        }
    }
}

void Player::UpdateFireBulletsTimer() {
    if (m_FireBulletTimer >= 0) {  // TODO: add spell card specific fire rule
        SpawnBullets();
        m_FireBulletTimer++;
    }

    if (m_FireBulletTimer >= 30 || m_PlayerState == PlayerState::DEAD ||
        m_PlayerState == PlayerState::SPAWNING) {  // TODO: add spell card specific fire rule
        m_FireBulletTimer = -1;
    }
}

void Player::SpawnBullets() {
    FireBulletResult result;
    int              curBulletIdx = 0;
    bool             fireIsDone   = false;
    for (int i = 0; i < 100; i++) {
        PlayerBullet* currBullet = &m_Bullets[i];

        if (currBullet->m_BulletState == BulletState::UNUSED) {
            while (1) {
                result = m_FireBulletCallback(currBullet, curBulletIdx);

                if (result == FireBulletResult::FIRED ||
                    result == FireBulletResult::FIRED_AND_LAST) {
                    currBullet->m_BulletState = BulletState::FIRED;
                    m_Renderer.AddChild(currBullet->m_Vm.obj);
                    curBulletIdx++;
                    if (result == FireBulletResult::FIRED_AND_LAST) {
                        fireIsDone = true;
                    }
                    break;
                } else if (result == FireBulletResult::CONTINUE_SPAWNING) {
                    curBulletIdx++;
                    continue;
                } else if (result == FireBulletResult::END_SPAWNING) {
                    fireIsDone = true;
                    break;
                }
            }
        }
        if (fireIsDone) break;
    }
}

FireBulletResult Player::FireSingleBullet(PlayerBullet* bullet, int bulletIdx,
                                          const CharacterPowerData* powerData) {
    while (this->m_Power >= powerData->m_Power) {
        powerData++;  // Pushes pointer to next power level
    }

    const CharacterPowerBulletData* bulletData = powerData->m_Bullets + bulletIdx;

    if (bulletData->m_BulletType == BulletType::LASER) {
        for (auto& activeBullet : m_Bullets) {
            if (activeBullet.m_BulletState != BulletState::FIRED ||
                activeBullet.m_BulletType != BulletType::LASER ||
                activeBullet.m_LaserSpawnPositionIdx != bulletData->m_SpawnPositionIdx) {
                continue;
            }

            activeBullet.m_Damage     = bulletData->m_Damage;
            activeBullet.m_Size.x     = bulletData->m_Size.x;
            activeBullet.m_LaserOffset = bulletData->m_SpawnOffset;
            activeBullet.m_AliveTimer = PLAYER_LASER_HOLD_FRAMES;
            return bulletIdx + 1 >= powerData->m_NumBullets ? FireBulletResult::END_SPAWNING
                                                            : FireBulletResult::CONTINUE_SPAWNING;
        }

        if (m_FireBulletTimer % bulletData->m_BulletsInterval == bulletData->m_FireBulletOffset) {
            *bullet = PlayerBullet{};
            m_Anm.SetScript(bullet->m_Vm, bulletData->m_ScriptIdx, bulletData->m_SpriteOffset);
            bullet->m_Size                 = {bulletData->m_Size.x, 1.0f};
            bullet->m_Damage               = bulletData->m_Damage;
            bullet->m_BulletType           = BulletType::LASER;
            bullet->m_LaserOffset          = bulletData->m_SpawnOffset;
            bullet->m_LaserSpawnPositionIdx = bulletData->m_SpawnPositionIdx;
            bullet->m_AliveTimer           = PLAYER_LASER_HOLD_FRAMES;
            return bulletIdx + 1 >= powerData->m_NumBullets ? FireBulletResult::FIRED_AND_LAST
                                                            : FireBulletResult::FIRED;
        }
    } else if (m_FireBulletTimer % bulletData->m_BulletsInterval ==
               bulletData->m_FireBulletOffset) {
        *bullet = PlayerBullet{};
        m_Anm.SetScript(bullet->m_Vm, bulletData->m_ScriptIdx, bulletData->m_SpriteOffset);

        if (!bulletData->m_SpawnPositionIdx) {  // spawn from body
            bullet->m_Vm.pos = this->m_BodyPos + bulletData->m_SpawnOffset;
        } else {  // spawn from orb
            bullet->m_Vm.pos =
                this->m_OrbVms[bulletData->m_SpawnPositionIdx - 1]->pos + bulletData->m_SpawnOffset;
        }
        bullet->m_Size       = bulletData->m_Size;
        bullet->m_Speed      = bulletData->m_Speed;
        bullet->m_Angle      = bulletData->m_Angle;
        bullet->m_Damage     = bulletData->m_Damage;
        bullet->m_BulletType = bulletData->m_BulletType;
        bullet->m_Velocity   = {bullet->m_Speed * cos(bullet->m_Angle),
                                bullet->m_Speed * sin(bullet->m_Angle)};
        return bulletIdx + 1 >= powerData->m_NumBullets ? FireBulletResult::FIRED_AND_LAST
                                                        : FireBulletResult::FIRED;
    }

    return bulletIdx + 1 >= powerData->m_NumBullets ? FireBulletResult::END_SPAWNING
                                                    : FireBulletResult::CONTINUE_SPAWNING;
}

void Player::UpdatePlayerBullets() {
    PlayerBullet* bullet;
    // TODO: update laser timer

    for (int i = 0; i < 100; i++) {
        bullet = &m_Bullets[i];

        if (bullet->m_BulletState == BulletState::COLLIDED) {
            bullet->m_BulletState = BulletState::UNUSED;
            m_Renderer.RemoveChild(bullet->m_Vm.obj);
        }

        if (bullet->m_BulletState != BulletState::UNUSED) {
            switch (bullet->m_BulletType) {
                case BulletType::HOMING:
                    break;
                case BulletType::PIERCE:
                    break;
                case BulletType::LASER:
                    bullet->m_AliveTimer--;
                    if (bullet->m_AliveTimer < 0) {
                        bullet->m_BulletState = BulletState::UNUSED;
                        m_Renderer.RemoveChild(bullet->m_Vm.obj);
                        break;
                    }

                    if (bullet->m_LaserSpawnPositionIdx == 0) {
                        bullet->m_Vm.pos = this->m_BodyPos + bullet->m_LaserOffset;
                    } else {
                        bullet->m_Vm.pos = this->m_OrbVms[bullet->m_LaserSpawnPositionIdx - 1]->pos +
                                           bullet->m_LaserOffset;
                    }

                    bullet->m_Size.y = std::min(PLAYER_LASER_DEFAULT_LENGTH,
                                                bullet->m_Size.y + PLAYER_LASER_GROWTH_RATE);
                    bullet->m_Vm.pos.y -= bullet->m_Size.y * 0.5f;
                    bullet->m_Vm.rotation = 0.0f;
                    bullet->m_Vm.scale    = {bullet->m_Size.x / 32.0f, bullet->m_Size.y / 32.0f};
                    break;
                case BulletType::STRAIGHT:
                    break;
            }

            if (bullet->m_BulletType != BulletType::LASER) {
                bullet->m_Vm.pos += bullet->m_Velocity;
            }
            m_Anm.UpdateObjects(bullet->m_Vm);
            if (bullet->m_BulletType != BulletType::LASER &&
                !Util::IsInGameBounds(bullet->m_Vm.pos.x, bullet->m_Vm.pos.y, bullet->m_Size.x,
                                      bullet->m_Size.y)) {
                bullet->m_BulletState = BulletState::UNUSED;
                m_Renderer.RemoveChild(bullet->m_Vm.obj);
            }
        }
    }
}
