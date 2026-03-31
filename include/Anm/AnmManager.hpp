#ifndef ANM_MANAGER_HPP
#define ANM_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmTypes.hpp"
namespace Anm {

struct LoadedEntry {
    const Entry* entry;
    int          scriptCount;
};

class Manager {
   public:
    static constexpr int MAX_ENTRIES = 2048;

    // 全域共用
    std::array<Sprite, MAX_ENTRIES> sprites;
    std::array<Script, MAX_ENTRIES> scripts;

    /**
     * Load an ANM txt file.
     * @param spriteFolder  folder containing sprite_0.png, sprite_1.png, ...
     * @param txtPath       path to the decompiled .anm txt file
     * @param idxOffset  global index base for this entry
     */
    /** Returns the number of scripts loaded for the number of VMs and the offset. */
    int LoadAnm(const std::string& spriteFolder, const std::string& txtPath, int idxOffset);

    /** Set a script on a VM and immediately execute its time=0 instructions. */
    void SetScript(Vm& vm, int globalScriptIdx, int spriteOffset);

    /** Run one frame of script for a VM. Call once per frame per object. */
    void ExecuteScript(Vm& vm);

    /** Execute scripts and sync all VM states to their corresponding GameObjects. */
    void UpdateObjects(std::vector<Vm>& vms);
    void UpdateObjects(Vm& vm);

    /**
     * Send an interrupt to a VM.
     * If the VM is stopped, it will resume from InterruptLabel(interrupt)
     * on the next ExecuteScript call.
     */
    void SendInterrupt(Vm& vm, int interrupt);

    /** Convert th06 screen coords to PTSD world coords.
     *  th06: (0,0)=top-left, y-down, 640x480
     *  PTSD: (0,0)=center,   y-up,   pixel units */
    static glm::vec2 ToPtsd(glm::vec2 th06pos);
};

}  // namespace Anm

#endif  // ANM_MANAGER_HPP