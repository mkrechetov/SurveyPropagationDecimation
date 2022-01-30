//
// Created by mk on 12.12.2021.
//

#include "model.h"
#include "edge.h"
#include <iostream>
#include <fstream> // reading from file
#include <sstream> // reading from file line by line

#include <random> // for shaking messages
#include <algorithm>  // for shuffle
#include <map>

void Model::summary() {
    std::cout << "CNF summary:" << std::endl;
    std::cout << "#Variables: " << all_vars.size() << std::endl;
    std::cout << "#Clauses: " << all_clauses.size() << std::endl;
    std::cout << "#Edges: " << all_edges.size() << std::endl;
};

Model::Model(std::string filename) {
    std::ifstream infile(filename);
    std::stringstream iss;
    std::string line;

    // read number of variables and clauses
    std::getline(infile, line);
    iss.str(line);
    int var_num, clause_num;
    std::string p, cnf;
    if (!(iss >> p >> cnf >> var_num >> clause_num)) {
        std::cout << "*.fg file should start with number of variables and factors!" << std::endl;
        std::abort();
    }
    all_vars.reserve(var_num);
    all_clauses.reserve(clause_num);
    all_edges.reserve(clause_num);
    edge_order.reserve(clause_num);
    iss.clear();

    for (int v=0; v<var_num+1; ++v) {all_vars.emplace_back(Variable(v)); }//active_vars.emplace_back(v);}

    for (int c=0; c<clause_num; ++c) {
        std::getline(infile, line);
        iss.str(line);
        int new_var;
        std::vector<int> vars = {};
        std::vector<int> edges = {};
        int e;
        while (iss >> new_var) {
            if (new_var == 0) {
                break;
            }
            else {
                vars.emplace_back(new_var);
                e = all_edges.size();
                edges.push_back(e);
                if (new_var > 0) {
                    Edge new_edge(std::abs(new_var), all_clauses.size(), false);
                    this->all_vars[std::abs(new_var)].var_hood->emplace_back(e);

                    this->all_vars[std::abs(new_var)].pos_hood->emplace_back(e);
                    all_edges.push_back(new_edge);
                }
                else if (new_var < 0) {
                    Edge new_edge(std::abs(new_var), all_clauses.size(), true);
                    this->all_vars[std::abs(new_var)].var_hood->emplace_back(e);

                    this->all_vars[std::abs(new_var)].neg_hood->emplace_back(e);
                    all_edges.push_back(new_edge);
                }
                edge_order.push_back(e);
            }
        }
        iss.clear();

        Clause new_clause(vars);
        new_clause.clause_id = all_clauses.size();
        for (int e : edges) {new_clause.clause_hood->emplace_back(e);}
        all_clauses.emplace_back(new_clause);
//        active_clauses.emplace_back(new_clause.clause_id);
    }
}

void Model::update_variable(int var_id) {
    for (int edge_id : *this->all_vars[var_id].var_hood) {
        if (this->all_clauses[this->all_edges[edge_id].clause].active) {
            double aggregated_neg_eta=1.0, aggregated_pos_eta=1.0;

            for (int other_edge_id : *this->all_vars[var_id].pos_hood) {
                if (other_edge_id != edge_id) {
                    if (this->all_clauses[this->all_edges[other_edge_id].clause].active) {
                        aggregated_pos_eta *= (1.0 - this->all_edges[other_edge_id].eta);
                    }
                }
            }

            for (int other_edge_id : *this->all_vars[var_id].neg_hood) {
                if (other_edge_id != edge_id) {
                    if (this->all_clauses[this->all_edges[other_edge_id].clause].active) {
                        aggregated_neg_eta *= (1.0 - this->all_edges[other_edge_id].eta);
                    }
                }
            }

            if (this->all_edges[edge_id].J) {
                this->all_edges[edge_id].pu = (1.0-this->ro*aggregated_pos_eta)*aggregated_neg_eta;
                this->all_edges[edge_id].ps = (1.0-aggregated_neg_eta)*aggregated_pos_eta;
                this->all_edges[edge_id].po = aggregated_pos_eta*aggregated_neg_eta;
            } else {
                this->all_edges[edge_id].pu = (1.0-this->ro*aggregated_neg_eta)*aggregated_pos_eta;
                this->all_edges[edge_id].ps = (1.0-aggregated_pos_eta)*aggregated_neg_eta;
                this->all_edges[edge_id].po = aggregated_pos_eta*aggregated_neg_eta;
            }

//            if (this->all_edges[edge_id].pu+this->all_edges[edge_id].ps+this->all_edges[edge_id].po == 0.0) {
//                std::cout << "Reached infeasibility while updating variable " << var_id << "!!!" << std::endl;
//                break;
//            }
        }
    }
}

