#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include "random.hpp"
#include <chrono>
#include "main.h"
#include "graph.h"
#include "solver.h"
//#include <glpk.h>
using namespace std;

int edge_branching(Graph& G, const Graph& G_orig,int max_obj, vector <edge>& sol, int rec_depth, vector<vector<int>>& uv){

  if(G.num_nodes <= 1)  return 0;

  if(G.num_nodes == 2){
    if(G.Weight(0,1) > 0 && G.Flag(0,1) == 0){
      G.permanent(0, 1, sol, G_orig);
      G.reset_flag(0,1);
    }
    if(G.Weight(0,1) <= 0 && G.Flag(0,1) == 0){
      G.forbid(0, 1, sol, G_orig);
      G.reset_flag(0,1);
    }
    return 0;
  }

  vector <int> triple;

  if(!G.conflict_triple(triple)){
    FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
      if(G.Weight(u,v) > 0 && G.Flag(u,v) == 0){
        G.permanent(u, v, sol, G_orig);
        G.reset_flag(u,v);
      }
      if(G.Weight(u,v) <= 0 && G.Flag(u,v) == 0){
        G.forbid(u, v, sol, G_orig);
        G.reset_flag(u,v);
      }

    }
    return 0;
  }

  if(max_obj <= 0)  return -1;

  int best = -1;
  vector <edge> best_sol;


  if (rec_depth % 10 == 0){
    uv.clear();
    FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
      if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
      vector<int> s2_tmp;
      int cost = 0;
      FOR(z, 0, G.num_nodes){
        if(u == z || v == z) continue;
        if((G.Weight(u,z) > 0 && G.Weight(v,z) <= 0) || (G.Weight(u,z) <= 0 && G.Weight(v,z) > 0)){
          cost += min(abs(G.Weight(u,z)), abs(G.Weight(v,z)));
        }
      }
      s2_tmp.push_back(cost);
      s2_tmp.push_back(G.node_names[u]);
      s2_tmp.push_back(G.node_names[v]);
      uv.push_back(s2_tmp);
      s2_tmp.clear();
    }
    sort(uv.begin(), uv.end(), [](const vector<int> &alpha, const vector<int > &beta){return alpha[0] > beta[0];});
  }
  bool flag = false;
  if (uv.size() > 0) {
    FOR (i,0,uv.size()) {
      int u_name = uv[i][1];
      int v_name = uv[i][2];
      int u = G.name_to_ind[u_name];
      int v = G.name_to_ind[v_name];

      if (u == -1 || v == -1) continue;
      if(G.weight[u_name][v_name] <= 0 || G.flag[u_name][v_name] != 0) continue;
      if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
      flag = true;
      bool merge_flag = true;
      FOR(z, 0, G.num_nodes){
        if(u_name == z || v_name == z) continue;
        if((G.flag[u_name][z] == -1 && G.flag[v_name][z] == 1) || (G.flag[u_name][z] == 1 && G.flag[v_name][z] == -1)) merge_flag = false;
      }

      if(u_name > v_name) SWAP(int, u_name, v_name);
      int w = G.Weight(u,v);
      G.forbid(u, v, best_sol, G_orig);
      G.flip_edge(u,v);

      best = edge_branching(G, G_orig, max_obj-w, best_sol, rec_depth+1, uv);
      if(best != -1) {
        best += w;
        if(best < max_obj) max_obj = best;
      }
      G.reset_flag(G.name_to_ind[u_name],G.name_to_ind[v_name]);
      G.flip_edge(G.name_to_ind[u_name],G.name_to_ind[v_name]);


      if(merge_flag){
        vector <edge> tmpsol;
        G.permanent(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, G_orig);
        MergeData mg(G_orig.num_nodes);
        int mergecost = G.merge_nodes(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, mg, G_orig);
        int cost = edge_branching(G, G_orig, max_obj-mergecost, tmpsol, rec_depth+1, uv);
        if((cost != -1) && (best == -1 || cost + mergecost < best)){
          best = cost + mergecost;
          best_sol = tmpsol;
        }

        G.expand_nodes(u_name, v_name, mg);
        G.reset_flag(G.name_to_ind[u_name],G.num_nodes-1);
      }

      if(best != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
      return best;
    }
  }

    if(flag == false){
    uv.clear();
    //auto start = system_clock::now();
    FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
      if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
      vector<int> s2_tmp;
      int cost = 0;
      FOR(z, 0, G.num_nodes){
        if(u == z || v == z) continue;
        if((G.Weight(u,z) > 0 && G.Weight(v,z) <= 0) || (G.Weight(u,z) <= 0 && G.Weight(v,z) > 0)){
          cost += min(abs(G.Weight(u,z)), abs(G.Weight(v,z)));
        }
      }
      s2_tmp.push_back(cost);
      s2_tmp.push_back(G.node_names[u]);
      s2_tmp.push_back(G.node_names[v]);
      uv.push_back(s2_tmp);
      s2_tmp.clear();
    }
    sort(uv.begin(), uv.end(), [](const vector<int> &alpha, const vector<int > &beta){return alpha[0] > beta[0];});
  }
  if (uv.size() > 0) {
    FOR (i,0,uv.size()) {
      int u_name = uv[i][1];
      int v_name = uv[i][2];
      int u = G.name_to_ind[u_name];
      int v = G.name_to_ind[v_name];

      if (u == -1 || v == -1) continue;
      if(G.weight[u_name][v_name] <= 0 || G.flag[u_name][v_name] != 0) continue;
      if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
      flag = true;
      bool merge_flag = true;
      FOR(z, 0, G.num_nodes){
        if(u_name == z || v_name == z) continue;
        if((G.flag[u_name][z] == -1 && G.flag[v_name][z] == 1) || (G.flag[u_name][z] == 1 && G.flag[v_name][z] == -1)) merge_flag = false;
      }

      if(u_name > v_name) SWAP(int, u_name, v_name);
      int w = G.Weight(u,v);
      G.forbid(u, v, best_sol, G_orig);
      G.flip_edge(u,v);

      best = edge_branching(G, G_orig, max_obj-w, best_sol, rec_depth+1, uv);
      if(best != -1) {
        best += w;
        if(best < max_obj) max_obj = best;
      }
      G.reset_flag(G.name_to_ind[u_name],G.name_to_ind[v_name]);
      G.flip_edge(G.name_to_ind[u_name],G.name_to_ind[v_name]);


      if(merge_flag){
        vector <edge> tmpsol;
        G.permanent(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, G_orig);
        MergeData mg(G_orig.num_nodes);
        int mergecost = G.merge_nodes(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, mg, G_orig);
        int cost = edge_branching(G, G_orig, max_obj-mergecost, tmpsol, rec_depth+1, uv);
        if((cost != -1) && (best == -1 || cost + mergecost < best)){
          best = cost + mergecost;
          best_sol = tmpsol;
        }

        G.expand_nodes(u_name, v_name, mg);
        G.reset_flag(G.name_to_ind[u_name],G.num_nodes-1);
      }

      if(best != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
      return best;
    }
  }
}



