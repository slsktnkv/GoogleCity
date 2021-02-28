#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include<string>
using namespace std;
ifstream fin;
struct Street{
    int from;
    int dist;
    int length;
    string name;
    queue<int> cars;
};

struct Car{
    queue<Street> path;
    int current_intersect;
    bool blocked;
};

struct Intersect{
    vector<Street>in;
    vector<Street>out;
};
struct Light{
    int time;
    string name;
};
struct Semaphore{
    vector<Light> lights;
    int time;
    int pos;
    int cycle;
    int intersect;
};

struct Schedule{
    vector<Semaphore> semaphores;
    int len;
    int score;
};


int get_semaphore_position(Semaphore &s, int time){
    time = time % s.cycle;
    int pos = 0;
    while(time >= s.lights[pos].time){
        time -= s.lights[pos].time;
        pos ++;
    }
    return pos;
}

Schedule get_schedule(vector<Intersect>& Intersects, int time){
    vector<Semaphore> sch;
    for(int i = 0; i < Intersects.size(); ++i){
        vector<Light> l;
        for(int j = 0; j < Intersects[i].in.size(); ++j){
            Light light;
            light.name = Intersects[i].in[j].name;
            light.time = time;
            l.push_back(light);
        }
        Semaphore s = {l, 0, 0, time * (int)Intersects[i].in.size(), i};
        sch.push_back(s);
    }
    Schedule schedule = {sch, (int)sch.size(), 0};
    return schedule;
}

void show_schedule(Schedule s){
    cout << s.len<< endl;
    for(int i = 0; i < s.semaphores.size(); ++i){
        cout << s.semaphores[i].intersect<< endl;
        cout << s.semaphores[i].lights.size() << endl;
        for(int j = 0; j < s.semaphores[i].lights.size(); ++j)
            cout << s.semaphores[i].lights[j].name<< " " << s.semaphores[i].lights[j].time << endl;
    }
}

int simulate(int duration, vector<Intersect>&Intersects, map<string, Street> street_table, vector<Car> cars, Schedule &schedule, int bonus){
    // расставить машины на стартовые позиции
    if (schedule.score) return schedule.score;
    int total_score = 0;
    for(int i = 0; i < cars.size(); ++i){
        cars[i].path.front().length = 0;
        street_table[cars[i].path.front().name].cars.push(i);
        cars[i].current_intersect = street_table[cars[i].path.front().name].dist;
    }
    for(int t = 0; t < duration; ++t){
        for(int i = 0; i < cars.size(); ++i){
            if(!cars[i].blocked){
                if(cars[i].path.front().length){
                    cars[i].path.front().length --;
                    if(!cars[i].path.front().length){
                        street_table[cars[i].path.front().name].cars.push(i);
                        cars[i].current_intersect = cars[i].path.front().dist;
                    }
                }
            }
        }
        //cout << active_semaphores.size()<<endl;
        // сложность O(duration * intersections) <= 10 ^ 9
        // Можем оптимизировать до O(duration * cars) <= 10 ^ 7
        // если итерироваться не по всем сфетофорам, а только по тем, где стоят машины
        map<int, int> used;
        for(int iter = 0; iter < cars.size(); ++iter){
            int i = cars[iter].current_intersect;
            if(i >= 0 && !(used.count(i))){
                int pos = get_semaphore_position(schedule.semaphores[i], t);
                string green_street = schedule.semaphores[i].lights[pos].name;
                if(!street_table[green_street].cars.empty()){
                    int car_number = street_table[green_street].cars.front();
                    street_table[green_street].cars.pop();
                    cars[car_number].path.pop();
                    cars[car_number].current_intersect = -1;
                    if(cars[car_number].path.size() == 1){
                        cars[car_number].blocked = true;
                        if(cars[car_number].path.front().length + t <= duration)
                            total_score += bonus +(duration - (cars[car_number].path.front().length + t));
                    }
                }
            }
            used[i] = 1;
        }
    }
    return total_score;
}

Schedule read_schedule(vector<Intersect>& Intersects, string schedule_file){
    vector<Semaphore> sch;
    fin.open(schedule_file);
    int semaphore_count;
    fin >> semaphore_count;
    for(int i = 0; i < semaphore_count; ++i){
        vector<Light> l;
        int lights_count, intersect;
        fin >> intersect >> lights_count;
        int cycle = 0;
        for(int j = 0; j < lights_count; ++j){
            Light light;
            fin >> light.name >> light.time;
            l.push_back(light);
            cycle += light.time;
        }
        Semaphore s = {l, 0, 0, cycle , intersect};
        sch.push_back(s);
    }
    Schedule schedule = {sch, (int)sch.size(), 0};
    fin.close();
    return schedule;
}

