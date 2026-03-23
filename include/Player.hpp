#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Anm/AnmTypes.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

enum class CharacterItem {
    Reimu,
    Marisa,
};

enum class ReimuSpellCardItem {
    HomingAmulet,
    PersuasionNeedle,
};

enum class MarisaSpellCardItem {
    MagicMissile,
    IllusionLaser,
};

enum class MoveState {
    Idle,
    MoveRight,
    MoveLeft,
};

enum class PlayerScript {
    Idle            = 0,
    MoveLeft        = 1,
    ReturnFromLeft  = 2,
    MoveRight       = 3,
    ReturnFromRight = 4,
};

enum class ShotType {
    A,
    B,
};

enum BulletType
{
    STRAIGHT,
    HOMING,
    PIERCE,
    LASER
};

enum PlayerState
{
    ALIVE,
    SPAWNING,
    DEAD,
    INVULNERABLE,
};


// th06 screen coordinates (origin top-left, y-down)
constexpr float PLAYER_SPAWN_X  = 192.0f;
constexpr float PLAYER_SPAWN_Y  = 384.0f;
constexpr float PLAY_AREA_LEFT  = 32.0f;
constexpr float PLAY_AREA_RIGHT = 416.0f;
constexpr float PLAY_AREA_TOP   = 16.0f;
constexpr float PLAY_AREA_BOTTOM = 464.0f;

struct PlayerData {
    float              speed;
    float              speedFocused;
    const Anm::Entry  *anmEntry;
};

constexpr PlayerData REIMU_DATA  = { 4.0f, 2.0f,  &Anm::PLAYER00 };
constexpr PlayerData MARISA_DATA = { 5.0f, 2.5f,  &Anm::PLAYER01 };

class Player {
public:
    Player(CharacterItem character, int spellCardIdx);
    void Update();

private:
    void SetMoveState(MoveState newState);
    void SetScript(PlayerScript script);

    const PlayerData *m_Data;
    int                  m_SpellCardIdx;
    MoveState            m_MoveState       = MoveState::Idle;
    int                  m_ReturnFramesLeft = 0;
    glm::vec2            m_BodyPos   = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y};

    Anm::Manager  m_Anm;

    std::vector<Anm::Vm>                           m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    Anm::Vm*      m_BodyVm = nullptr;

    Util::Renderer m_Renderer;
};

#endif // PLAYER_HPP
