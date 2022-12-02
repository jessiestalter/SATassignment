#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

#include "parseFiles.h"

using namespace std;

// struct to hold the results of the fitness function
struct equationFitness {
	int numSatisfied;
	vector<bool> satisfied; // vector of all clauses, indicating which are satisfied or not
	vector<int> notSatisfied; // vector of just the unsatisfied clauses, to be used in WalkSAT algorithm
};

// random number generator code (is faster and better than built in rand() function)
// used this instead of rand() because random numbers are needing to be generated quite often, 
// so having an efficient and effective random number generator is important
// got the code for this random number generator from the following site: 
// https://thompsonsed.co.uk/random-number-generators-for-c-performance-tested
// by Sam Thompson
const uint64_t modulusNew = static_cast<unsigned long>(pow(2, 32));
const uint64_t multiplier = 1664525;
const uint64_t increment = 1013904223;
uint64_t x;
uint64_t random()
{
	x = (multiplier * x + increment) % modulusNew;
	return x;
}

// function to find the fitness of an equation based on a certain a assignment
equationFitness fitness(equation eq, string a) {
	// initialize output variable
	equationFitness output;

	// check which clauses are satisfied & store them in a vector & add up the number
	int numSatisfied = 0;
	for (int i = 0; i < eq.numClauses; i++) {
		bool isSatisfied = false;
		for (int j = 0; j < eq.clauses.at(i).size(); j++) {
			int variable = eq.clauses.at(i).at(j);
			bool isNegative = variable < 0;
			variable = abs(variable);
			if (isNegative && a[variable - 1] == '0') {
				numSatisfied++;
				isSatisfied = true;
				break;
			}
			else if (!isNegative && a[variable - 1] == '1') {
				numSatisfied++;
				isSatisfied = true;
				break;
			}
		}
		output.satisfied.push_back(isSatisfied);
		if (!isSatisfied) {
			output.notSatisfied.push_back(i);
		}
	}
	output.numSatisfied = numSatisfied;
	return output;
}

// function to determine if an equation is satisfiable using local search
// returns the satisfying assignment if yes, returns highest number of satisfied clauses if not
// used the pseudocode given to us in class to write this algorithm
string solveUsingLocalSearch(equation eq) {
	int maxIterations = 1000; // max number of iterations performed before a failure is pronounced
	int numIterations = 0; // variable to keep track of number of iterations

	int highestFitness = 0; // variable to store the highestFitness found from any assignment a

	// get random a assignment
	string a = "";
	for (int i = 0; i < eq.numVars; i++) {
		// get random number between 0 and 1 and add it to a
		a += to_string(random() % 2);
	}

	// get fitness of the current a assignment & assign it to the highest fitness variable
	int currFitness = fitness(eq, a).numSatisfied;
	highestFitness = currFitness;

	// while there is still an unsatisfied clause & the max number of iterations has not been reached
	while (currFitness != eq.numClauses && numIterations < maxIterations) {
		// determine if equation has a neighbor w/ better fitness
		// if not, pick new random a value
		bool hasBetterNeighbor = false;
		int i = 0;
		while ((i < eq.numVars) && !hasBetterNeighbor) {
			if (a[i] == '0') {
				a[i] = '1';
				if (fitness(eq, a).numSatisfied > currFitness) {
					hasBetterNeighbor = true;
				}
				else {
					a[i] = '0';
				}
			}
			else {
				a[i] = '0';
				if (fitness(eq, a).numSatisfied > currFitness) {
					hasBetterNeighbor = true;
				}
				else {
					a[i] = '1';
				}
			}
			i++;
		}
		if (!hasBetterNeighbor) {
			// get new random a assignment
			a = "";
			for (int i = 0; i < eq.numVars; i++) {
				// get random number between 0 and 1 and add it to a
				//a += rand() / RAND_MAX;
				a += random() % 2;
			}
		}
		currFitness = fitness(eq, a).numSatisfied;
		highestFitness = max(highestFitness, currFitness);
		numIterations++;
	}

	// if the loop was broken because the max iterations were reached, the equation was not solved
	if (numIterations == maxIterations) {
		return to_string(highestFitness); // return the highest fitness achieved
	}
	else {
		return a; // if equation was solved, return the satisfying assignment
	}
}
