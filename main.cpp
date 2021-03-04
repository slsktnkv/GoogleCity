#include <iostream>
#include <fstream>

#include <omp.h>

#include "City.h"
#include "Simulation.h"
#include "Genetics.h"
#include "DummyOptimization.h"

int main(int argc, char** argv)
{
    std::string data, solution;
    int times;
    size_t size;
    if (argc == 5) {
        data = argv[1];
        solution = argv[2];
        size = atoi(argv[3]);
        times = atoi(argv[4]);
    } else {
        data = "a.txt";
        solution = "answer_a.txt";
        size = 8;
        times = 20;
    }
    City city;
    std::ifstream in, sch;
    in.open(data);
    city.LoadCity(in);
    in.close();
    if (!city.CheckRouts()) {
        std::cout << "PROBLEM WITH CARS' ROUTES!" << std::endl;
    }

    /*
    std::vector<Simulation> sims;
    sims.emplace_back(city);
    sch.open(solution);
    sims.front().LoadScheduler(sch);
    sch.close();
    sims.front().Run();
    std::cout << "Initial:\t" << sims.front().Score() << std::endl;
    for (int i = 1; i < times; ++i) {
        sims.emplace_back(city);
        std::cout << "Constructed:" << "\t" << i << std::endl;
    }
    #pragma omp parallel for
    for (int i = 1; i < times; ++i) {
        sims[i].SetScheduler(sims.front().Scheduler());
        sims[i].Run();
        std::cout << i << "\t" << sims[i].Score() << std::endl;
    }
    */
    /*
    size_t elite_size = size;
    Genetics::Population world(city, elite_size, 0);
    int time = 0;
    std::cout << "Generation: " << time;
    for (const auto& c : world.GetTop(elite_size * 2)) {
        std::cout << "\t" << c->Score();
    }
    std::cout << std::endl;
    for (++time; time < times; time++) {
        world.NextGeneration();
        std::cout << "Generation: " << time;
        for (const auto& c : world.GetTop(elite_size * 2)) {
            std::cout << "\t" << c->Score();
        }
        std::cout << std::endl;
    }
    */
    Simulation sim(city);
    sch.open(solution);
    sim.LoadScheduler(sch);
    sch.close();
    sim.Run();
    std::cout << "initial:\t" << sim.Score() << std::endl;
    std::cout << "unused:\t" << ReduceUnsed(sim);
    sim.Run();
    std::cout << "\tscore:\t" << sim.Score() << std::endl;
    /*
    for (int s = 0; s < times; ++s) {
        std::cout << "unused:\t" << ReduceUnsed(sim);
        sim.Run();
        std::cout << "\tscore:\t" << sim.Score() << std::endl;
        std::cout << "increase:\t" << IncreaseStucked(sim);
        sim.Run();
        std::cout << "\tscore:\t" << sim.Score() << std::endl;
    }
    */

    return 0;
}
