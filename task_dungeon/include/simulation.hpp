#pragma once
#include "models.hpp"

#include <fstream>

class Simulation
{
  public:
    explicit Simulation(Config config, std::ofstream& out);
    void run();

  private:
    Config config_;
    Bot bot_;
    ResourcePrices prices_;
    std::ofstream& out_;

    void applyMultiplier();
    void printState();

    bool collectBestResource(Room& room);

    std::vector<int> findPath(int targetId, bool onlyVisited);
};