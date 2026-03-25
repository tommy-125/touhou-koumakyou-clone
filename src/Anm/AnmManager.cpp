#include "Anm/AnmManager.hpp"

#include <fstream>
#include <memory>
#include <vector>

#include <sstream>
#include <string>

#include <glm/glm.hpp>

#include "Util/Logger.hpp"

namespace Anm {

// ── Parsing helpers ───────────────────────────────────────────────────────────
// Take parameter out of the ANM txt file, and covert to float
// Ex: "1.0f" -> 1.0, "100" -> 100.0
static float ParseArg(const std::string &token) {
    std::string s = token;
    if (!s.empty() && s.back() == 'f') s.pop_back();
    return std::stof(s);
}


int Manager::LoadAnm(const std::string &spriteFolder,
                     const std::string &txtPath,
                     int spriteIdxOffset) {
    // Open the ANM txt file
    std::ifstream file(txtPath);
    if (!file.is_open()) {
        LOG_ERROR("AnmManager: cannot open {}", txtPath);
        return 0;
    }

    int currentScriptId  = -1;
    int scriptCount      = 0;
    int scriptByteOffset = 0;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Sprite: N W*H+X+Y
        if (line.rfind("Sprite: ", 0) == 0) {
            // Take the parameters out of the line
            std::istringstream ss(line.substr(8));

            int id;
            char sep1, sep2, sep3;
            float w, h, x, y;
            // format: id W*H+X+Y
            ss >> id >> w >> sep1 >> h >> sep2 >> x >> sep3 >> y;

            // give global index
            int globalId = id + spriteIdxOffset;

            // load the sprite image
            std::string path = spriteFolder + "/sprite_" + std::to_string(id) + ".png";
            sprites[globalId].image  = std::make_shared<Util::Image>(path);
            sprites[globalId].width  = w;
            sprites[globalId].height = h;
            continue;
        }

        // Script: N
        if (line.rfind("Script: ", 0) == 0) {
            currentScriptId = std::stoi(line.substr(8)) + spriteIdxOffset;
            scripts[currentScriptId] = Script{};
            scriptCount++;
            scriptByteOffset = 0;
            continue;
        }

        // Instruction #N: time unk opcode [args...]
        if (line.rfind("Instruction #", 0) == 0 && currentScriptId >= 0) {
            auto colonPos = line.find(':');
            if (colonPos == std::string::npos) continue;

            std::istringstream ss(line.substr(colonPos + 1));
            Instr instr;
            int unk;
            ss >> instr.time >> unk >> instr.opcode;

            std::string token;
            while (ss >> token) {
                try {
                    instr.args.emplace_back(ParseArg(token));
                } catch (...) {}
            }
            instr.byteOffset  = scriptByteOffset;
            scriptByteOffset += 4 + static_cast<int>(instr.args.size()) * 4;
            scripts[currentScriptId].instrs.emplace_back(std::move(instr));
            continue;
        }
    }
    return scriptCount;
}

// ── Script execution ──────────────────────────────────────────────────────────

static float Easing(float t, int mode) {
    t = glm::clamp(t, 0.0f, 1.0f);

    switch (mode) {
        case 1: return 1.0f - (1.0f - t) * (1.0f - t); // decel (ease-out)
        case 2: return t * t;                            // accel (ease-in)
        default: return t;                               // linear
    }
}

void Manager::SetScript(Vm &vm, int globalScriptIdx, int spriteOffset) { // set script and immediately execute time=0 instructions to set initial state
    vm.scriptIdx    = globalScriptIdx;
    vm.spriteOffset = spriteOffset;
    vm.instrIdx     = 0;
    vm.currentTime  = 0;
    vm.isStopped    = false;
    ExecuteScript(vm); // immediately execute for initial state
}

