#include "CityUtils.h"

void CityUtils::SetEmptyScheduler(City& city)
{
    for (auto& l : city.scheduler) {
        l.clear();
    }
}

void CityUtils::SetRandomScheduler(City& city, int seed, int d, int m)
{
    std::mt19937 g(seed);
    for (Intersection i = 0; i < city.I; ++i) {
        auto& l = city.scheduler[i];
        l.clear();
        for (Jam* j : city.lights[i]) {
            if (g() > UINT32_MAX / d) {
                l.emplace_back(j, g() % m + 1);
            }
        }
        std::shuffle(l.begin(), l.end(), g);
    }
}

int CityUtils::Simulate(City& city, const Scheduler& s)
{
    city.ApplyScheduler(s);
    city.Reset();
    while (city.T < city.D) {
        city.NextTurn();
    }
    return city.score;
}

void CityUtils::SetUniformScheduler(City& city)
{
    for (Intersection i = 0; i < city.I; ++i) {
        auto& l = city.scheduler[i];
        l.clear();
        for (Jam* j : city.lights[i]) {
            l.emplace_back(j, 1);
        }
    }
}

void CityUtils::ImproveScheduler(City& city)
{

}

Genetics::Population::Population(City& city, size_t size, int seed)
    : city(city), rnd(seed), population(2 * size, {Scheduler(city.I), 0})
{
    for (auto& s : population) {
        for (int i = 0; i < city.I; ++i) {
            for (Jam* j : city.lights[i]) {
                s.first[i].emplace_back(j, 1);
            }
        }
        s.second = CityUtils::Simulate(city, s.first);
    }
    SortCreatures(population);
}

Scheduler Genetics::Population::Mutate(Scheduler s, int mutation_rate) const
{
    for (auto& l : s) {
        if (rnd() % 5 == 0) {
            for (auto& p : l) {
                p.second += rnd() % (mutation_rate * 2  + 1 ) - mutation_rate;
                p.second = p.second < 0 ? 0 : p.second;
            }
        }
        if (rnd() % 5 == 0) {
            std::shuffle(l.begin(), l.end(), rnd);
        }
    }
    return s;
}

void Genetics::Population::SortCreatures(std::vector<Genetics::Population::Creature>& creatures)
{
    std::sort(creatures.begin(), creatures.end(), [](const Creature& lhs, const Creature& rhs){return lhs.second > rhs.second;});
}

void Genetics::Population::NextGeneration()
{
    for (int i = 0; i < int(population.size()) / 2; ++i) {
        Creature& victim(population[i + population.size()/2]);
        victim.first = Mutate(population[i].first, i + 5);
        victim.second = CityUtils::Simulate(city, victim.first);
    }
    SortCreatures(population);
}

int Genetics::Population::TopScore() const
{
    return population.front().second;
}
