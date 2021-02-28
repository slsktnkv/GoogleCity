#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>

using Intersection = int;
struct Jam;
struct City;

struct Street {
    Intersection b;
    Intersection e;
    int l;
    Jam* jam;
    std::string name;
};

struct Car {
    int remainder = 0;
    size_t i_jam = 0;
    std::vector<Jam*> path;
};

struct Jam {
    class UnsafeCarQueue
    {
    private:
        std::vector<Car*> queue;
        size_t head;
        size_t tail;
    public:
        inline void init(size_t);
        inline void push(Car*);
        inline void pop();
        inline void clear();
        inline Car* front() const;
    };
    int lenght;
    int green_shift = 0, green_time = 0, all_time = 1;
    int last_update = -1;
    Street* street = nullptr;
    UnsafeCarQueue cars;
    inline bool IsGreen(int);
};

struct City {
    int D, I, S, V, F, score, T;
    std::map<std::string, Street> street_names;
    std::map<std::pair<Intersection, Intersection>, Street*> streets;
    std::vector<std::vector<Jam*>> lights;
    std::vector<Jam> jams;
    std::vector<Car> cars;
    std::vector<std::vector<std::pair<Jam*, int>>> scheduler;

    void LoadCity(std::istream&);
    void LoadScheduler(std::istream&);
    void SerializeScheduler(std::ostream&);
    void ParseScheduler(std::ifstream&);

    void ApplyScheduler();
    void Reset();
    void NextTurn();
    bool CheckCars();
    bool CheckScheduler();
};