void Model::update_clause(int clause_id, double &deviation) {
    for (int edge_id : *this->all_clauses[clause_id].clause_hood) {
        if (this->all_vars[this->all_edges[edge_id].var].value == -1) {
            double new_eta = 1.0;
            for (int other_edge_id : *this->all_clauses[clause_id].clause_hood) {
                if (edge_id != other_edge_id) {
                    double pi_sum = this->all_edges[other_edge_id].pu+this->all_edges[other_edge_id].ps+this->all_edges[other_edge_id].po;
                    if (this->all_vars[this->all_edges[other_edge_id].var].value == -1) {
                        if (pi_sum == 0.0) {
                            new_eta = 0.0;
                        } else {
                            new_eta *= this->all_edges[other_edge_id].pu;
                            new_eta /= pi_sum;
                        }
                    }
                }
            }
            deviation += std::abs(new_eta - this->all_edges[edge_id].eta);
            this->all_edges[edge_id].eta = new_eta;
        }
    }
}

void Model::update_edge(int edge_id) {

}

void Model::edge_solve(){
    for (int iter=0; iter<this->max_iter; ++iter){
        std::cout << "Iteration " << iter << std::endl;

        double deviation = 0.0;
//        int edge_counter = 0;
        for (int edge_id : this->edge_order) {
            this->update_variable(this->all_edges[edge_id].var);
            this->update_clause(this->all_edges[edge_id].clause, deviation);
//            edge_counter += 1;
//            if (edge_counter % 10000 == 0) {
//                std::cout << edge_counter << std::endl;
//                std::cout << "Deviation = " << std::to_string(deviation) << std::endl;
//            }
        }

        if (deviation <= this->precision) {
            std::cout << "SP converged in " << iter << " steps" << std::endl;
            break;
        }

    }

}

int Model::solve(){// code 1 is when SP is stable (converge in 0 steps)
    for (int iter=0; iter<this->max_iter; ++iter){
        std::cout << "Iteration " << iter << std::endl;

        double deviation = 0.0;
        for (int v=0; v<this->all_vars.size(); ++v) {
            if (this->all_vars[v].value == -1) {
                this->update_variable(v);
            }
        }

        for (int c=0; c<this->all_clauses.size(); ++c) {
            if (this->all_clauses[c].active) {
                this->update_clause(c, deviation);
            }
        }
//        for (int v : this->active_vars){this->update_variable(v);}
//        for (int c : this->active_clauses) { this->update_clause(c, deviation); }

        std::cout << "Deviation = " << std::to_string(deviation) << std::endl;

        if (deviation <= this->precision) {
            std::cout << "SP converged in " << iter << " steps" << std::endl;
            if (iter == 0) {this->save_subformula("subformula.cnf");}
            return 1;
        }
    }
    return 0;
}

void Model::compute_marginals() {
    for (auto & all_var : this->all_vars) {
        if (all_var.value == -1) {
            double aggregated_u_eta=1.0, aggregated_s_eta=1.0;
            double pu, ps, po;

            for (int edge_id : *all_var.pos_hood) {
                aggregated_s_eta *= (1.0-this->all_edges[edge_id].eta);
            }

            for (int edge_id : *all_var.neg_hood) {
                aggregated_u_eta *= (1.0-this->all_edges[edge_id].eta);
            }

            pu = (1-this->ro*aggregated_u_eta)*aggregated_s_eta;
            ps = (1-this->ro*aggregated_s_eta)*aggregated_u_eta;
            po = aggregated_s_eta*aggregated_u_eta;

            all_var.W0 = pu/(pu+ps+po);
            all_var.W1 = ps/(pu+ps+po);
        }
    }
}

