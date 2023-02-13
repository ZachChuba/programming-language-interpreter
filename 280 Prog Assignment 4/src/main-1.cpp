/*
 * main.cpp
 */

#include "lex.h"
#include "parse.h"
#include "val.h"
#include <fstream>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

void swap(string &str, const int &location1, const int &location2) {
	char temp = str[location1];
	str[location1] = str[location2];
	str[location2] = temp;
}
void runtime_err(const int& linenum, const string& errorMessage) {
	if (errorMessage == "") {
		throw std::string("");
	}
	throw std::string("RUNTIME ERROR at " + std::to_string(linenum) + ": " + errorMessage + '\n');

}

int main(int argc, char *argv[]) {
	ifstream file;
	istream *in;
	int linenum = 0;

	if (argc == 1) {
		in = &cin;
	}

	else if (argc == 2) {
		file.open(argv[1]);
		if (file.is_open() == false) {
			cout << "COULD NOT OPEN " << argv[1] << endl;
			return 1;
		}
		in = &file;
	}

	else {
		cout << "TOO MANY FILENAMES" << endl;
		return 1;
	}

	ParseTree *prog = Prog(*in, linenum);

	if (prog == 0)
		return 0;

	map<string, Val> symbols;

	map<string, bool> declaredId;
	int a = prog->CheckLetBeforeUse(declaredId);
    if (a > 0)
        return -1;
	try {
		prog->Eval(symbols);
	} catch (std::string& e) {
		cerr << e;
	}
	/*
	cout << "BANG COUNT: " << prog->BangCount() << endl;
	cout << "MAX DEPTH: " << prog->MaxDepth() << endl;
	*/
	return 0;
}