Schedule mutate(Schedule s, int k){
    for(int i = 0; i < s.semaphores.size(); ++i){
        int new_cycle = 0;
        //cout << s.semaphores[i].lights.size() << endl;
        for(int j = 0; j < s.semaphores[i].lights.size(); ++j){
            s.semaphores[i].lights[j].time += rand() % (2 * k + 1) - k;
            if(s.semaphores[i].lights[j].time < 0)
                s.semaphores[i].lights[j].time = 0;
            new_cycle += s.semaphores[i].lights[j].time;
            if(!new_cycle && j == s.semaphores[i].lights.size() - 1){
                new_cycle = 1;
                s.semaphores[i].lights[j].time = 1;
            }
        }
        s.semaphores[i].cycle = new_cycle;
    }
    s.score = 0;
    return s;
}


vector<Schedule> get_genetic_winners(vector<Schedule> population, int duration, vector<Intersect>&Intersects, map<string, Street> street_table, vector<Car> cars, int bonus, int part){
    vector<Schedule> winners;
    vector<pair<int, int>> scores;
    for(int i = 0; i < population.size(); ++i){
        int score = simulate(duration, Intersects, street_table, cars, population[i], bonus);
        cout << score<<" ";
        pair<int, int> temp = make_pair(i, score);
        scores.push_back(temp);
    }
    cout << endl;
    for(int i = 0; i < population.size(); ++i)
        for(int j = 0; j < population.size() - i - 1; ++j)
            if(scores[j].second < scores[j + 1].second)
                swap(scores[j], scores[j + 1]);
    for(int i = 0; i < population.size() * part / 100; i ++){
        winners.push_back(population[scores[i].first]);
        winners.push_back(mutate(population[scores[i].first], 1));
        winners.push_back(mutate(population[scores[i].first], 2));
        cout << scores[i].second<< endl;
    }
    winners.push_back(mutate(population[scores[population.size() - 3].first], 1));
    return winners;
}

void genetic_algo(vector<Schedule>&population, int duration, vector<Intersect>&Intersects, map<string, Street> street_table, vector<Car>&cars, int bonus, int part){
    vector<Schedule> winners;
    vector<pair<int, int>> scores;
    for(int i = 0; i < population.size(); ++i){
        int score = simulate(duration, Intersects, street_table, cars, population[i], bonus);
        cout << score<<" ";
        pair<int, int> temp = make_pair(i, score);
        scores.push_back(temp);
    }
    cout << endl;
    vector<int>dead;
    for(int i = 0; i < population.size(); ++i)
        for(int j = 0; j < population.size() - i - 1; ++j)
            if(scores[j].second < scores[j + 1].second)
                swap(scores[j], scores[j + 1]);
    for(int i = population.size() * part / 100; i < population.size(); ++i)
        dead.push_back(scores[i].first);
    for(int i = 0; i < population.size() * part / 100; i ++){
        population[dead.back()] = mutate(population[i], 1);
        dead.pop_back();
        population[dead.back()] = mutate(population[i], 2);
        dead.pop_back();
        cout << scores[i].second<< endl;
    }
}

int main(){
    int duration, intersections_count, streets_count, cars_count, bonus;
    // duration <= 10^4 intersections <= 10^5 cars <= 10^3
    char file_name[] = "b.txt";
    map<string, Street> street_table;
    fin.open(file_name);
    fin >> duration >> intersections_count >> streets_count >> cars_count >> bonus;
    vector<Intersect> Intersects(intersections_count);
    for(int i = 0; i < streets_count; ++i){
        Street st;
        fin >> st.from >> st.dist >> st.name >> st.length;
        street_table[st.name] = st;
        Intersects[st.from].out.push_back(st);
        Intersects[st.dist].in.push_back(st);
    }
    cout <<"street input complete" << endl;
    vector<Car> cars;
    for(int i = 0; i < cars_count; ++i){
        Car c;
        int path_len;
        char street_name[31];
        fin >> path_len;
        for(int j = 0; j < path_len; ++j){
            fin >> street_name;
            c.path.push(street_table[street_name]);
        }
        c.blocked = false;
        cars.push_back(c);
    }
    fin.close();
    cout <<"cars input complete" << endl;
    Schedule s = get_schedule(Intersects, 1);
    //show_schedule(s);
    //s = mutate(s, 2);
    //show_schedule(s);
    int population_size = 10, steps = 10;
    vector<Schedule> population;
    population.push_back(s);
    for(int i = 0; i < population_size; ++i){
        population.push_back(mutate(s, 1 + i / 5));
    }
    for(int iter = 0; iter < steps; iter ++){
        cout << "Iteration " << iter + 1 << endl;
        genetic_algo(population, duration, Intersects, street_table, cars, bonus, 30);
    }
    return 0;
}
