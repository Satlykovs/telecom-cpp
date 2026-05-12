#include "parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

bool readInts(const std::string& line, std::vector<int>& ints)
{
    std::istringstream ss(line);

    int val{};

    while (ss >> val) {
        ints.push_back(val);
    }

    ss >> std::ws;
    return ss.eof() && !ints.empty();
}

Config parseInput(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
        throw std::runtime_error("Could not open file " + filename);

    Config config;
    std::string line;

    if (!std::getline(file, line))
        throw std::runtime_error("Could not read line from file " + filename);

    std::vector<int> mn;
    if (!readInts(line, mn) || mn.size() != 2) {
        throw ParseError(line);
    }

    if (mn[0] < 1 || mn[0] > 100 || mn[1] < 1 || mn[1] > 100) {
        throw ParseError(line);
    }

    config.M = mn[0];
    config.N = mn[1];

    for (int i = 0; i < config.M - 1; ++i) {
        if (!std::getline(file, line))
            throw std::runtime_error("Could not read line from file " + filename);

        std::vector<int> row;

        if (!readInts(line, row) || row.size() != config.N) {
            throw ParseError(line);
        }

        for (int val : row) {
            if (val < 0 || val > 10000) {
                throw ParseError(line);
            }
        }
        config.T.push_back(std::move(row));
    }

    for (int j = 0; j < config.N; ++j) {
        if (!std::getline(file, line))
            throw std::runtime_error("Could not read line from file " + filename);

        std::vector<int> row;
        if (!readInts(line, row)) {
            throw ParseError(line);
        }

        int q_j = row[0];
        if (q_j < 0 || row.size() != q_j + 1) {
            throw ParseError(line);
        }

        std::vector<int> types;
        for (int p = 1; p <= q_j; ++p) {
            if (row[p] < 0 || row[p] > config.M - 2) {
                throw ParseError(line);
            }
            types.push_back(row[p]);
        }
        config.initialQueues.push_back(std::move(types));
    }
    return config;
}