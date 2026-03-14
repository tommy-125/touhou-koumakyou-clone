#ifndef ANMDEFS_HPP
#define ANMDEFS_HPP
namespace Anm {

struct Entry {
    const char *folder;
    const char *txt;
    int         offset;
};

constexpr Entry TITLE01 = { // Main menu / option (Gray)
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01/title01.txt",
    0,
};

constexpr Entry TITLE01S = { // Main menu / option (Red)
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01s",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01s/title01s.txt",
    40,
};

constexpr Entry TITLE02 = { // Title logo
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02/title02.txt",
    80,
};

constexpr Entry TITLE03 = { // Title logo
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title03",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title03/title03.txt",
    90,
};

constexpr Entry SELECT01 = { // Difficulty select title / Normal / Easy 
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select01",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select01/select01.txt",
    95,
};

constexpr Entry SELECT02 = { // Hard / Lunatic
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select02",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select02/select02.txt",
    100,
};

constexpr Entry SELECT03 = { // Character select title / Spell card select title
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select03",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select03/select03.txt",
    105,
};

constexpr Entry SELECT04 = { // Spell cards
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select04",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select04/select04.txt",
    110,
};

constexpr Entry SELECT05 = { // Extra
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select05",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/select05/select05.txt",
    115,
};

constexpr Entry SLPL00A = { // Upper part of Reimu's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00a",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00a/slpl00a.txt",
    120,
};

constexpr Entry SLPL00B = { // Lower part of Reimu's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00b",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl00b/slpl00b.txt",
    125,
};

constexpr Entry SLPL01A = { // Upper part of Marisa's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01a",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01a/slpl01a.txt",
    130,
};

constexpr Entry SLPL01B = { // Lower part of Marisa's sprite art
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01b",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/slpl01b/slpl01b.txt",
    135,
};

} // namespace Anm

#endif // ANMDEFS_HPP