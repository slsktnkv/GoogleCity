#include <iostream>
#include <fstream>

#include "City.h"
#include "Simulation.h"
#include "Genetics.h"

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
    for (int i = 0; i < times; ++i) {
        sims.emplace_back(city);
        sims.back().SetScheduler(sims.front().Scheduler());
        sims.back().Run();
        std::cout << i << "\t" << sims.back().Score() << std::endl;
    }
*/
    {
        Simulation sim(city);
        sch.open(solution);
        sim.LoadScheduler(sch);
        sch.close();
        sim.RunUniformScheduler();
        std::cout << "Uniform:\t" << sim.Score() << std::endl;
    }
    size_t elite_size = 16;
    Genetics::Population world(city, elite_size, 0);
    int time = 0;
    std::cout << "Generation: " << time;
    for (const auto& c : world.GetTop(elite_size * 4)) {
        std::cout << "\t" << c->Score();
    }
    std::cout << std::endl;
    for (++time; time < times; time++) {
        world.NextGeneration();
        std::cout << "Generation: " << time;
        for (const auto& c : world.GetTop(elite_size * 4)) {
            std::cout << "\t" << c->Score();
        }
        std::cout << std::endl;
    }

    return 0;
}
