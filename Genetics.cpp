#include "Genetics.h"

#include <omp.h>

Genetics::Population::Population(const City& city, size_t size, int seed)
    : city(city), size(size), rnd(seed)
{
    gd = std::geometric_distribution<>(0.1);
    for (size_t s = 0; s < size * size; ++s) {
        creatures.emplace_back(std::make_shared<Simulation>(Simulation(city)));
    }
    #pragma omp parallel for
    for (size_t c = 0; c < size; ++c) {
        creatures[c]->RunUniformScheduler();
    }
    #pragma omp parallel for
    for (size_t c = size; c < size * size; ++c) {
        creatures[c]->RunRandomScheduler(rnd, 5, 5);
    }
    SortCreatures();
}

void Genetics::Population::Mutate(Simulation& target, const Simulation& source, int mutation_rate) const
{
    target.ClearScheduler();
    for (City::Intersection_t i = 0; i < int(source.Scheduler().size()); ++i) {
        if (rnd() % 5 == 0) {
            for (const auto& p : source.Scheduler()[i]) {
                int time = p.second + (rnd() % (mutation_rate * 2  + 1 ) - mutation_rate);
                time = time < 0 ? 0 : time;
                target.AddLightTime(p.first->Street(), time);
            }
        } else {
            for (const auto& p : source.Scheduler()[i]) {
                target.AddLightTime(p.first->Street(), p.second);
            }
        }
        if (rnd() % 5 == 0) {
            target.ShuffleLight(rnd, i);
        }
    }
}

void Genetics::Population::Copulate(Simulation& child, const Simulation& mother, const Simulation& father)
{
    child.ClearScheduler();
    for (int i = 0; i < city.I; ++i) {
        int m_problem = 0, f_problem = 0;
        for (const auto& l : mother.Scheduler()[i]) {
            m_problem += l.first->Statistics().stuck;
        }
        for (const auto& l : father.Scheduler()[i]) {
            f_problem += l.first->Statistics().stuck;
        }
        for (const auto& l : m_problem <= f_problem ? mother.Scheduler()[i] : father.Scheduler()[i]) {
            child.AddLightTime(l.first->Street(), l.second);
        }
    }
}

void Genetics::Population::SortCreatures()
{
    std::sort(creatures.begin(), creatures.end(), [](const Creature& lhs, const Creature& rhs){return lhs->Score() > rhs->Score();});
}

void Genetics::Population::NextGeneration()
{
    size_t mutate = creatures.size() / 2;
    #pragma omp parallel for
    for (size_t i = 0; i < mutate; ++i) {
        Mutate(*creatures[i + mutate], *creatures[i], i + 5);
        creatures[i + mutate]->Run();
    }
    SortCreatures();
    #pragma omp parallel for
    for (size_t i = size; i < creatures.size(); ++i) {
        Copulate(*creatures[i], *creatures[gd(rnd) % creatures.size()], *creatures[rnd() % creatures.size()]);
        creatures[i]->Run();
    }
    SortCreatures();
}

std::vector<Genetics::Population::Creature> Genetics::Population::GetTop(size_t size) const
{
    return std::vector<Genetics::Population::Creature>(creatures.begin(), creatures.begin() + size);
}