// Execute sequence: handle interrupts -> if stopped, do nothing -> otherwise, execute instructions whose time has come -> update interpolation
void Manager::ExecuteScript(Vm &vm) {
    if (vm.scriptIdx < 0) return;
    const Script &script = scripts[vm.scriptIdx];

    // ── Interrupt handling ────────────────────────────────────────────────────
    if (vm.pendingInterrupt != 0) {
        int target = -1;

        for (int i = 0; i < static_cast<int>(script.instrs.size()); i++) {
            const Instr &label = script.instrs[i];
            if (label.opcode == InterruptLabel &&
                !label.args.empty() &&
                static_cast<int>(label.args[0]) == vm.pendingInterrupt) {
                target = i;
                break;
            }
        }
        if (target >= 0) {
            vm.instrIdx         = target;
            vm.currentTime      = script.instrs[target].time;
            vm.isStopped        = false;
            vm.isVisible        = true;
            vm.pendingInterrupt = 0;
        } else {
            vm.isStopped        = true;
            vm.pendingInterrupt = 0;
            goto update_interp;
        }
    } else if (vm.isStopped) {
        goto update_interp;
    }

    // Dispatch instructions whose time has come
    while (vm.instrIdx < static_cast<int>(script.instrs.size())) {
        const Instr &instr = script.instrs[vm.instrIdx];

        if (instr.time > vm.currentTime) break;

        vm.instrIdx++;

        switch (instr.opcode) {
        case Exit:
            vm.scriptIdx = -1;
            return;

        case ExitHide:
            vm.scriptIdx = -1;
            vm.isVisible = false;
            return;

        case Stop:
        case StopHide:
            if (instr.opcode == StopHide) vm.isVisible = false;
            vm.isStopped = true;

            if (vm.pendingInterrupt != 0) {
                for (int j = 0; j < static_cast<int>(script.instrs.size()); j++) { // search for the interrupt label
                    const Instr &label = script.instrs[j];
                    if (label.opcode == InterruptLabel &&
                        !label.args.empty() &&
                        static_cast<int>(label.args[0]) == vm.pendingInterrupt) {
                        vm.instrIdx         = j;
                        vm.currentTime      = label.time;
                        vm.isStopped        = false;
                        vm.isVisible        = true;
                        vm.pendingInterrupt = 0;
                        break;
                    }
                }
            }
            if (vm.isStopped) goto update_interp;
            continue; // resume dispatch from new instrIdx

        case SetActiveSprite:
            if (!instr.args.empty()) {
                vm.spriteIdx = static_cast<int>(instr.args[0]) + vm.spriteOffset;
                vm.isVisible = true;
            }
            break;

        case SetPosition:
            if (instr.args.size() >= 2)
                vm.pos = {instr.args[0], instr.args[1]};
            break;

        case PosTimeLinear:
        case PosTimeDecel:
        case PosTimeAccel:
            if (instr.args.size() >= 4) {
                vm.posInterp        = true;
                vm.posInterpMode    = instr.opcode - PosTimeLinear;
                vm.posInterpStart   = vm.pos;
                vm.posInterpEnd     = {instr.args[0], instr.args[1]};
                vm.posInterpDuration = static_cast<int>(instr.args[3]);
                vm.posInterpTimer   = 0;
            }
            break;

        case Jump:
            if (!instr.args.empty()) {
                int targetOffset = static_cast<int>(instr.args[0]);

                for (int j = 0; j < static_cast<int>(script.instrs.size()); j++) {
                    if (script.instrs[j].byteOffset == targetOffset) {
                        vm.instrIdx    = j;
                        vm.currentTime = script.instrs[j].time;
                        break;
                    }
                }
                continue;
            }
            break;

        case InterruptLabel:
            // nop — only meaningful as a jump target
            break;

        case SetAlpha:
            if (!instr.args.empty()) {
                vm.alpha      = instr.args[0] / 255.0f;
                vm.fadeInterp = false;
            }
            break;

        case Fade:
            if (instr.args.size() >= 2) {
                vm.fadeInterp    = true;
                vm.fadeStart     = vm.alpha;
                vm.fadeTarget    = instr.args[0] / 255.0f;
                vm.fadeDuration  = static_cast<int>(instr.args[1]);
                vm.fadeTimer     = 0;
            }
            break;

        case SetAngleVel:
            if (instr.args.size() >= 3)
                vm.angleVel = instr.args[2];
            break;

        case SetScale:
            if (instr.args.size() >= 2)
                vm.scale = {instr.args[0], instr.args[1]};
            break;

        case SetRotation:
            if (instr.args.size() >= 3)
                vm.rotation = instr.args[2];
            break;

        case AnchorTopLeft:
            vm.anchorTopLeft = true;
            break;

        case ScaleTime:
            if (instr.args.size() >= 3) {
                vm.scaleInterp         = true;
                vm.scaleInterpStart    = vm.scale;
                vm.scaleInterpEnd      = {instr.args[0], instr.args[1]};
                vm.scaleInterpDuration = static_cast<int>(instr.args[2]);
                vm.scaleInterpTimer    = 0;
            }
            break;

        case FlipX:
            vm.flipX = !vm.flipX;
            break;

        case FlipY:
            vm.flipY = !vm.flipY;
            break;

        case SetScaleSpeed:
            if (instr.args.size() >= 2)
                vm.scaleSpeed = {instr.args[0], instr.args[1]};
            break;

        case SetBlendAdditive:
        case SetBlendDefault:
        case SetZWriteDisable:
        case SetAutoRotate:
        case Nop:
        default:
            break;
        }
    }

    vm.currentTime++;

update_interp:
    // Angle velocity
    vm.rotation += vm.angleVel;

    // Scale velocity
    vm.scale += vm.scaleSpeed;

    // Alpha fade interpolation
    if (vm.fadeInterp) {
        float t = (vm.fadeDuration > 0)
                      ? (float)vm.fadeTimer / vm.fadeDuration
                      : 1.0f;
        vm.alpha = glm::mix(vm.fadeStart, vm.fadeTarget, t);
        vm.fadeTimer++;
        if (vm.fadeTimer > vm.fadeDuration) {
            vm.fadeInterp = false;
            vm.alpha      = vm.fadeTarget;
        }
    }

    // Scale interpolation
    if (vm.scaleInterp) {
        float t = (vm.scaleInterpDuration > 0)
                      ? (float)vm.scaleInterpTimer / vm.scaleInterpDuration
                      : 1.0f;
        vm.scale = glm::mix(vm.scaleInterpStart, vm.scaleInterpEnd, t);
        vm.scaleInterpTimer++;
        if (vm.scaleInterpTimer > vm.scaleInterpDuration) {
            vm.scaleInterp = false;
            vm.scale       = vm.scaleInterpEnd;
        }
    }

    // Position interpolation (runs even when script is stopped mid-tween)
    if (vm.posInterp) {
        float t = (vm.posInterpDuration > 0)
                      ? (float)vm.posInterpTimer / vm.posInterpDuration
                      : 1.0f;
        vm.pos = glm::mix(vm.posInterpStart, vm.posInterpEnd, Easing(t, vm.posInterpMode));
        vm.posInterpTimer++;
        if (vm.posInterpTimer > vm.posInterpDuration) {
            vm.posInterp = false;
            vm.pos       = vm.posInterpEnd;
        }
    }
}

