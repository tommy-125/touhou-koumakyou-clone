#ifndef ANM_TYPES_HPP
#define ANM_TYPES_HPP

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Util/Image.hpp"

namespace Anm {

// ── Opcode constants ──
constexpr int Exit             = 0;
constexpr int SetActiveSprite  = 1;
constexpr int SetScale         = 2;
constexpr int SetAlpha         = 3;
constexpr int SetColor         = 4;
constexpr int Jump             = 5;
constexpr int Nop              = 6;
constexpr int FlipX            = 7;
constexpr int FlipY            = 8;
constexpr int SetRotation      = 9;
constexpr int SetAngleVel      = 10;
constexpr int SetScaleSpeed    = 11;
constexpr int Fade             = 12;
constexpr int SetBlendAdditive = 13;
constexpr int SetBlendDefault  = 14;
constexpr int ExitHide         = 15;
constexpr int SetRandomSprite  = 16;
constexpr int SetPosition      = 17;
constexpr int PosTimeLinear    = 18;
constexpr int PosTimeDecel     = 19;
constexpr int PosTimeAccel     = 20;
constexpr int Stop             = 21;
constexpr int InterruptLabel   = 22;
constexpr int AnchorTopLeft    = 23;
constexpr int StopHide         = 24;
constexpr int UsePosOffset     = 25;
constexpr int SetAutoRotate    = 26;
constexpr int UVScrollX        = 27;
constexpr int UVScrollY        = 28;
constexpr int SetVisibility    = 29;
constexpr int ScaleTime        = 30;
constexpr int SetZWriteDisable = 31;

// ── Data structures ───────────────────────────────────────────────────────────

struct Sprite {
    std::shared_ptr<Util::Image> image;
    float width  = 0;
    float height = 0;
};

struct Instr {
    int time   = 0;
    int opcode = 0;
    std::vector<float> args;
};

struct Script {
    std::vector<Instr> instrs;
};

// ── VM ────────────────────────────────────────────────────────────────────────

struct Vm {
    int scriptIdx   = -1;  // global index into AnmManager::scripts[]
    int instrIdx    = 0;
    int currentTime = 0;
    int spriteOffset = 0;  // spriteIdxOffset for this ANM entry

    int       spriteIdx = -1;      // global index into AnmManager::sprites[]
    glm::vec2 pos       = {0, 0};  // th06 coords: (0,0) = top-left, y-down
    float     rotation  = 0;
    glm::vec2 scale     = {1, 1};
    float     alpha     = 1.0f;
    bool      isVisible = false;
    bool      isStopped = false;
    int       pendingInterrupt = 0;

    // Position interpolation
    bool      posInterp        = false;
    int       posInterpMode    = 0;  // 0=linear 1=decel 2=accel
    glm::vec2 posInterpStart   = {0, 0};
    glm::vec2 posInterpEnd     = {0, 0};
    int       posInterpDuration = 0;
    int       posInterpTimer    = 0;
};

} // namespace Anm

#endif // ANM_TYPES_HPP