#pragma once
#include <cstdint>
#include <optional>
#include <queue>
#include <vector>

struct Product
{
    int id;
    int type;
};

struct Machine
{
    int id{};
    std::queue<Product> queue;
    std::optional<Product> curProduct;
    long long busyUntil{};
    long long queueTimeSum{};
};

enum class EventType : std::uint8_t { FINISH = 0, START = 1, WAIT = 2, READY = 3 };

struct Event
{
    long long time{};
    EventType type{EventType::FINISH};
    int productId{};
    int machineId{};
    int opType{};
    int queueSize{};

    bool operator>(const Event& other) const
    {
        if (time != other.time)
            return time > other.time;
        return type > other.type;
    }
};

struct Config
{
    int M;
    int N;
    std::vector<std::vector<int>> T;
    std::vector<std::vector<int>> initialQueues;
};