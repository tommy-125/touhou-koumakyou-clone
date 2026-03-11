#ifndef ANMDEFS_HPP
#define ANMDEFS_HPP
namespace Anm {

struct Entry {
    const char *folder;
    const char *txt;
    int         offset;
};

constexpr Entry TITLE01 = { // Main menu and option
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title01/title01.txt",
    0,
};

constexpr Entry TITLE01S = { // Main menu and option
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

} // namespace Anm

#endif // ANMDEFS_HPP