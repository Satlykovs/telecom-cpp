#include "parser.hpp"

#include "models.hpp"

#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

bool parseNeighbours(std::string s, std::set<int>& neighbours)
{
    for (char c : s) {
        if (!isdigit(c) && c != ',' && c != ' ')
            return false;
    }

    std::ranges::replace(s, ',', ' ');

    std::istringstream ss(s);
    int n{};

    while (ss >> n) {
        neighbours.insert(n);
    }
    ss >> std::ws;
    return ss.eof() && !neighbours.empty();
}

Config parseInput(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Could not open file " + filename);

    Config config;
    std::string line;

    if (!std::getline(file, line)) {
        throw std::runtime_error("Could not read line from file " + filename);
    }

    std::istringstream ssN(line);

    if (!(ssN >> config.N) || config.N < 1 || config.N > 255)
        throw ParseError(line);

    ssN >> std::ws;
    if (!ssN.eof())
        throw ParseError(line);

    config.rooms.resize(config.N + 1);

    for (int i = 0; i <= config.N; i++) {
        if (!std::getline(file, line))
            throw std::runtime_error("Could not read line from file " + filename);

        std::istringstream ss(line);
        std::string neighboursString;
        Room room;

        if (!(ss >> room.id >> neighboursString))
            throw ParseError(line);

        if (!parseNeighbours(neighboursString, room.neighbours))
            throw ParseError(line);

        if (!(ss >> room.iron >> room.gold >> room.gems >> room.exp))
            throw ParseError(line);

        if (room.iron < 0 || room.iron > 255 || room.gold < 0 || room.gold > 255 || room.gems < 0 ||
            room.gems > 255 || room.exp < 0 || room.exp > 255)
            throw ParseError(line);

        ss >> std::ws;
        if (!ss.eof())
            throw ParseError(line);

        config.rooms[room.id] = std::move(room);
    }

    for (int i = 0; i <= config.N; ++i) {
        for (int to : config.rooms[i].neighbours) {
            config.rooms[to].neighbours.insert(i);
        }
    }
    if (!std::getline(file, line))
        throw std::runtime_error("Could not read line from file " + filename);

    std::istringstream ssLast(line);
    std::string targetResStr;
    if (!(ssLast >> config.maxFood >> targetResStr))
        throw ParseError(line);

    if (config.maxFood < 2 || config.maxFood > 255)
        throw ParseError(line);

    if (targetResStr == "iron")
        config.targetRes = ResType::IRON;
    else if (targetResStr == "gold")
        config.targetRes = ResType::GOLD;
    else if (targetResStr == "gems")
        config.targetRes = ResType::GEMS;
    else if (targetResStr == "exp")
        config.targetRes = ResType::EXP;
    else
        throw ParseError(line);

    ssLast >> std::ws;
    if (!ssLast.eof())
        throw ParseError(line);

    return config;
}