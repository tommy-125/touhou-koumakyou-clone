#include "Anm/AnmManager.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
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


void Manager::LoadAnm(const std::string &spriteFolder,
                      const std::string &txtPath,
                      int spriteIdxOffset) {
    // Open the ANM txt file
    std::ifstream file(txtPath);
    if (!file.is_open()) {
        LOG_ERROR("AnmManager: cannot open {}", txtPath);
        return;
    }

    int currentScriptId = -1;

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
            scripts[currentScriptId].instrs.emplace_back(std::move(instr));
            continue;
        }
    }
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

void Manager::ExecuteScript(Vm &vm) {
    if (vm.scriptIdx < 0) return;
    const Script &script = scripts[vm.scriptIdx];

    // Resume from interrupt if stopped
    if (vm.isStopped) {

        // if there is no pending interrupt, skip to update position interpolation
        if (vm.pendingInterrupt == 0) goto update_interp;

        // search for the interrupt label in the script
        for (int i = 0; i < (int)script.instrs.size(); i++) {
            const Instr &instr = script.instrs[i];

            if (instr.opcode == InterruptLabel &&
                !instr.args.empty() &&
                (int)instr.args[0] == vm.pendingInterrupt) { // found the interrupt label
                vm.instrIdx    = i;
                vm.currentTime = instr.time;
                vm.isStopped   = false;
                vm.pendingInterrupt = 0;
                break;
            }
        }
        if (vm.isStopped) goto update_interp; // label not found
    }

    // Dispatch instructions whose time has come
    while (vm.instrIdx < (int)script.instrs.size()) {
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
            vm.isStopped = true;
            goto update_interp;

        case StopHide:
            vm.isVisible = false;
            vm.isStopped = true;
            goto update_interp;

        case SetActiveSprite:
            if (!instr.args.empty()) {
                vm.spriteIdx = (int)instr.args[0] + vm.spriteOffset;
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
                vm.posInterpDuration = (int)instr.args[3];
                vm.posInterpTimer   = 0;
            }
            break;

        case InterruptLabel:
            // nop — only meaningful as a jump target
            break;

        case SetAlpha:
            if (!instr.args.empty())
                vm.alpha = instr.args[0] / 255.0f;
            break;

        case SetScale:
            if (instr.args.size() >= 2)
                vm.scale = {instr.args[0], instr.args[1]};
            break;

        case SetRotation:
            if (!instr.args.empty())
                vm.rotation = instr.args[0];
            break;

        case SetBlendAdditive:
        case SetBlendDefault:
        case SetZWriteDisable:
        case AnchorTopLeft:
        case SetAutoRotate:
        case Nop:
        default:
            break;
        }
    }

    vm.currentTime++;

update_interp:
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

void Manager::SendInterrupt(Vm &vm, int interrupt) {
    vm.pendingInterrupt = interrupt;
}

glm::vec2 Manager::ToPtsd(glm::vec2 th06pos) {
    return {th06pos.x - 320.0f, 240.0f - th06pos.y};
}

} // namespace Anm
