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
};

bool exist(vector<int> v, int a){
    for(int i = 0; i < v.size(); ++i)
        if(v[i] == a)
            return true;
    return false;
}

int get_semaphore_position(Semaphore s, int time){
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
        Semaphore s = {l, 0, 0, int(time * Intersects[i].in.size()), i};
        sch.push_back(s);
    }
    Schedule schedule = {sch, int(sch.size())};
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

int simulate(int duration, vector<Intersect> Intersects, map<string, Street> street_table, vector<Car> cars, Schedule schedule, int bonus){
    // расставить машины на стартовые позиции
    int total_score = 0;
    vector<int> active_semaphores;
    for(int i = 0; i < cars.size(); ++i){
        cars[i].path.front().length = 0;
        street_table[cars[i].path.front().name].cars.push(i);
        active_semaphores.push_back(street_table[cars[i].path.front().name].dist);
    }
    for(int t = 0; t < duration; ++t){
        vector<int> next;
        for(int i = 0; i < cars.size(); ++i){
            if(!cars[i].blocked){
                if(cars[i].path.front().length){
                    cars[i].path.front().length --;
                    if(!cars[i].path.front().length){
                        street_table[cars[i].path.front().name].cars.push(i);
                        if(! exist(next, street_table[cars[i].path.front().name].dist))
                            next.push_back(street_table[cars[i].path.front().name].dist);
                    }
                }
            }
        }
        //cout << active_semaphores.size()<<endl;
        // сложность O(duration * intersections) <= 10 ^ 9
        // Можем оптимизировать до O(duration * cars) <= 10 ^ 7
        // если итерироваться не по всем сфетофорам, а только по тем, где стоят машины
        for(int iter = 0; iter < active_semaphores.size(); ++iter){
            int i = active_semaphores[iter];
            int pos = get_semaphore_position(schedule.semaphores[i], t);
            string green_street = schedule.semaphores[i].lights[pos].name;
            if(!exist(next, i))
                next.push_back(i);
            if(!street_table[green_street].cars.empty()){
                int car_number = street_table[green_street].cars.front();
                street_table[green_street].cars.pop();
                if(street_table[green_street].cars.empty() && exist(next, i)){
                    //cout << "erase " << next.back()<<endl;
                    next.pop_back();
                }
                cars[car_number].path.pop();
                if(cars[car_number].path.size() == 1){
                    cars[car_number].blocked = true;
                    if(cars[car_number].path.front().length + t <= duration)
                        total_score += bonus +(duration - (cars[car_number].path.front().length + t));
                }
            }
        }
        active_semaphores = next;
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
    Schedule schedule = {sch, int(sch.size())};
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
        }
        s.semaphores[i].cycle = new_cycle;
    }
    return s;
}

int main(){
    int duration, intersections_count, streets_count, cars_count, bonus;
    // duration <= 10^4 intersections <= 10^5 cars <= 10^3
    char file_name[] = "d.txt";
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
    /*for(int i = 0; i < Intersects.size(); ++i){
        cout << "intersect  "<< i<<"  : ";
        for(int j = 0; j < Intersects[i].out.size(); ++j){
            cout << Intersects[i].out[j].dist<<"("<< Intersects[i].out[j].length << ")   ";
        }
        cout << endl;
    }*/
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
    for (int i = 0; i < 20; ++ i) {
        cout << i << " " << simulate(duration, Intersects, street_table, cars, s, bonus) << endl;
    }
    return 0;
}
