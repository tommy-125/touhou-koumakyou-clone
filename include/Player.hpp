#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Anm/AnmTypes.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include "Util/Math.hpp"

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

enum class CharacterItem {
    Reimu,
    Marisa,
};

enum class SpellCardItem {
    Shot_Type_A,
    Shot_Type_B,
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

enum class BulletType
{
    STRAIGHT,
    HOMING,
    PIERCE,
    LASER
};

enum class BulletState
{
    UNUSED,
    FIRED,
    COLLIDED,
};

enum class PlayerState
{
    ALIVE,
    SPAWNING,
    DEAD,
    INVULNERABLE,
};

struct CharacterPowerBulletData { // Bullet data for one bullet
    int m_WaitBetweenBullets;
    int m_BulletFrame;
    glm::vec2 m_motion;
    glm::vec2 m_size;
    float m_Direction;
    float m_Velocity;
    int m_Damage;
    int m_SpawnPositionIdx;
    BulletType m_BulletType;
    int m_AnmFileIdx;
    // int m_BulletSoundIdx; 
};

struct CharacterPowerData { // Record bullet data depending on power level

    int m_NumBullets;
    int m_Power;
    const CharacterPowerBulletData *m_Bullets;
};


// th06 screen coordinates (origin top-left, y-down)
constexpr float PLAYER_SPAWN_X  = 192.0f;
constexpr float PLAYER_SPAWN_Y  = 384.0f;
constexpr float PLAY_AREA_LEFT  = 32.0f;
constexpr float PLAY_AREA_RIGHT = 416.0f;
constexpr float PLAY_AREA_TOP   = 16.0f;
constexpr float PLAY_AREA_BOTTOM = 464.0f;

struct PlayerData {
    float              m_OrthogonalMovementSpeed;
    float              m_OrthogonalMovementSpeedFocus;
    float              m_DiagonalMovementSpeed;
    float              m_DiagonalMovementSpeedFocus;
    const Anm::Entry  *m_AnmEntry;
};

struct PlayerBullet {
    Anm::Vm m_Sprite;
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    glm::vec2 m_Velocity;
    float m_SidewaysMotion;
    int m_Damage;
    BulletState m_BulletState;
    BulletType m_BulletType;
    int m_SpawnPositionIdx;
};


constexpr PlayerData REIMU_DATA  = { 4.0f, 2.0f, 4.0f * INV_SQRT2, 2.0f * INV_SQRT2, &Anm::PLAYER00 };
constexpr PlayerData MARISA_DATA = { 5.0f, 2.5f, 5.0f * INV_SQRT2, 2.5f * INV_SQRT2, &Anm::PLAYER01 };

class Player {
public:
    Player(CharacterItem character, int spellCardIdx);
    void Update();

private:
    void SetMoveState(MoveState newState);
    void SetMoveScript(PlayerScript script);
    void HandlePlayerInput();
    void HandleMovement();
    void HandleStateMachine();
    const PlayerData *m_Data;
    int                  m_SpellCardIdx;
    MoveState            m_MoveState       = MoveState::Idle;
    int                  m_ReturnFramesLeft = 0;
    glm::vec2            m_BodyPos   = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y};
    glm::vec2            m_HitboxTopLeft;
    glm::vec2            m_HitboxBottomRight;
    glm::vec2            m_GrabItemTopLeft;
    glm::vec2            m_GrabItemBottomRight;
    glm::vec2            m_HitboxSize;
    glm::vec2            m_GrabItemSize;

    Anm::Manager  m_Anm;

    std::vector<Anm::Vm>                           m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    Anm::Vm*      m_BodyVm = nullptr;

    Util::Renderer m_Renderer;
};

#endif // PLAYER_HPP
