#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>

struct City {
    using Intersection_t = int;
    struct Street {
        Intersection_t b;
        Intersection_t e;
        int l;
        std::string name;
    };

    struct Route {
        std::vector<const Street*> path;
    };
    int D, I, S, V, F;
    std::map<std::string, Street> street_names;
    std::map<std::pair<Intersection_t, Intersection_t>, Street*> streets;
    std::vector<std::vector<Street*>> lights;
    std::vector<Route> routes;

    void LoadCity(std::istream&);

    bool CheckRouts();
};