void Model::clear_variable(int v) {
    if (this->all_vars[v].W0 >= this->all_vars[v].W1) {
        this->all_vars[v].value = 0;
        for (int edge_id : *this->all_vars[v].var_hood) {
            if (this->all_clauses[this->all_edges[edge_id].clause].Sign(v) == -1) {
                this->all_clauses[this->all_edges[edge_id].clause].active = false;
            }
        }
    }
    else {
        this->all_vars[v].value = 1;
        for (int edge_id : *this->all_vars[v].var_hood) {
            if (this->all_clauses[this->all_edges[edge_id].clause].Sign(v) == 1) {
                this->all_clauses[this->all_edges[edge_id].clause].active = false;
            }
        }
    }
    this->decimated += 1;
}

void Model::decimate(){
//    this->solve();
    this->compute_marginals();
    bool smth_to_decimate = true;

    while (smth_to_decimate) {
        smth_to_decimate = false;
        for (auto & all_var : this->all_vars) {
            if (all_var.value == -1) {
                if (std::abs(all_var.W1 - all_var.W0) >= this->round_precision) {
//                    std::cout << "Decimating " << all_var.var_id <<  std::endl;
                    if (all_var.var_id >= 2913 && all_var.var_id < 2913+32) {
                        std::cout << "HOORAY !!! " << std::endl;
                    }
                    this->clear_variable(all_var.var_id);
                    smth_to_decimate = true;
                }
            }
        }
    }
}

void Model::set_edge_order(std::string order){
    // sets this->edge_order.
    this->edge_order.clear();
    for (int e=0; e<all_edges.size(); ++e) {this->edge_order.emplace_back(e);}

    if (order == "forward") {
        // pass
    }
    else if (order == "backward") {
        std::reverse(this->edge_order.begin(),this->edge_order.end());
    }
    else if (order == "random") {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(this->edge_order.begin(), this->edge_order.end(), g);
    }
    else if (order == "spantree") {
        this->edge_order = this->random_spanning_tree();
    }
    else {
        if (VERBOSE) {std::cout << "Unknown edge order. Setting to forward." << std::endl;}
    }
}

std::vector<int> Model::random_spanning_tree(){
    std::vector<int> edges_indices, spanning_tree, var_occ, clause_occ;
    for (int e=0; e<all_edges.size(); ++e) {edges_indices.push_back(e);}
    for (int v=0; v<all_vars.size(); ++v) {var_occ.push_back(0);}
    for (int f=0; f<all_clauses.size(); ++f) {clause_occ.push_back(0);}

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(edges_indices.begin(), edges_indices.end(), g);


    for (int e=0; e<all_edges.size(); ++e) {
        if ((var_occ[all_edges[edges_indices[e]].var] <= 1) and (clause_occ[all_edges[edges_indices[e]].clause] <= 1)) {
            spanning_tree.push_back(e);
            var_occ[all_edges[edges_indices[e]].var] += 1;
            clause_occ[all_edges[edges_indices[e]].clause] += 1;
        }
    }

    return spanning_tree;
}

void Model::save_subformula(const std::string& filename) {
    std::map<int, int> free_vars;
    for (Variable& var:this->all_vars){if(var.value==-1&&var.var_id!=0){free_vars[var.var_id] = free_vars.size()+1;}}

    std::vector<int> free_clauses;
    for (Clause& cl : this->all_clauses){if (cl.active) {free_clauses.emplace_back(cl.clause_id);}}

    std::ofstream o(filename);
    o << "p cnf " << free_vars.size() << " " << free_clauses.size() << std::endl;
    for (int cl:free_clauses) {
        for (int var_id : this->all_clauses[cl].adj_variables) {
            if (this->all_vars[std::abs(var_id)].value == -1) {
                if (var_id < 0) {
                    o << -1*free_vars[-1*var_id] << " ";
                } else {
                    o << free_vars[var_id] << " ";
                }
            }
        }
        o << 0 << std::endl;
    }

    std::cout << "New nid: " << free_vars[2913] << std::endl;
}


