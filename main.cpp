#include <iostream>
#include <fstream>

#include "City.h"

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
    city.ApplyScheduler();
    for (int i = 0; i < 40; ++ i) {
        city.Reset();
        while (city.T < city.D) {
            city.NextTurn();
        }
        std::cout << i << " " << city.score << std::endl;
    }
    return 0;
}
