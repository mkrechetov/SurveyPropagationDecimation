//
// Created by mk on 12.12.2021.
//

#ifndef MYSURPROP_CLAUSE_H
#define MYSURPROP_CLAUSE_H
#include <utility>
#include <vector>
#include "edge.h"
#include <iostream>

class Clause{
public:
    int clause_id = -1;
    bool active = true;
    std::vector<int> adj_variables;
    neighborhood* clause_hood;

    explicit Clause(std::vector<int> vars) {
        for (int var_id : vars) {
            this->adj_variables.push_back(var_id);
            clause_hood = new neighborhood;
            clause_hood->reserve(vars.size());
        }
    };

    int Sign(int target_id) {
        for (int var : adj_variables){if (var == target_id) {return 1;} else if (var == -target_id) {return -1;} }
        std::cout << "Variable " << target_id << " is not in the clause " << clause_id << " !!!" << std::endl;
        return 0;
    }

};


#endif //MYSURPROP_CLAUSE_H
