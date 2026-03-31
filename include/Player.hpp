#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Anm/AnmTypes.hpp"
#include "Util/Renderer.hpp"

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

enum class PlayerMovementScript {
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

enum class BulletType {
    STRAIGHT,
    HOMING,
    PIERCE,
    LASER
};

enum class BulletState {
    UNUSED,
    FIRED,
    COLLIDED,
};

enum class FireBulletResult {
    FIRED,
    CONTINUE_SPAWNING,
    END_SPAWNING,
    FIRED_AND_LAST,
};
enum class PlayerState {
    ALIVE,
    SPAWNING,
    DEAD,
    INVULNERABLE,
};

struct CharacterPowerBulletData {  // Bullet data for one bullet
    int        m_BulletsInterval;
    int        m_FireBulletOffset;
    glm::vec2  m_SpawnOffset;
    glm::vec2  m_Size;
    float      m_Angle;  // in radians
    float      m_Speed;
    int        m_Damage;
    int        m_SpawnPositionIdx;  // 0 for body, 1 for left orb, 2 for right orb
    BulletType m_BulletType;
    int        m_SpriteOffset;  // Base sprite offset for the ANM entry
    int        m_ScriptIdx;     // Global script index for this bullet
};

struct CharacterPowerData {  // Record bullet data depending on power level

    int                             m_NumBullets;
    int                             m_Power;
    const CharacterPowerBulletData* m_Bullets;
};

// th06 screen coordinates (origin top-left, y-down)
constexpr float PLAYER_SPAWN_X   = 192.0f;
constexpr float PLAYER_SPAWN_Y   = 384.0f;
constexpr float PLAY_AREA_LEFT   = 32.0f;
constexpr float PLAY_AREA_RIGHT  = 416.0f;
constexpr float PLAY_AREA_TOP    = 16.0f;
constexpr float PLAY_AREA_BOTTOM = 464.0f;

// player hitbox size
constexpr float PLAYER_HITBOX_X = 1.25f;
constexpr float PLAYER_HITBOX_Y = 1.25f;

// grab item size
constexpr float GRAB_ITEM_X = 12.0f;
constexpr float GRAB_ITEM_Y = 12.0f;

struct PlayerData {
    float             m_OrthogonalMovementSpeed;
    float             m_OrthogonalMovementSpeedFocus;
    float             m_DiagonalMovementSpeed;
    float             m_DiagonalMovementSpeedFocus;
    const Anm::Entry* m_AnmEntry;
};

struct PlayerBullet {
    Anm::Vm     m_Vm;
    glm::vec2   m_Size;
    float       m_Angle;  // in radians
    float       m_Speed;
    glm::vec2   m_Velocity;
    int         m_Damage;
    BulletState m_BulletState = BulletState::UNUSED;
    BulletType  m_BulletType;
    float       m_LaserOffset;
    int         m_LaserSpawnPositionIdx;
    int         m_AliveTimer;
};

constexpr float      INV_SQRT2   = 0.70711f;  // Inverse of SQRT(2)
constexpr PlayerData REIMU_DATA  = {4.0f, 2.0f, 4.0f * INV_SQRT2, 2.0f * INV_SQRT2, &Anm::PLAYER00};
constexpr PlayerData MARISA_DATA = {5.0f, 2.5f, 5.0f * INV_SQRT2, 2.5f * INV_SQRT2, &Anm::PLAYER01};

using FireBulletCallback = std::function<FireBulletResult(PlayerBullet* bullet, int bulletIdx)>;

class Player {
   public:
    Player(CharacterItem character, SpellCardItem spellCard);
    void             Update();
    const glm::vec2& GetPos() const { return m_BodyPos; }

   private:
    void SetMoveState(MoveState newState);
    void SetMoveScript(PlayerMovementScript script);
    void HandlePlayerInput();
    void HandleMovement();

    void             UpdateFireBulletsTimer();
    void             SpawnBullets();
    FireBulletResult FireSingleBullet(PlayerBullet* bullet, int bulletIdx,
                                      const CharacterPowerData* powerData);
    void             UpdatePlayerBullets();

    FireBulletCallback m_FireBulletCallback;

    const PlayerData* m_Data;
    int               m_Power = 0;

    CharacterItem m_Character;
    SpellCardItem m_SpellCard;

    PlayerState m_PlayerState      = PlayerState::ALIVE;
    MoveState   m_MoveState        = MoveState::Idle;
    int         m_ReturnFramesLeft = 0;
    glm::vec2   m_BodyPos          = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y};

    glm::vec2 m_HitboxTopLeft;
    glm::vec2 m_HitboxBottomRight;
    glm::vec2 m_HitboxSize = {PLAYER_HITBOX_X, PLAYER_HITBOX_Y};

    glm::vec2 m_GrabItemTopLeft;
    glm::vec2 m_GrabItemBottomRight;
    glm::vec2 m_GrabItemSize = {GRAB_ITEM_X, GRAB_ITEM_Y};

    bool     m_OrbVisible = false;
    Anm::Vm* m_OrbVms[2];

    bool m_IsFocus    = false;  // false for unfocused, true for focused
    int  m_FocusTimer = 0;  // counts frames since focus state changed, used for animating orbs

    int          m_FireBulletTimer = -1;
    PlayerBullet m_Bullets[100];  // Object pool for bullets

    Anm::Manager m_Anm;

    std::vector<Anm::Vm> m_Vms;

    Anm::Vm* m_BodyVm = nullptr;

    Util::Renderer m_Renderer;
};

#endif  // PLAYER_HPP
