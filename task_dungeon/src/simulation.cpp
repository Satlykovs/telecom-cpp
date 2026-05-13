#include "simulation.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>

Simulation::Simulation(Config config, std::ofstream& out) : config_(std::move(config)), out_(out)
{
    bot_.curRoom = 0;
    bot_.maxFood = config_.maxFood;
    bot_.food = config_.maxFood;
    bot_.targetRes = config_.targetRes;

    config_.rooms[0].visited = true;

    applyMultiplier();
}

void Simulation::applyMultiplier()
{
    switch (bot_.targetRes) {
        case ResType::IRON:
            prices_.iron *= 2;
            break;
        case ResType::GOLD:
            prices_.gold *= 2;
            break;
        case ResType::GEMS:
            prices_.gems *= 2;
            break;
        case ResType::EXP:
            prices_.exp *= 2;
            break;
        case ResType::NONE:
            break;
    }
}

std::string formatResource(int amount)
{
    if (amount == -1)
        return "_";
    return std::to_string(amount);
}

void Simulation::printState()
{
    const Room& r = config_.rooms[bot_.curRoom];

    out_ << "state " << bot_.curRoom << ' ' << formatResource(r.iron) << ' '
         << formatResource(r.gold) << ' ' << formatResource(r.gems) << ' ' << formatResource(r.exp)
         << '\n';
}

bool Simulation::collectBestResource(Room& room)
{
    int bestPrice = -1;

    ResType bestRes = ResType::NONE;

    if (room.iron > 0 && prices_.iron > bestPrice) {
        bestPrice = prices_.iron;
        bestRes = ResType::IRON;
    }
    if (room.gold > 0 && prices_.gold > bestPrice) {
        bestPrice = prices_.gold;
        bestRes = ResType::GOLD;
    }
    if (room.exp > 0 && prices_.exp > bestPrice) {
        bestPrice = prices_.exp;
        bestRes = ResType::EXP;
    }
    if (room.gems > 0 && prices_.gems > bestPrice) {
        bestPrice = prices_.gems;
        bestRes = ResType::GEMS;
    }

    if (bestPrice == -1)
        return false;

    if (room.freeLootTaken) {
        if (bot_.food <= 0)
            return false;
        --bot_.food;
    } else {
        room.freeLootTaken = true;
    }

    out_ << "collect ";
    switch (bestRes) {
        case ResType::IRON:
            bot_.totalIron += room.iron;
            room.iron = -1;
            out_ << "iron\n";
            break;
        case ResType::GOLD:
            bot_.totalGold += room.gold;
            room.gold = -1;
            out_ << "gold\n";
            break;
        case ResType::GEMS:
            bot_.totalGems += room.gems;
            room.gems = -1;
            out_ << "gems\n";
            break;
        case ResType::EXP:
            bot_.totalExp += room.exp;
            room.exp = -1;
            out_ << "exp\n";
            break;
        default:
            break;
    }
    return true;
}

std::vector<int> Simulation::findPath(int targetId, bool onlyVisited)
{
    int n = config_.rooms.size();
    std::queue<int> q;
    std::vector<int> parent(n, -1);
    std::vector<int> dist(n, -1);

    q.push(bot_.curRoom);
    dist[bot_.curRoom] = 0;

    while (!q.empty()) {
        int curId = q.front();
        q.pop();

        Room& room = config_.rooms[curId];
        for (int neighbourId : room.neighbours) {
            if (dist[neighbourId] != -1)
                continue;
            if (onlyVisited && !config_.rooms[neighbourId].visited)
                continue;

            dist[neighbourId] = dist[curId] + 1;
            parent[neighbourId] = curId;
            q.push(neighbourId);
        }
    }

    int target = -1;
    if (targetId == -1) {
        int bestDist = std::numeric_limits<int>::max();
        int bestId = std::numeric_limits<int>::max();

        for (int id = 0; id < n; ++id) {
            if (id == bot_.curRoom)
                continue;
            if (dist[id] == -1)
                continue;
            if (config_.rooms[id].visited)
                continue;

            if (dist[id] < bestDist || (dist[id] == bestDist && id < bestId)) {
                bestDist = dist[id];
                bestId = id;
                target = id;
            }
        }
    } else {
        if (targetId >= 0 && targetId < n && dist[targetId] != -1)
            target = targetId;
    }

    if (target == -1)
        return {};

    std::vector<int> path;
    for (int v = target; v != -1; v = parent[v])
        path.push_back(v);

    std::ranges::reverse(path);

    if (!path.empty()) {
        path.erase(path.begin());
    }
    return path;
}

void Simulation::run()
{
    int researchLimit = bot_.maxFood / 2;

    while (bot_.food > researchLimit) {
        Room& curRoom = config_.rooms[bot_.curRoom];

        int nextId = -1;
        for (int neighbourId : curRoom.neighbours) {
            if (!config_.rooms[neighbourId].visited) {
                nextId = neighbourId;
                break;
            }
        }

        std::vector<int> path;
        if (nextId != -1)
            path = {nextId};
        else
            path = findPath(-1, false);

        if (path.empty() || bot_.food - path.size() < researchLimit)
            break;

        for (int stepId : path) {
            bot_.curRoom = stepId;
            --bot_.food;

            out_ << "go " << stepId << '\n';

            Room& r = config_.rooms[stepId];
            r.visited = true;

            printState();
            if (collectBestResource(r))
                printState();
        }
    }

    std::vector<int> pathToHome = findPath(0, true);

    int extraFood = bot_.food - pathToHome.size();

    for (int stepId : pathToHome) {
        bot_.curRoom = stepId;
        --bot_.food;

        out_ << "go " << stepId << '\n';
        if (stepId == 0)
            break;

        Room& r = config_.rooms[bot_.curRoom];

        printState();
        while (extraFood > 0) {
            if (!collectBestResource(r))
                break;
            printState();
            extraFood--;
        }
    }

    const int total = (bot_.totalIron * prices_.iron) + (bot_.totalGold * prices_.gold) +
                      (bot_.totalGems * prices_.gems) + (bot_.totalExp * prices_.exp);

    out_ << "result " << bot_.totalIron << ' ' << bot_.totalGold << ' ' << bot_.totalGems << ' '
         << bot_.totalExp << " " << total << '\n';
}
