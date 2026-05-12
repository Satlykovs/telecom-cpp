#pragma once
#include "models.hpp"

class Simulation
{
  public:
    explicit Simulation(Config config);
    void run();

  private:
    Config config_;
    std::vector<Machine> machines_;
    std::priority_queue<Event, std::vector<Event>, std::greater<>> events_;

    long long curTime_{};
    int totalProducts_{};
    int finishedProducts_{};

    int findBestMachine(int itemType) const;
    void tryStartMachine(int machineId);
};