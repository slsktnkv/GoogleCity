#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <deque>

using namespace std;

struct Car;

struct Street {
  int B;
  int E;
  int L;
  string name;
  deque<Car*> cars;
  bool green;
};

struct Car {
  queue<Street*> path;
  int remain;
};

struct Light {
  queue<pair<Street*, int>> green;
};

int main() {
  cout << "START LOADING WORLD" << endl;
  int D, I, S, V, F;
  fstream in, sch;
  in.open("b.txt", ios_base::in);
  in >> D >> I >> S >> V >> F;

  map<string, Street> Names;

  for (int i = 0; i < S; ++i) {
    int b, e, l;
    string name;
    in >> b >> e >> name >> l;
    Street* tmp = &Names[name];
    tmp->B = b;
    tmp->E = e;
    tmp->name = name;
    tmp->L = l;
    tmp->green = false;
  }

  for (int i = 0; i < V; ++i) {
    Car* car = new Car;
    int p;
    string name;
    in >> p;
    for (int k = 0; k < p; ++k) {
      in >> name;
      car->path.push(&Names[name]);
    }
    car->remain = 0;
    car->path.front()->cars.push_back(car);
  }
  in.close();
  cout << "WORLD LOADED" << endl;

  cout << "START SCHEDULER LOADING" << endl;
  sch.open("answer_b.txt", ios_base::in);
  vector<Light> Lights(I);
  int A;
  sch >> A;
  for (int i = 0; i < A; ++i) {
    int ID, E;
    sch >> ID >> E;
    for (int k = 0; k < E; ++k) {
      int T;
      string name;
      sch >> name >> T;
      Lights[ID].green.push(pair<Street*, int>(&Names[name], T + 1));
    }
  }
  sch.close();
  vector<Light> b = Lights;
  cout << "SCHEDULER LOADED" << endl;

  cout << "START SIMULATION" << endl;
  int score = 0;
  for (int t = 0; t < D; ++t) {
    for (int i = 0; i < I; ++i){
      if (!Lights[i].green.empty()) {
        if (--Lights[i].green.front().second > 0) {
          Lights[i].green.front().first->green = true;
        } else {
          Lights[i].green.pop();
          --i;
        }
      }
    }

    for (auto it = Names.begin(); it != Names.end(); ++it) {
      if (it->second.cars.empty()) {
        continue;
      }
      for (auto car = it->second.cars.begin(); car != it->second.cars.end(); ++car) {
        if ((*car)->remain > 0) {
          --(*car)->remain;
        }
      }
    }

    for (auto it = Names.begin(); it != Names.end(); ++it) {
      if (it->second.green && !it->second.cars.empty() && it->second.cars.front()->remain == 0) {
        Car* c = it->second.cars.front();
        it->second.cars.pop_front();
        c->path.pop();
        if (c->path.size() < 2) {
          if (c->path.front()->L <= (D - t)) {
            score += F + (D - t) - c->path.front()->L;
          }
          delete c;
        } else {
          c->path.front()->cars.push_back(c);
          c->remain = c->path.front()->L;
        }
      }
    }
  }
  cout << "SIMULATION FINISHED" << endl;

  cout << score << endl;
//  fstream fout;
//  fout.open("out.txt", ios_base::out);
//  fout << A << endl;
//  for (int i = 0; i < I; ++i) {
//    if (!b[i].green.empty()) {
//      fout << i << endl;
//      fout << b[i].green.size() << endl;
//      do {
//        fout << b[i].green.front().first->name << " " << b[i].green.front().second << endl;
//        b[i].green.pop();
//      } while (!b[i].green.empty());
//    }
//  }
//  fout.close();
    return 0;
}
