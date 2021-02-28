#include "CityUtils.h"

void CityUtils::SetEmptyScheduler(City& city) {
    for (auto& l : city.scheduler) {
        l.clear();
    }
}

void CityUtils::SetRandomScheduler(City& city, int seed, int d, int m) {
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

int CityUtils::Simulate(City& city) {
    city.ApplyScheduler();
    city.Reset();
    while (city.T < city.D) {
        city.NextTurn();
    }
    return city.score;
}

void CityUtils::SetUniformScheduler(City& city) {
    for (Intersection i = 0; i < city.I; ++i) {
        auto& l = city.scheduler[i];
        l.clear();
        for (Jam* j : city.lights[i]) {
            l.emplace_back(j, 1);
        }
    }
}
