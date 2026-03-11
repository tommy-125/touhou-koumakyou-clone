#ifndef ANMDEFS_HPP
#define ANMDEFS_HPP
namespace Anm {

struct Entry {
    const char *folder;
    const char *txt;
    int         offset;
};

constexpr Entry TITLE02 = {
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02/title02.txt",
    0,
};

constexpr Entry TITLE03 = {
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title03",
    GA_RESOURCE_DIR "/th06c/th06c_TL_output/title03/title03.txt",
    5,
};

} // namespace Anm

#endif // ANMDEFS_HPP