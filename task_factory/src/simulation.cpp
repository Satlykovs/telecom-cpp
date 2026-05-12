#include "simulation.hpp"

#include <iostream>

Simulation::Simulation(Config config) : config_(std::move(config))
{
    for (int j = 0; j < config_.N; ++j) {
        machines_.push_back(Machine{j});
    }

    int curId = 0;

    for (int j = 0; j < config_.N; ++j) {
        for (const int type : config_.initialQueues[j]) {
            Product p{curId++, type};

            machines_[j].queue.push(p);
            machines_[j].queueTimeSum += config_.T[type][j];
            totalProducts_++;
        }
    }

    for (int j = 0; j < config_.N; ++j) {
        tryStartMachine(j);
    }
}

void Simulation::tryStartMachine(int machineId)
{

    if (auto& m = machines_[machineId]; !m.curProduct.has_value() && !m.queue.empty()) {
        const Product p = m.queue.front();
        m.queue.pop();

        m.queueTimeSum -= config_.T[p.type][machineId];

        m.curProduct = p;

        events_.push({curTime_, EventType::START, p.id, machineId, p.type});
    }
}

int Simulation::findBestMachine(int itemType) const
{
    int bestJ = 0;

    long long minWaitTime = machines_[0].queueTimeSum;

    for (int j = 1; j < config_.N; ++j) {
        if (machines_[j].queueTimeSum < minWaitTime) {
            minWaitTime = machines_[j].queueTimeSum;
            bestJ = j;
        }
    }
    return bestJ;
}

void Simulation::run()
{
    while (!events_.empty()) {
        Event e = events_.top();
        events_.pop();

        curTime_ = e.time;

        switch (e.type) {
            case EventType::START: {
                auto& m = machines_[e.machineId];

                const int duration = config_.T[e.opType][e.machineId];
                m.busyUntil = curTime_ + duration;

                std::cout << "start " << curTime_ << ' ' << e.productId << ' ' << e.opType << ' '
                          << e.machineId << '\n';

                events_.push({m.busyUntil, EventType::FINISH, e.productId, e.machineId, e.opType});
                break;
            }
            case EventType::FINISH: {

                auto& m = machines_[e.machineId];
                std::cout << "finish " << curTime_ << ' ' << e.productId << ' ' << e.opType << ' '
                          << e.machineId << '\n';

                m.curProduct.reset();

                if (const int nextType = e.opType + 1; nextType == config_.M - 1) {
                    events_.push({curTime_, EventType::READY, e.productId, e.machineId, e.opType});
                } else {
                    const int bestJ = findBestMachine(nextType);

                    if (Machine& targetMachine = machines_[bestJ];
                        targetMachine.curProduct.has_value()) {
                        const int inQueueBefore = targetMachine.queue.size();

                        targetMachine.queue.push(Product{e.productId, nextType});
                        targetMachine.queueTimeSum += config_.T[nextType][bestJ];

                        events_.push({curTime_, EventType::WAIT, e.productId, bestJ, nextType,
                                      inQueueBefore});
                    } else {
                        targetMachine.curProduct = Product{e.productId, nextType};
                        events_.push({curTime_, EventType::START, e.productId, bestJ, nextType});
                    }
                }
                tryStartMachine(e.machineId);
                break;
            }
            case EventType::WAIT: {
                std::cout << "wait " << curTime_ << ' ' << e.productId << ' ' << e.opType << ' '
                          << e.machineId << ' ' << e.queueSize << '\n';
                break;
            }
            case EventType::READY: {
                std::cout << "ready " << curTime_ << ' ' << e.productId << ' ' << e.machineId
                          << '\n';
                finishedProducts_++;
                if (finishedProducts_ == totalProducts_) {
                    std::cout << "stop " << curTime_ << '\n';
                    return;
                }
                break;
            }
        }
    }
}
