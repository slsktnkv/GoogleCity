#include <iostream>
#include <fstream>

#include "City.h"
#include "CityUtils.h"

int main(int argc, char** argv)
{
    std::string data, solution;
    int times;
    if (argc == 4) {
        data = argv[1];
        solution = argv[2];
        times = atoi(argv[3]);
    } else {
        data = "a.txt";
        solution = "answer_a.txt";
        times = 20;
    }
    City city;
    std::ifstream in, sch;
    in.open(data);
    city.LoadCity(in);
    in.close();
    sch.open(solution);
    city.LoadScheduler(sch);
    sch.close();
    if (!city.CheckCars()) {
        std::cout << "PROBLEM WITH CARS" << std::endl;
    }
    std::cout << "Solution\tScore" << std::endl;
    std::cout << "initial\t" << CityUtils::Simulate(city, city.scheduler) << std::endl;
    CityUtils::SetEmptyScheduler(city);
    std::cout << "empty\t" << CityUtils::Simulate(city, city.scheduler) << std::endl;
    CityUtils::SetUniformScheduler(city);
    std::cout << "uniform\t" << CityUtils::Simulate(city, city.scheduler) << std::endl;
    /*
    for (int i = 0; i < 20; ++ i) {
        int d = rand() % 1000, m = rand() % 7 + 1;
        CityUtils::SetRandomScheduler(city, i, d, m);
        std::cout << "s " << i << " d " << d << " m " << m << "\t" << CityUtils::Simulate(city, city.scheduler) << std::endl;
    }
    */
    Genetics::Population world(city, 10, 0);
    int time = 0;
    std::cout << "Generation: " << time;
    for (const auto& c : world.GetPopulation()) {
        std::cout << "\t" << c.second;
    }
    std::cout << std::endl;
    for (++time; time < times; time++) {
        world.NextGeneration();
        std::cout << "Generation: " << time;
        for (const auto& c : world.GetPopulation()) {
            std::cout << "\t" << c.second;
        }
        std::cout << std::endl;
    }
    return 0;
}
