#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "parseFiles.h"
#include "localSearch.h"

using namespace std;

// function to attempt to solve a cnf equation using WalkSAT
// found pseudocode for this function at
// https://www.sciencedirect.com/topics/computer-science/unsatisfied-clause
// by Holger H. Hoos and Edward Tsang
string solveUsingWalkSAT(equation eq) {
	int maxTries = 1000; // max number of tries performed before a failure is pronounced
	int maxFlips = 12; // max number of variable flips
	float p = 0.5; // probability

	srand((unsigned)time(NULL)); // seed the random number generator

	int highestFitness = 0; // variable to store the highest fitness reached

	for (int numTries = 0; numTries < maxTries; numTries++) {
		// start with a random assignment a
		string a = "";
		for (int i = 0; i < eq.numVars; i++) {
			// get random number between 0 and 1 and add it to a
			//a += to_string(rand() / RAND_MAX);
			a += to_string(random() % 2);
		}

		for (int numFlips = 0; numFlips < maxFlips; numFlips++) {
			// compare best assignment with a and retain the best
			equationFitness currFitness = fitness(eq, a);
			highestFitness = max(highestFitness, currFitness.numSatisfied);

			// if a is the solution, return a
			if (currFitness.numSatisfied == eq.numClauses) {
				return a;
			}
			else {
				// randomly choose an unsatisfied clause C
				int C = random() % currFitness.notSatisfied.size();

				// get random float between 0 and 1
				float r = rand() / (float)RAND_MAX;

				// with probability p, choose a random literal in C to flip
				if (r <= p) {
					int randomNum = random() % eq.clauses.at(C).size();
					int randomLiteral = eq.clauses.at(C).at(randomNum);

					randomLiteral = abs(randomLiteral); // account for negative literals

					// flip this literal
					if (a[randomLiteral - 1] == '0') {
						a[randomLiteral - 1] = '1';
					}
					else {
						a[randomLiteral - 1] = '0';
					}
				}
				// with probability 1-p, choose a literal in C to flip that minimizes the # of clauses that go from T to F
				else {
					int minFlips = eq.numClauses + 1;
					int minFlipsLocation;
					char minFlipsValue;
					for (int i = 0; i < eq.clauses.at(C).size(); i++) {
						int literal = abs(eq.clauses.at(C).at(i));
						// flip the current literal
						if (a[literal] == '0') {
							a[literal] = '1';
							if ((currFitness.numSatisfied - fitness(eq, a).numSatisfied) < minFlips) {
								minFlips = currFitness.numSatisfied - fitness(eq, a).numSatisfied;
								minFlipsLocation = literal;
								minFlipsValue = '1';
							}
							a[literal] = '0'; // reset
						}
						else {
							a[literal] = '0';
							if ((currFitness.numSatisfied - fitness(eq, a).numSatisfied) < minFlips) {
								minFlips = currFitness.numSatisfied - fitness(eq, a).numSatisfied;
								minFlipsLocation = literal;
								minFlipsValue = '0';
							}
							a[literal] = '1'; // reset
						}
					}
					a[minFlipsLocation] = minFlipsValue;
				}
			}
		}
	}
	return to_string(highestFitness);
}