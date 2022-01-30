//
// Created by mk on 12.12.2021.
//

#ifndef MYSURPROP_VARIABLE_H
#define MYSURPROP_VARIABLE_H
#include <vector>
#include <clause.h>

class Variable{
public:
    int var_id;
    int value = -1;
    neighborhood* var_hood;
    neighborhood* pos_hood;
    neighborhood* neg_hood;
    double W0 = 0.0, W1 = 0.0;

    Variable(int new_var_id);
    Variable(int new_var_id, int degree);

    void add_clause(Clause &cl);
};

#endif //MYSURPROP_VARIABLE_H
