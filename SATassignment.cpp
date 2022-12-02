#include <iostream>
#include <ctime>

#include "parseFiles.h"
#include "localSearch.h"
#include "WalkSAT.h"
#include "dpll.h"

using namespace std;

// function to print out the menu options for this program
void printMenu() {
    cout << endl;
    cout << "Program Options (Type the desired number): " << endl;
    cout << "0 - Stop running the program" << endl;
    cout << "1 - Print the menu again" << endl;
    cout << "2 - Solve all 350 equations using Local Search" << endl;
    cout << "3 - Solve all 350 equations using WalkSAT" << endl;
    cout << "4 - Solve all 350 equations using DPLL" << endl;
    cout << "5 - Check fitness of a certain equation with an a assignment" << endl;
    cout << endl;
}

int main()
{
    // read in the CNF files
    cout << "Reading in the test files..." << endl;
    equation equations[350]; // variable to store the data from all test files

    int count = 0;

    // CNF Formulas
    for (int i = 1; i <= 50; i++) {
        string fileName = "./TestFiles/CNF Formulas/cnfFormulas (" + to_string(i) + ").cnf";
        equations[count] = readCNFFile(fileName);
        count++;
    }

    // HARD CNF Formulas
    for (int i = 1; i <= 300; i++) {
        string fileName = "./TestFiles/HARD CNF Formulas/hardcnfFormulas (" + to_string(i) + ").cnf";
        equations[count] = readCNFFile(fileName);
        count++;
    }

    // initializing input with a value that is not present in the menu
    int input = 6;

    printMenu();

    while (input != 0) {
        cout << "Enter menu Selection:" << endl;
        cin >> input;

        switch (input) {
        case 0:
            break;
        case 1: {
            printMenu();
            break;
        }
        case 2: {
            for (int i = 0; i < 350; i++) {
                clock_t startTime = clock();
                string output = solveUsingLocalSearch(equations[i]);
                clock_t endTime = clock();
                long double timeElapsed = 1.0 * (endTime - startTime) / CLOCKS_PER_SEC;

                cout << "Equation " << i << ": " << equations[i].numClauses << " " << output << " " << to_string(timeElapsed) << endl;
            }
            break;
        }
        case 3: {
            for (int i = 0; i < 350; i++) {
                clock_t startTime = clock();
                string output = solveUsingWalkSAT(equations[i]);
                clock_t endTime = clock();
                long double timeElapsed = 1.0 * (endTime - startTime) / CLOCKS_PER_SEC;

                cout << "Equation " << i << ": " << equations[i].numClauses << " " << output << " " << to_string(timeElapsed) << endl;
            }
            break;
        }
        case 4: {
            SATDPLL dpll;
            for (int i = 0; i < 350; i++) {
                clock_t startTime = clock();
                dpll.solve(equations[i]);
                clock_t endTime = clock();
                long double timeElapsed = 1.0 * (endTime - startTime) / CLOCKS_PER_SEC;
                cout << "CPU Time Taken: " << timeElapsed << endl;
            }
            break;
        }
        case 5: {
            cout << "Which equation would you like to check?" << endl;
            int equationNum;
            cin >> equationNum;
            cout << "What is the assignment you would like to test?" << endl;
            string a;
            cin >> a;
            cout << fitness(equations[equationNum], a).numSatisfied << endl;
            break;
        }
        default: {
            printMenu();
            break;
        }
        }
    }
}
