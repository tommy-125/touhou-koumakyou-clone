#ifndef ANMDEFS_HPP
#define ANMDEFS_HPP
namespace Anm {

struct Entry {
    const char *folder;
    const char *txt;
    int         offset;
};

// offset = previous offset + previous script count
constexpr Entry TITLE01 = { // Main menu / option (Gray)
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01/title01.txt",
    0,
};

constexpr Entry TITLE01S = { // Main menu / option (Red)
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01s",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01s/title01s.txt",
    0+40,
};

constexpr Entry TITLE02 = { // Title logo
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02/title02.txt",
    40+40,
};

constexpr Entry TITLE03 = { // Title logo
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title03",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title03/title03.txt",
    80+10,
};

constexpr Entry SELECT01 = { // Difficulty select title / Normal / Easy 
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select01",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select01/select01.txt",
    90+5,
};

constexpr Entry SELECT02 = { // Hard / Lunatic
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select02",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select02/select02.txt",
    95+5,
};

constexpr Entry SELECT03 = { // Character select title / Spell card select title
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select03",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select03/select03.txt",
    100+5,
};

constexpr Entry SELECT04 = { // Spell cards
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select04",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select04/select04.txt",
    105+5,
};

constexpr Entry SELECT05 = { // Extra
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select05",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select05/select05.txt",
    110+5,
};

constexpr Entry SLPL00A = { // Upper part of Reimu's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00a",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00a/slpl00a.txt",
    115+5,
};

constexpr Entry SLPL00B = { // Lower part of Reimu's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00b",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00b/slpl00b.txt",
    120+5,
};

constexpr Entry SLPL01A = { // Upper part of Marisa's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01a",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01a/slpl01a.txt",
    125+5,
};

constexpr Entry SLPL01B = { // Lower part of Marisa's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01b",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01b/slpl01b.txt",
    130+5,
};


constexpr Entry PLAYER00 = { // Reimu sprite
    GA_RESOURCE_DIR "/th06c/th06c_CM_output/player00",
    GA_RESOURCE_DIR "/th06c/th06c_CM_output/player00/player00.txt",
    135+5,
};

constexpr Entry PLAYER01 = { // Marisa sprite
    GA_RESOURCE_DIR "/th06c/th06c_CM_output/player01",
    GA_RESOURCE_DIR "/th06c/th06c_CM_output/player01/player01.txt",
    140+200,
};

constexpr int SCRIPT_REIMU_LEFT_ORB = (PLAYER00.offset + 128);
constexpr int SCRIPT_REIMU_RIGHT_ORB = (PLAYER00.offset + 129);

constexpr int SCRIPT_PLAYER_BULLET = (PLAYER00.offset + 64); // Basic player bullet script for both character

constexpr int SCRIPT_PLAYER_REIMU_A_ORB_BULLET = (PLAYER00.offset + 65);
constexpr int SCRIPT_PLAYER_REIMU_A_ORB_BULLET_COLLISION = (PLAYER00.offset + 97);
constexpr int SCRIPT_PLAYER_REIMU_B_ORB_BULLET = (PLAYER00.offset + 66);
constexpr int SCRIPT_PLAYER_REIMU_B_ORB_BULLET_COLLISION = (PLAYER00.offset + 98);
constexpr int SCRIPT_PLAYER_REIMU_BULLET_COLLISION = (PLAYER00.offset + 96);

// ── Stage 1 enemy sprites ────────────────────────────────────────────────────
// stg1enm (small/medium fairies) and stg1enm2 (boss Rumia) share the same offset
// because their sprite/script IDs don't overlap (enm: 0-23, enm2: 128-138)
constexpr Entry STG1ENM = {
    GA_RESOURCE_DIR "/th06c/th06c_ST_output/stg1enm",
    GA_RESOURCE_DIR "/th06c/th06c_ST_output/stg1enm/stg1enm.txt",
    340+200, // 540
};

constexpr Entry STG1ENM2 = { // Boss Rumia sprites (scripts 128+)
    GA_RESOURCE_DIR "/th06c/th06c_ST_output/stg1enm2",
    GA_RESOURCE_DIR "/th06c/th06c_ST_output/stg1enm2/stg1enm2.txt",
    340+200, // 540 (same offset as STG1ENM)
};

// ── Enemy bullet sprites ─────────────────────────────────────────────────────
constexpr Entry ETAMA3 = {
    GA_RESOURCE_DIR "/th06c/th06c_CM_output/etama3",
    GA_RESOURCE_DIR "/th06c/th06c_CM_output/etama3/etama3.txt",
    540+200, // 740
};

} // namespace Anm

#endif // ANMDEFS_HPP