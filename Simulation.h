#pragma once

#include <queue>
#include <random>

#include "City.h"

// #define CAR_QUEUE_ON_STACK

class Simulation
{
    struct Car;
public:
    const City& city;
    class Jam {
    public:
        struct Statistics {
            int used = 0;
            int stuck = 0;
        };
        const Statistics& Statistics() const {return statistics;}
        const City::Street* Street() const {return street;}
        int GreenTime() const {return green_time;}
    private:
        class UnsafeCarQueue
        {
        private:
            size_t head;
        #ifdef CAR_QUEUE_ON_STACK
            static const size_t UNSAFE_CAR_QUEUE_SIZE = 64;
            Car* queue[UNSAFE_CAR_QUEUE_SIZE];
            size_t tail;
        public:
        #else // CAR_QUEUE_ON_STACK
            static const size_t RESIZE_FACTOR = 2;
            std::vector<Car*> queue;
            size_t size;
        public:
            static size_t max_size;
            inline void reserve(size_t);
        #endif //CAR_QUEUE_ON_STACK
            inline void push(Car*);
            inline void pop();
            inline void clear();
            inline Car* front() const;
        };
        const City::Street* street = nullptr;
        int begin;
        int end;
        int lenght;
        int green_shift = 0, green_time = 0, all_time = 1;
        int last_update = -1;
        struct Statistics statistics;
        UnsafeCarQueue cars;
        inline bool IsGreen(int);
        friend class Simulation;
    };
    using Scheduler_t = std::vector<std::vector<std::pair<Jam*, int>>>;

    Simulation(const City& city);
    Jam* GetJam(const City::Street* street) {return names.at(street);}
    const std::vector<std::vector<Jam*>>& Lights() const {return lights;}
    void Run();
    int Score() const {return score;}

    void LoadScheduler(std::istream&);
    void SerializeScheduler(std::ostream&);
    void ParseScheduler(std::ifstream&);
    bool CheckScheduler(const Scheduler_t&) const;
    const Scheduler_t& Scheduler() const {return scheduler;}
    Scheduler_t& UnsafeScheduler() {return scheduler;}

    void ClearScheduler();
    void SetScheduler(const Scheduler_t& scheduler);
    void ShuffleLight(std::mt19937&, City::Intersection_t);
    void AddLightTime(const City::Street*, int);

    void RunEmptyScheduler();
    void RunUniformScheduler();
    void RunRandomScheduler(std::mt19937&, int d, int m);

private:
    struct Car {
        int remainder = 0;
        size_t i_jam = 0;
        std::vector<Jam*> path;
    };

    int D, I, S, V, F, score, T;
    std::map<const City::Street*, Jam*> names;
    std::vector<Jam> jams;
    std::vector<Car> cars;
    Scheduler_t scheduler;
    std::vector<std::vector<Jam*>> lights;

    void Reset();
    void NextTurn();
    int Time() const {return T;}
    int EndTime() const {return D;}
    void ApplyScheduler(const Scheduler_t&);
    void ApplyScheduler();
};

