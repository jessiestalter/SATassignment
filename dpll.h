#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdint>

#include "parseFiles.h"

using namespace std;

//need to store some flag states for sat, unsat, ongoing, completed
enum satFlag {
    sat, unsat, ongoing, completed
};

class Formula {
public:
    // a vector that stores the value assigned to each variable, where
    // -1 = unassigned, 0 = true, 1 = false
    vector<int> vars;
    vector<int> var_frequency; // how many time a var occurs

    // stores an int if >0 more vars are pos (true), <0 more vars are neg (false)
    vector<int> var_polarity;

    // vector to store the clauses
    // if variable is pos stored as 2n, if neg stored as 2n+1
    vector<vector<int>> clauses;

    Formula() {}

    // copy constructor for copying a formula - each member is copied over
    Formula(const Formula& f) {
        vars = f.vars;
        clauses = f.clauses;
        var_frequency = f.var_frequency;
        var_polarity = f.var_polarity;
    }
};


class SATDPLL {
private:
    Formula formula;                // the initial formula given as input
    int literal_count;              // the number of variables in the formula
    int clause_count;               // the number of clauses in the formula
    int unitPropagate(Formula&);    // performs unit propagation
    int recursiveDPLL(Formula, int num_OG_clauses);     // performs DPLL recursively
    vector <int> applyValue(Formula&, int);  // applies the value of the literal in every clause
    void showResult(Formula&, int, int num_OG_clauses); // displays the result
public:
    SATDPLL() {}
    void setup(equation eq); // takes parser output as input and sets up the variables
    void solve(equation eq);            // calls the solver
};


void SATDPLL::setup(equation eq) {

    // set the vectors to their appropriate sizes and initial values
    formula.vars.clear();
    formula.vars.resize(eq.numVars, -1);
    formula.clauses.clear();
    formula.clauses.resize(eq.numClauses);
    formula.var_frequency.clear();
    formula.var_frequency.resize(eq.numVars, 0);
    formula.var_polarity.clear();
    formula.var_polarity.resize(eq.numVars, 0);


    int var;

    for (int i = 0; i < eq.numClauses; i++) {
        for (int j = 0; j < eq.clauses[i].size(); j++) {
            var = eq.clauses[i][j];     // grab the variables out of the parser 
            if (var > 0) {
                formula.clauses[i].push_back(2 * (var - 1));    //store as 2n (-1 to make it 0 oriented)

                //increment freq and polarity
                formula.var_frequency[var - 1]++;
                formula.var_polarity[var - 1]++;

            }
            else if (var < 0) {
                formula.clauses[i].push_back(2 * ((-1) * var - 1) + 1); //store as 2n+1

                //increment freq and polarity
                formula.var_frequency[-1 - var]++;
                formula.var_polarity[-1 - var]--;
            }
        }
    }
}

//'solves' a formula
//input = formula, output = satFlag state
int SATDPLL::unitPropagate(Formula& f) {
    bool unit_clause_found = false;     //stores if the unit clause is found

    if (f.clauses.size() == 0) {
        return satFlag::sat;    //since there are no clauses it is sat 
    }

    do {
        unit_clause_found = false;

        for (int i = 0; i < f.clauses.size(); i++) {
            if (f.clauses.size() == 1) {
                unit_clause_found = true;   //if size of the clause is 1 then it is a unit clause

                f.vars[f.clauses[i][0] / 2] = f.clauses[i][0] % 2;  //0 = true, 1 = false
                f.var_frequency[f.clauses[i][0] / 2] = -1;  //it was assigned so mark it as assigned with -1 

                vector<int> result = applyValue(f, f.clauses[i][0] / 2);    //apply the change

                //return the result 
                if (result[0] == satFlag::sat || result[0] == satFlag::unsat) {
                    return result[0];
                }
                break;  //exit loop if it is not sat or unsat yet
            }
            else if (f.clauses[i].size() == 0) {
                return satFlag::unsat;  //if clause is empty return unsat 

            }
        }
    } while (unit_clause_found);

    return satFlag::ongoing;
}


