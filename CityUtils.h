#pragma once

#include "City.h"
#include <random>

namespace CityUtils {
    void SetEmptyScheduler(City&);
    void SetUniformScheduler(City&);
    void SetRandomScheduler(City&, int seed = 0, int d = 2, int m = 2);
    void ImproveScheduler(City&);
    int Simulate(City&);
};