int random_pivot(Graph& G, const Graph& G_orig, std::vector <edge>& sol){
  rnd.seed(RANDOM_SEED);
  int best_cost = -1;
  vector <edge> best_sol;

  REP(t, NUM_PIVOT){
    int n = G.num_nodes;
    vector <int> nodes;
    REP(i, n) nodes.push_back(i);
    vector <edge> tmpsol;
    int cost = 0;

    while(n > 1){
      int k = rnd(0, n-1);
      int pivot = nodes[k];
      vector <int> neighbor;
      nodes.erase(nodes.begin()+k);
      n--;

      for(int i=n-1; i >= 0; i--){
        if(G.Weight(nodes[i],pivot) > 0) {
          neighbor.push_back(nodes[i]);
          if(G.Flag(nodes[i],pivot) == 0){
            G.permanent(nodes[i],pivot,sol,G_orig);
            G.reset_flag(nodes[i], pivot);
          }
          nodes.erase(nodes.begin()+i);
        }
      }
      n -= neighbor.size();

      if(neighbor.size() > 1){
        FOR(i,0, (int) neighbor.size()-1) FOR(j, i+1, (int) neighbor.size()){
          if(G.Flag(neighbor[i],neighbor[j]) == 0){
            G.permanent(neighbor[i],neighbor[j],sol,G_orig);
            G.reset_flag(neighbor[i], neighbor[j]);
          }

          if(G.Weight(neighbor[i],neighbor[j]) < 0){
            cost-= G.Weight(neighbor[i],neighbor[j]);
          }
        }
      }
      if(neighbor.size() > 0 && n > 0){
        REP(i, (int) neighbor.size()) REP(j, n){
          if(G.Flag(neighbor[i],nodes[j]) == 0){
            G.forbid(neighbor[i],nodes[j],sol,G_orig);
            G.reset_flag(neighbor[i],nodes[j]);
          }

          if(G.Weight(neighbor[i],nodes[j]) > 0){
            cost += G.Weight(neighbor[i],nodes[j]);
          }
        }
      }
    }
    if(best_cost == -1 || cost < best_cost){
      best_cost = cost;
      best_sol = tmpsol;
    }
  }

  if(best_cost != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  return best_cost;
}

/*
// lp_solve glpk version
double lp_solve(const Graph &G, vector <vector <double> >& lp_sol){
  int n = G.num_nodes;
  lp_sol = vector <vector <double> > (n, vector <double>(n, 0));

  glp_prob *lp;
  glp_smcp parm;
  glp_init_smcp(&parm);
  parm.msg_lev = GLP_MSG_OFF;
  double obj=0;

  lp = glp_create_prob();
  glp_set_obj_dir(lp, GLP_MIN);
  glp_add_cols(lp, n*(n-1)/2);
  FOR(i, 0, n-1) FOR(j, i+1, n){
    // ij corresponds to index i(2n-i-1)/2 + j - i
    glp_set_col_bnds(lp, i*(2*n-i-1)/2+j-i, GLP_DB, 0.0, 1.0);
    //glp_set_col_kind(lp, i*(2*n-i-1)/2+j-i, GLP_BV);
    if(G.Weight(i,j) > 0) glp_set_obj_coef(lp, i*(2*n-i-1)/2+j-i, 1.0);
    else{
      glp_set_obj_coef(lp, i*(2*n-i-1)/2+j-i, -1.0);
      obj += 1;
    }
  }

  glp_add_rows(lp, n*(n-1)*(n-2)/2);
  int ia[1+3*n*(n-1)*(n-2)/2], ja[1+3*n*(n-1)*(n-2)/2];
  double ar[1+3*n*(n-1)*(n-2)/2];
  int ind = 1;
  FOR(i, 0, n-1) FOR(j, i+1, n) FOR(k, 0, n){
    if(k == i || k == j) continue;
    glp_set_row_bnds(lp, ind, GLP_UP, 0.0, 0);
    ia[3*ind-2] = ind;
    ja[3*ind-2] = i*(2*n-i-1)/2+j-i;
    ar[3*ind-2] = 1;
    ia[3*ind-1] = ind;
    if(i < k) ja[3*ind-1] = i*(2*n-i-1)/2+k-i;
    else ja[3*ind-1] = k*(2*n-k-1)/2+i-k;
    ar[3*ind-1] = -1;
    ia[3*ind] = ind;
    if(j < k) ja[3*ind] = j*(2*n-j-1)/2+k-j;
    else ja[3*ind] = k*(2*n-k-1)/2+j-k;
    ar[3*ind] = -1;
    ind++;
  }

  glp_load_matrix(lp, 3*n*(n-1)*(n-2)/2, ia, ja, ar);
  glp_simplex(lp, &parm);
  //glp_intopt(lp, NULL);
  obj += glp_get_obj_val(lp);
  glp_delete_prob(lp);
  //cout << "lp_obj: " << obj << endl;

  if(glp_get_status(lp) != GLP_OPT && glp_get_status(lp) != GLP_FEAS) cerr << "lp solver error" << endl;

  FOR(i, 0, n-1) FOR(j, i+1, n){
    lp_sol[i][j] = glp_get_col_prim(lp, i*(2*n-i-1)/2+j-i);
    // cout << i << "," << j << ": " << lp_sol[i][j] << endl;
  }

  return obj;
}


int lp_pivot(Graph &G,  const Graph& G_orig, vector <edge>& sol, const vector <vector <double> >& lp_sol){
  rnd.seed(RANDOM_SEED);
  int best_cost = -1;
  vector <edge> best_sol;

  REP(t, NUM_PIVOT){
    int n = G.num_nodes;
    vector <int> nodes;
    REP(i, n) nodes.push_back(i);
    vector <edge> tmpsol;
    int cost = 0;

    while(n > 1){
      int k = rnd(0, n-1);
      int pivot = nodes[k];
      vector <int> neighbor;
      nodes.erase(nodes.begin()+k);
      n--;
      neighbor.push_back(pivot);

      for(int i=n-1; i >= 0; i--){
        if(rnd(0,0xffffff) > lp_sol[nodes[i]][pivot] * 0xffffff) {
          neighbor.push_back(nodes[i]);
          nodes.erase(nodes.begin()+i);
          n--;
        }
      }

      if(neighbor.size() > 1){
        FOR(i,0, (int) neighbor.size()-1) FOR(j, i+1, (int) neighbor.size()){
          if(G.Flag(neighbor[i],neighbor[j]) == 0){
            G.permanent(neighbor[i],neighbor[j],sol,G_orig);
            G.reset_flag(neighbor[i], neighbor[j]);
          }
          if(G.Weight(neighbor[i],neighbor[j]) < 0){
            cost-= G.Weight(neighbor[i],neighbor[j]);
          }
        }
      }
      if(neighbor.size() > 0 && n > 0){
        REP(i, (int) neighbor.size()) REP(j, n){
          if(G.Flag(neighbor[i],nodes[j]) == 0){
            G.forbid(neighbor[i],nodes[j],sol,G_orig);
            G.reset_flag(neighbor[i],nodes[j]);
          }
          if(G.Weight(neighbor[i],nodes[j]) > 0){
            cost += G.Weight(neighbor[i],nodes[j]);
          }
        }
      }
    }
    if(best_cost == -1 || cost < best_cost){
      best_cost = cost;
      best_sol = tmpsol;
    }
  }


  if(best_cost != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  return best_cost;
}
*/