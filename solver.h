
// return:
//  - the opt val if it is <= max_obj
//  - -1 if opt val > max_obj
// assumption: max_obj >= 0

int random_pivot(Graph& G, const Graph& G_orig, std::vector <edge>& sol);


int edge_branching(Graph& G, const Graph& G_orig, int max_obj, std::vector <edge>& sol, int rec_depth, std::vector<std::vector<int>>& uv);

//double lp_solve(const Graph &G, std::vector <std::vector <double> >& lp_sol);

//int lp_pivot(Graph &G,  const Graph& G_orig, std::vector <edge>& sol, const std::vector <std::vector <double> >& lp_sol);