#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <io.h>
#include <vector>

using namespace std;

// struct to hold the information present in the Boolean formulas
struct equation {
	int numVars;
	int numClauses;
	vector<vector<int>> clauses;
};

// function to read in a .cnf file & return an equation struct with the information in the file
equation readCNFFile(string fileName) {
	// initialize variables
	int numVars = 0;
	int numClauses = 0;
	vector<vector<int>> clauses;

	// create fstream for file
	fstream myFile;

	// open file
	myFile.open(fileName);

	// make sure file was found and opened correctly
	if (!myFile) {
		cout << "File not found!" << endl;
	}
	// read in the file information
	else {
		char curr = myFile.get();

		// deal with all comments first
		while (curr == 'c') {
			while (curr != '\n') {
				curr = myFile.get();
			}
			curr = myFile.get();
		}

		// read in the clause information
		if (curr == 'p') {
			string cnf;
			string numVarsString;
			string numClausesString;

			myFile >> cnf;
			myFile >> numVarsString;
			myFile >> numClausesString;

			// convert strings to int
			numVars = stoi(numVarsString);
			numClauses = stoi(numClausesString);

			for (int i = 0; i < numClauses; i++) {
				string currVar = "";
				vector<int> clause;

				myFile >> currVar;
				while (currVar != "0") {
					clause.push_back(stoi(currVar));
					currVar = "";
					myFile >> currVar;
				}
				clauses.push_back(clause);
			}
		}
	}

	// close file
	myFile.close();

	// assign information to an output equation struct & return it
	equation output;
	output.numVars = numVars;
	output.numClauses = numClauses;
	output.clauses = clauses;

	return output;
}