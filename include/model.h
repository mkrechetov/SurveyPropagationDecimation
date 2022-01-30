//
// Created by mk on 12.12.2021.
//

#ifndef MYSURPROP_MODEL_H
#define MYSURPROP_MODEL_H
#include "variable.h"
#include "clause.h"

#include <vector>
#include <string>
#include <list>

class Model {
public:
    std::vector<Variable> all_vars; // remember that var_id starts from 1 !!!
    std::vector<Clause> all_clauses;
    std::vector<Edge> all_edges;
    std::vector<int> edge_order;

//    std::list<int> active_vars;
//    std::list<int> active_clauses;

    double precision = 0.01;
    double ro = 1.0;
    double round_precision = 0.5;
    int max_iter = 1;
    bool VERBOSE = true;
    int decimated = 0;

    void summary();
    Model(std::string filename);
    void save_subformula(const std::string& filename);

    void update_variable(int v);
    void update_clause(int c, double &deviation);
    void update_edge(int edge_id);

    void compute_marginals();
    void clear_variable(int v);
    void decimate();

    void edge_solve();
    int solve();

    void set_edge_order(std::string order);
    std::vector<int> random_spanning_tree();

};
#endif //MYSURPROP_MODEL_H
