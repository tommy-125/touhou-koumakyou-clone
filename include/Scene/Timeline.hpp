#ifndef SCENE_TIMELINE_HPP
#define SCENE_TIMELINE_HPP

struct TimelineEntry {
    int   frame;
    int   subId;
    float x, y;
    int   life;
    int   score;
    bool  mirrored;
    bool  randomX  = false;  // enemy_create_random: pick x randomly at spawn
    int   itemDrop = -99;    // -99 = use InitSub default; otherwise override m_ItemDrop
};

#endif  // SCENE_TIMELINE_HPP
