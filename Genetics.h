#pragma once

#include <random>
#include <memory>

#include "Simulation.h"

namespace Genetics {
class Population {
public:
    using Creature = std::shared_ptr<Simulation>;
    Population(const City& city, size_t size, int seed);
    void NextGeneration();
    std::vector<Creature> GetTop(size_t) const;

private:
    const City& city;
    size_t size;
    mutable std::mt19937 rnd;
    std::geometric_distribution<> gd;
    std::vector<Creature> creatures;

    void Mutate(Simulation& target, const Simulation& source, int mutation_rate) const;
    void Copulate(Simulation& child, const Simulation& mother, const Simulation& father);
    inline void SortCreatures();
};
}
