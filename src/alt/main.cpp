#include <iostream>
#include <istream>
#include <fstream>
#include <map>
#include <set>
#include "parse.h"
using namespace std;

void DFT(ParseTree *tree, set<string> &usedIds) {
	// Base case: root reached
	if (tree == 0) return;
	// Post order, traverse down first
	DFT(tree->GetLeft(), usedIds);
	DFT(tree->GetRight(), usedIds);
	// Check for let statements, add such a var to a set of all defined vars
	if (tree->IsLet()) {
		usedIds.insert(tree->GetId());
	} else if (tree->IsIdent()) { // If a var isn't defined, yet used, print it
		if (usedIds.find(tree->GetId()) == usedIds.end()) {
			cout << "UNDECLARED VARIABLE " << tree->GetId() << endl;
		}
	}
}

int main(int argc, char** argv) {
	istream *input = &cin;
	ifstream i_file;
	int linenum = 0;
	if (argc == 2) {
		i_file.open(argv[1]);
		if (!i_file.is_open()) {
			cout << "COULD NOT OPEN " << argv[1] << '\n';
			return -1;
		}
		input = &i_file;
	} else if (argc > 2) {
		cout << "TOO MANY FILENAMES\n";
		return -1;
	}

	ParseTree* result = Prog(*input, linenum);
	// output undeclared variables
	set<string> declaredID;
	DFT(result, declaredID);
	// output bang count
	cout << "BANG COUNT: " << result->BangCount() << endl;
	// output depth
	cout << "MAX DEPTH: " << result->MaxDepth() << endl;

	/* DEBUG
	set<string> usedIds;
	DFT(result, usedIds);
	for (auto i = usedIds.begin(); i != usedIds.end(); i++) {
		cout << *i << " ";
	}
	*/
	return 0;
}





