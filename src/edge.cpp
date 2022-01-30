#include "edge.h"
Edge::Edge(int in_var, int in_clause, bool in_J) {
    this->var = in_var;
    this->clause = in_clause;
    this->J = in_J;
    this->eta = 0.0;
    this->pu = 0.0;
    this->ps = 0.0;
    this->po = 1.0;
}