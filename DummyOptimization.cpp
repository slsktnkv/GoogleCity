#include "DummyOptimization.h"


int ReduceUnsed(Simulation& sim)
{
    int counter = 0;
    Simulation::Scheduler_t sch = sim.UnsafeScheduler();
    for (auto& i : sch) {
        if (i.size() <= 1) {
            continue;
        }
        for (size_t l = 1; l < i.size(); ++l) {
            if (i[l].first->Statistics().stuck == 0 && i[l].first->Statistics().used == 0) {
                i[l].second = 0;
                if (i[l].second > 0) {
                    counter++;
                }
            }
        }
    }
    return counter;
}

int IncreaseStucked(Simulation& sim)
{
    int counter = 0;
    Simulation::Scheduler_t sch(sim.city.I);
    for (City::Intersection_t i = 0; i <  sim.city.I; ++i) {
        for (Simulation::Jam* j : sim.Lights()[i]) {
            sch[i].emplace_back(j, j->GreenTime());
        }
        std::sort(sch[i].begin(), sch[i].end(), [](const std::pair<Simulation::Jam*, int>& lhs, const std::pair<Simulation::Jam*, int>& rhs){
            return lhs.first->Statistics().stuck > rhs.first->Statistics().stuck;});
        if (sch[i].front().first->Statistics().stuck > 0) {
            sch[i].front().second++;
            counter++;
        }
    }
    sim.SetScheduler(sch);
    return counter;
}
