#include <iostream>
#include <fstream>

#include "City.h"
#include "CityUtils.h"

int main(int argc, char** argv)
{
    std::string data, solution;
    if (argc == 3) {
        data = argv[1];
        solution = argv[2];
    } else {
        data = "d.txt";
        solution = "answer_d.txt";
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
    std::cout << "initial\t" << CityUtils::Simulate(city) << std::endl;
    CityUtils::SetEmptyScheduler(city);
    std::cout << "empty\t" << CityUtils::Simulate(city) << std::endl;
    CityUtils::SetUniformScheduler(city);
    std::cout << "uniform\t" << CityUtils::Simulate(city) << std::endl;
    for (int i = 0; i < 20; ++ i) {
        int d = rand() % 1000, m = rand() % 7 + 1;
        CityUtils::SetRandomScheduler(city, i, d, m);
        std::cout << "s " << i << " d " << d << " m " << m << "\t" << CityUtils::Simulate(city) << std::endl;
    }
    return 0;
}
