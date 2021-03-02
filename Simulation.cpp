#include "Simulation.h"

//#include <iostream>
size_t Simulation::Jam::UnsafeCarQueue::max_size = 0;

Simulation::Simulation(const City& city) :
    city(city),
    D(city.D), I(city.I), S(city.S), V(city.V), F(city.F), score(0), T(0),
    jams(S), cars(V), scheduler(I), lights(I)
{
    for (size_t j = 0; const auto& street : city.street_names) {
        names[&street.second] = &jams[j];
        jams[j].street = &street.second;
        jams[j].begin = street.second.b;
        jams[j].end = street.second.e;
        jams[j].lenght = street.second.l;
        jams[j].cars.init(V);
        lights[jams[j].end].push_back(&jams[j]);
        ++j;
    }
    for (size_t v = 0; v < size_t(V); ++v) {
        for (const auto& s : city.routes[v].path) {
            cars[v].path.push_back(names[s]);
        }
    }
}

void Simulation::Run()
{
    ApplyScheduler(scheduler);
    Reset();
    while (T < D) {
        NextTurn();
    }
}

bool Simulation::Jam::IsGreen(int t) {
    return (t + green_shift) % all_time < green_time;
}

void Simulation::LoadScheduler(std::istream& in) {
    int A, ID, E, T;
    std::string name;
    in >> A;
    for (int i = 0; i < A; ++i) {
        in >> ID >> E;
        scheduler[ID].clear();
        for (int k = 0; k < E; ++k) {
            in >> name >> T;
            scheduler[ID].push_back({names[&city.street_names.at(name)], T});
        }
    }
}

void Simulation::ClearScheduler()
{
    for (auto& l : scheduler) {
        l.clear();
    }
}

void Simulation::SetScheduler(const Simulation::Scheduler_t& scheduler)
{
    for (City::Intersection_t i = 0; i < I; ++i) {
        this->scheduler[i].clear();
        for (const auto& l : scheduler[i]) {
            this->scheduler[i].emplace_back(names[l.first->street], l.second);
        }
    }
}

void Simulation::ShuffleLight(std::mt19937& rnd, City::Intersection_t i)
{
    std::shuffle(scheduler[i].begin(), scheduler[i].end(), rnd);
}

void Simulation::AddLightTime(const City::Street* street, int timer)
{
    Jam* jam = names[street];
    scheduler[jam->end].emplace_back(jam, timer);
}

void Simulation::ApplyScheduler(const Simulation::Scheduler_t& sch)
{
    for (auto& j : jams) {
        j.all_time = 1;
        j.green_shift = 0;
        j.green_time = 0;
    }
    for (const auto& l : sch) {
        int all = 0;
        for (const auto& si : l) {
            all += si.second;
        }
        int shift = 0;
        if (!all) {
            continue;
        }
        for (const auto& si : l) {
            si.first->green_shift = all - shift;
            si.first->all_time = all;
            si.first->green_time = si.second;
            shift += si.second;
        }
    }
}

void Simulation::ApplyScheduler()
{
    ApplyScheduler(scheduler);
}

void Simulation::Reset() {
    T = 0;
    score = 0;
    for (auto& j : jams) {
        j.last_update = -1;
        j.statistics.used = 0;
        j.statistics.stuck = 0;
    }
    for (auto& c : cars) {
        c.remainder = 0;
        c.i_jam = 0;
        c.path.front()->cars.push(&c);
    }
}

void Simulation::NextTurn() {
    for (auto& c : cars) {
        if (--c.remainder > 0) {
            // can go
        } else {
            if (Jam* cur_jam = c.path[c.i_jam];
                cur_jam->last_update < T && // only one car per turn
                cur_jam->cars.front() == &c &&// this car is first
                cur_jam->IsGreen(T)) {
                // move car from this jam
                cur_jam->last_update = T;
                cur_jam->cars.pop();
                c.i_jam++;
                if (c.i_jam + 1 < c.path.size()) {
                    // put car to next jam
                    c.path[c.i_jam]->statistics.used++;
                    Jam* next = c.path[c.i_jam];
                    c.remainder = next->lenght;
                    next->cars.push(&c);
                } else {
                    // finish
                    if (c.path.back()->lenght <= D - T) {
                        score += F + D - T - c.path.back()->lenght;
                    }
                }
            } else {
                // is stuck in jam
                c.path[c.i_jam]->statistics.stuck++;
            }
        }
    }
    ++T;
    //std::cout << Jam::UnsafeCarQueue::max_size << std::endl;
}

void Simulation::RunEmptyScheduler()
{
    for (auto& l : scheduler) {
        l.clear();
    }
    Run();
}

void Simulation::RunRandomScheduler(std::mt19937& rnd, int d, int m)
{
    for (City::Intersection_t i = 0; i < I; ++i) {
        auto& l = scheduler[i];
        l.clear();
        for (Simulation::Jam* j : lights[i]) {
            if (rnd() > UINT32_MAX / d) {
                l.emplace_back(j, rnd() % m + 1);
            }
        }
        std::shuffle(l.begin(), l.end(), rnd);
    }
    Run();
}

void Simulation::RunUniformScheduler()
{
    for (City::Intersection_t i = 0; i < city.I; ++i) {
        auto& l = scheduler[i];
        l.clear();
        for (Jam* j : lights[i]) {
            l.emplace_back(j, 1);
        }
    }
    Run();
}

void Simulation::Jam::UnsafeCarQueue::init(size_t size) {
    queue.resize(size);
    head = tail = 0;
}

void Simulation::Jam::UnsafeCarQueue::push(Car* car) {
    queue[tail++] = car;
    //max_size = max_size > (queue.size() + tail - head) % queue.size() ? max_size : (queue.size() + tail - head) % queue.size();
}

void Simulation::Jam::UnsafeCarQueue::pop() {
    ++head;
}

void Simulation::Jam::UnsafeCarQueue::clear() {
    head = tail = 0;
}

Simulation::Car* Simulation::Jam::UnsafeCarQueue::front() const {
    return queue[head];
}