vector <int> SATDPLL::applyValue(Formula& f, int var_to_apply) {
    int value_to_apply = f.vars[var_to_apply];      //value to appy either 0=T or 1=F

    int clauses_solved = 0;

    vector <int> return_vec;

    //loop through clauses
    for (int i = 0; i < f.clauses.size(); i++) {
        //loop through varibales in the clause 
        for (int j = 0; j < f.clauses[i].size(); j++) {
            //if the var is found in a clause set it to true if pos or false if neg
            if ((2 * var_to_apply + value_to_apply) == f.clauses[i][j]) {   //if the variable is same T=T or F=F
                f.clauses.erase(f.clauses.begin() + i); //remove the clause from the vector
                i--;    //reset counter

                clauses_solved++;

                if (f.clauses.size() == 0) {
                    return_vec = { satFlag::sat , clauses_solved };
                    return return_vec;    //if no clauses exist it is sat
                }
                break;  //go to next cluase
            }
            else if (f.clauses[i][j] / 2 == var_to_apply) {     //if var is different T=F or F=T
                f.clauses[i].erase(f.clauses[i].begin() + j);   //remove clause
                j--;    //reset counter

                if (f.clauses[i].size() == 0) {
                    return_vec = { satFlag::unsat , clauses_solved };
                    return return_vec;      //if clause is empty it is unsat at this time
                }
                break;  //go to next clause
            }
        }
    }
    //cout << endl << clauses_solved << endl;
    return_vec = { satFlag::ongoing , clauses_solved };
    return return_vec;   //if it reaches here it is still going

}

int SATDPLL::recursiveDPLL(Formula f, int num_OG_clauses) {
    int result = unitPropagate(f);

    if (result == satFlag::sat) {   //if its is sat return that it is sat with f
        showResult(f, result, num_OG_clauses);
        return satFlag::completed;
    }
    else if (result == satFlag::unsat) { //if it is unsat at this moment then continue with ongoing
        return satFlag::ongoing;
    }

    //find the variable that appears the most
    int i = distance(f.var_frequency.begin(), max_element(f.var_frequency.begin(), f.var_frequency.end()));

    //need to apply twice one with true other false
    for (int j = 0; j < 2; j++) {
        Formula new_f = f;
        if (new_f.var_polarity[i] > 0) {    //if it appears more true apply true
            new_f.vars[i] = j;
        }
        else {
            new_f.vars[i] = (j + 1) % 2;    //if it appears more false apply false
        }

        new_f.var_frequency[i] = -1;        //set to -1 to ignore

        vector<int> new_result = applyValue(new_f, i);

        if (new_result[0] == satFlag::sat) {
            showResult(new_f, new_result[0], num_OG_clauses);
            return satFlag::completed;
        }
        else if (new_result[0] == satFlag::unsat) {
            continue;
        }

        int dpll_result = recursiveDPLL(new_f, num_OG_clauses);

        if (dpll_result == satFlag::completed) {
            return dpll_result;
        }
    }

    return satFlag::ongoing;

}


void SATDPLL::showResult(Formula& f, int result, int num_OG_clauses) {
    if (result == satFlag::sat) {
        // if it is satisfied all clauses were solved
        cout << "Satisfied" << "  Number Solved Clauses: " << num_OG_clauses << endl;

        //prints all the formulas
        /*for (int i = 0; i < f.vars.size(); i++) {
            if (i != 0) {
                cout << " ";
            }
            if (f.vars[i] != -1) {
                cout << pow(-1, f.vars[i]) * (i + 1);
            }
            else {
                cout << (i + 1);
            }
        }
        cout << " 0";
        */
        
    }
    else {
        int solved_clauses = num_OG_clauses + f.clauses.size();
        cout << "Unsatisfied" << " Number Solved Clauses: " << abs(solved_clauses) << endl;
    }
}

void SATDPLL::solve(equation eq) {

    setup(eq);

    int result = recursiveDPLL(formula, eq.numClauses);
    if (result == satFlag::ongoing) {
        showResult(formula, satFlag::unsat, eq.numClauses);
    }
}

/*
int main(equation eq) {
    SATDPLL solver;
    solver.setup(eq);
    solver.solve();
}
*/



