#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <sstream>

#include "main.h"
#include "graph.h"
#include "solver.h"
#include "reduction.h"


using namespace std;

int main(int argc, char *argv[]){
  string buf;
  vector<string> input;
  int m = 10e6;
  int cnt = 0;

  while(1) {
    getline(cin,buf);
    if(buf[0] == 'c') {
      continue;
    } else if(buf[0] == 'p') {
      istringstream iss(buf);
      string s;
      int num = 0;
      while(iss >> s) {
        if(num == 3) m = stoi(s);
        num++;
      }
      input.push_back(buf);
    } else {
      input.push_back(buf);
      cnt++;
    }
    if(cnt == m) break;
  }

  // input
  Graph Gin;
  Gin = Graph(input);

  vector <edge> sol;

  //////////////////// begin

  Graph G = Gin;

/*
  vector <vector <double> > lp_sol;
  double lp_obj = lp_solve(Gin, lp_sol);
  vector <edge> sol1;
  int obj1 = lp_pivot(G, Gin, sol1, lp_sol);
  vector <edge> random_pivot_sol;
  int random_pivot_obj = random_pivot(G, Gin, random_pivot_sol);
  if(obj1 > random_pivot_obj){
    obj1 = random_pivot_obj;
    sol1 = random_pivot_sol;
  }
  */
  vector <edge> sol1;
  int obj1 = random_pivot(G, Gin, sol1);

  vector <edge> sol2;
  int red_cost = cal_reduction(G, Gin, obj1, sol2);
  red_cost += cal_ker(G, Gin, sol2);
  int rec_depth = 0;
  vector<vector<int> > uv;
  uv.clear();
  int obj2 = edge_branching(G, Gin, obj1-red_cost, sol2, rec_depth, uv);

  if (obj1 > obj2 + red_cost && obj2 != -1){
    obj1 = obj2 + red_cost;
    sol1 = sol2;
  }
  show_sol(sol2);

  return 0;
}


void show_sol(const vector <edge>& sol){
  for (auto u: sol){
    cout << u.first+1 << " " << u.second+1 << endl;
  }

  return;
}
