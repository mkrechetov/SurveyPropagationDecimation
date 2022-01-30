//
// Created by mk on 14.12.2021.
//
#ifndef MYSURPROP_EDGE_H
#define MYSURPROP_EDGE_H
#include <vector>
class Edge {
public:
    int var;
    int clause;
    bool J;
    double eta, pu, ps, po;
    Edge(int in_var, int in_clause, bool in_J);
};
typedef std::vector<int> neighborhood;
#endif //MYSURPROP_EDGE_H
