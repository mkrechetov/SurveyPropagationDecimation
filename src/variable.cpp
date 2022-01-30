//
// Created by mk on 12.12.2021.
//
#include "variable.h"

Variable::Variable(int new_var_id) {
    var_id = new_var_id;
    value = -1;
    var_hood = new neighborhood;
    var_hood->reserve(10); // arbitrary constant, change later !!!
    pos_hood = new neighborhood;
    pos_hood->reserve(10); // arbitrary constant, change later !!!
    neg_hood = new neighborhood;
    neg_hood->reserve(10); // arbitrary constant, change later !!!
}
Variable::Variable(int new_var_id, int degree) {
    var_id = new_var_id;
    value = -1;
    var_hood = new neighborhood;
    var_hood->reserve(degree);
    pos_hood = new neighborhood;
    pos_hood->reserve(degree);
    neg_hood = new neighborhood;
    neg_hood->reserve(degree);
}
