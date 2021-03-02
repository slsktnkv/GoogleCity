#include "City.h"


void City::LoadCity(std::istream& in) {
    in >> D >> I >> S >> V >> F;
    lights.resize(I);
    routes.resize(V);
    for (int i = 0; i < S; ++i) {
        int b, e, l;
        std::string name;
        in >> b >> e >> name >> l;
        lights[e].push_back(streets[{b, e}] = &(street_names[name] = {b, e, l, name}));
    }
    for (int i = 0; i < V; ++i) {
        int p;
        std::string name;
        in >> p;
        routes[i].path.resize(p, nullptr);
        for (int k = 0; k < p; ++k) {
            in >> name;
            routes[i].path[k] = &street_names[name];
        }
    }
}

bool City::CheckRouts() {
    for (const auto& r : routes) {
        if (r.path.size() < 2) {
            return false;
        }
    }
    return true;
}
