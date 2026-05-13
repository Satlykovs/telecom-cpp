#pragma once
#include <cstdint>
#include <set>
#include <string>
#include <vector>

struct ResourcePrices
{
    int iron{7};
    int gold{11};
    int gems{23};
    int exp{1};
};

enum class ResType : uint8_t { NONE, IRON, GOLD, GEMS, EXP };

struct Room
{
    int id{-1};
    std::set<int> neighbours;
    int iron{0}, gold{0}, gems{0}, exp{0};

    bool visited{false};
    bool freeLootTaken{false};
};

struct Bot
{
    int curRoom{0};
    int food{0};
    int maxFood{0};

    ResType targetRes;

    int totalIron{0}, totalGold{0}, totalGems{0}, totalExp{0};

    bool isReturning{false};
};

struct Config
{
    int N;
    int maxFood;
    ResType targetRes;
    std::vector<Room> rooms;
};
