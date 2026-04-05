#ifndef ENEMY_TIMELINE_HPP
#define ENEMY_TIMELINE_HPP

struct TimelineEntry {
    int   frame;
    int   subId;
    float x, y;
    int   life;
    int   score;
    bool  mirrored;
};

#endif  // ENEMY_TIMELINE_HPP