void Manager::UpdateObjects(std::vector<Vm> &vms) {
    for (int i = 0; i < static_cast<int>(vms.size()); i++) {
        ExecuteScript(vms[i]);

        const auto &vm  = vms[i];
        if (!vm.obj) continue;
        auto       &obj = *vm.obj;

        obj.SetVisible(vm.isVisible);
        obj.SetAlpha(vm.alpha);
        obj.SetZIndex(vm.zIndex);

        if (vm.spriteIdx >= 0 && sprites[vm.spriteIdx].image) {
            obj.SetDrawable(sprites[vm.spriteIdx].image);
        }

        glm::vec2 translation = ToPtsd(vm.pos);
        if (vm.anchorTopLeft) {
            const auto &spr = sprites[vm.spriteIdx];
            translation += glm::vec2{spr.width / 2.0f, -spr.height / 2.0f};
        }
        obj.m_Transform.translation = translation;
        obj.m_Transform.scale       = glm::vec2{
            vm.flipX ? -vm.scale.x : vm.scale.x,
            vm.flipY ? -vm.scale.y : vm.scale.y,
        };
        obj.m_Transform.rotation    = vm.rotation;
    }
}

void Manager::SendInterrupt(Vm &vm, int interrupt) {
    vm.pendingInterrupt = interrupt;
}

glm::vec2 Manager::ToPtsd(glm::vec2 th06pos) {
    return {th06pos.x - 320.0f, 240.0f - th06pos.y};
}

} // namespace Anm
