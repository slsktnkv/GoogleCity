#pragma once

#include "City.h"
#include <random>

namespace CityUtils {
    void SetEmptyScheduler(City&);
    void SetUniformScheduler(City&);
    void SetRandomScheduler(City&, int seed = 0, int d = 2, int m = 2);
    void ImproveScheduler(City&);
    int Simulate(City&, const Scheduler&);
};

namespace Genetics {
class Population {
public:
    using Creature = std::pair<Scheduler, int>;
    Population(City& city, size_t size, int seed);
    void NextGeneration();
    int TopScore() const;

    const std::vector<Creature>& GetPopulation() const {return population;}

private:
    City& city;
    mutable std::mt19937 rnd;
    std::vector<Creature> population;

    Scheduler Mutate(Scheduler, int mutation_rate) const;
    Scheduler Fuck(const Scheduler&, const Scheduler&);
    inline void SortCreatures(std::vector<Creature>&);
};
}
