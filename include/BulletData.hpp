#include "Player.hpp"
#include "Anm/AnmDefs.hpp"

/* ----------ReimuB---------- */

/* ReimuB Rank 1 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank1[] = {
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-90.0f), 12.0f, 48, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
};

/* ReimuB Rank 2 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank2[] = {
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-90.0f), 12.0f, 48, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {15, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {15, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

/* ReimuB Rank 3 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank3[] = {
    {5, 0, {-4.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-91.0f), 12.0f, 32, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {4.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-89.0f), 12.0f, 32, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {10, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {10, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

/* ReimuB Rank 4 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank4[] = {
    {5, 0, {-4.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-91.0f), 12.0f, 30, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {4.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-89.0f), 12.0f, 30, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {8, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {8, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

/* ReimuB Rank 5 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank5[] = {
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-97.0f), 12.0f, 20, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-90.0f), 12.0f, 28, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-83.0f), 12.0f, 20, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {8, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {8, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

/* ReimuB Rank 6 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank6[] = {
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-97.0f), 12.0f, 16, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-90.0f), 12.0f, 27, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-83.0f), 12.0f, 16, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {5, 0, {8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 12, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {8, 0, {-8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {8, 0, {-8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

/* ReimuB Rank 7 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank7[] = {
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-98.0f), 12.0f, 16, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-90.0f), 12.0f, 22, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-82.0f), 12.0f, 16, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {3, 0, {8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {3, 0, {8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {5, 0, {-8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {5, 0, {-8.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

/* ReimuB Rank 8 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank8[] = {
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-106.0f), 12.0f, 9, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-98.0f), 12.0f, 17, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-90.0f), 12.0f, 20, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-82.0f), 12.0f, 17, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-74.0f), 12.0f, 9, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {3, 0, {12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {3, 0, {12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {5, 0, {-12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {5, 0, {-12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {10, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {10, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

/* ReimuB Rank 9 */
constexpr CharacterPowerBulletData CharacterPowerBulletDataReimuBRank9[] = {
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-106.0f), 12.0f, 9, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-98.0f), 12.0f, 17, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-90.0f), 12.0f, 20, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-82.0f), 12.0f, 17, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {5, 0, {0.0f, 0.0f}, {12.0f, 12.0f}, glm::radians(-74.0f), 12.0f, 9, 0, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_BULLET},
    {3, 0, {12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {3, 0, {12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {3, 0, {-12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {3, 0, {-12.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {5, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 1, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
    {5, 0, {0.0f, -16.0f}, {12.0f, 40.0f}, glm::radians(-90.0f), 22.0f, 10, 2, BulletType::STRAIGHT, Anm::PLAYER00.offset ,Anm::SCRIPT_PLAYER_REIMU_B_ORB_BULLET},
};

constexpr CharacterPowerData CharacterPowerDataReimuB[] = {
    {1,8, CharacterPowerBulletDataReimuBRank1},
    {3, 16, CharacterPowerBulletDataReimuBRank2},
    {4, 32, CharacterPowerBulletDataReimuBRank3},
    {4, 48, CharacterPowerBulletDataReimuBRank4},
    {5, 64, CharacterPowerBulletDataReimuBRank5},
    {7, 80, CharacterPowerBulletDataReimuBRank6},
    {7, 96, CharacterPowerBulletDataReimuBRank7},
    {11, 127, CharacterPowerBulletDataReimuBRank8},
    {11, 999, CharacterPowerBulletDataReimuBRank9},
};