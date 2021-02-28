#include "City.h"


void City::LoadCity(std::istream& in) {
    in >> D >> I >> S >> V >> F;
    jams.resize(S);
    cars.resize(V);
    scheduler.resize(I);
    for (int i = 0; i < S; ++i) {
        int b, e, l;
        std::string name;
        in >> b >> e >> name >> l;
        jams[i].street = streets[{b, e}] = &(street_names[name] = {b, e, l, &jams[i], name});
        jams[i].lenght = l;
    }
    for (int i = 0; i < V; ++i) {
        int p;
        std::string name;
        in >> p;
        cars[i].path.resize(p, nullptr);
        for (int k = 0; k < p; ++k) {
            in >> name;
            cars[i].path[k] = street_names[name].jam;
        }
    }
}

void City::LoadScheduler(std::istream& in) {
    int A, ID, E, T;
    std::string name;
    in >> A;
    for (int i = 0; i < A; ++i) {
        in >> ID >> E;
        scheduler[ID].clear();
        for (int k = 0; k < E; ++k) {
            in >> name >> T;
            scheduler[ID].push_back({street_names[name].jam, T});
        }
    }
}

void City::ApplyScheduler() {
    for (auto& j : jams) {
        j.all_time = 1;
        j.green_shift = 0;
        j.green_time = 0;
    }
    for (const auto& s : scheduler) {
        int all = 0;
        for (const auto& si : s) {
            all += si.second;
        }
        int shift = 0;
        for (const auto& si : s) {
            si.first->green_shift = all - shift;
            si.first->all_time = all;
            si.first->green_time = si.second;
            shift += si.second;
        }
    }
}

void City::Reset() {
    T = 0;
    score = 0;
    for (auto& j : jams) {
        delete j.cars;
        j.cars = new std::deque<Car*>;
        j.last_update = -1;
    }
    for (auto& c : cars) {
        c.remainder = 0;
        c.i_jam = 0;
        c.path.front()->cars->push_back(&c);
    }
}

void City::NextTurn() {
    for (auto& c : cars) {
        if (--c.remainder > 0) {
            // can go
        } else {
            if (Jam* cur_jam = c.path[c.i_jam];
                cur_jam->last_update < T && // only one car per turn
                cur_jam->cars->front() == &c &&// this car is first
                cur_jam->IsGreen(T)) {
                // move car from this jam
                cur_jam->last_update = T;
                cur_jam->cars->pop_front();
                ++c.i_jam;
                if (c.i_jam + 1 < c.path.size()) {
                    // put car to next jam
                    Jam* next = c.path[c.i_jam];
                    c.remainder = next->lenght;
                    next->cars->push_back(&c);
                } else {
                    // finish
                    if (c.path.back()->lenght <= D - T) {
                        score += F + D - T - c.path.back()->lenght;
                    }
                }
            } else {
                // is stuck in jam
            }
        }
    }
    ++T;
}

bool City::CheckCars() {
    for (auto& c : cars) {
        if (c.path.size() < 2) {
            return false;
        }
    }
    return true;
}

bool Jam::IsGreen(int t) {
    return (t + green_shift) % all_time < green_time;
}
