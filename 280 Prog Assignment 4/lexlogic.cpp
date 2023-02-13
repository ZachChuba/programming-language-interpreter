#include "lex.h"
#include <cctype>
#include <ostream>

namespace expressions {
// create a way to iterate through the allowed expressions
// formerly used regex, but in this new implementation I still use the first 6 elements of this
const string strExpressionMap[] = { "print", "let", "if", "loop", "begin",
		"end", "[A-Za-z][A-Za-z0-9]*", "[0-9]+", "\"[.]*\"", "\\+", "-", "\\*",
		"/", "!", "\\(", "\\)", ";" }; // len = 17
}
namespace tokenNames{
    const string lexNames[] = { "PRINT", "LET", "IF", "LOOP", "BEGIN", "END", "ID",
		"INT", "STR", "PLUS", "MINUS", "STAR", "SLASH", "BANG", "LPAREN",
		"RPAREN", "SC", "ERR", "DONE" };
}

ostream& operator<<(ostream& out, const Lex& token) {
    if (token.GetToken() >= ID && token.GetToken() <= STR) {
        out << tokenNames::lexNames[token.GetToken()] << "(" << token.GetLexeme() << ")" << std::endl;
    }
    else if (token.GetToken() != ERR && token.GetToken() != DONE) {
        out << tokenNames::lexNames[token.GetToken()] << std::endl;
    } else if (token.GetToken() == ERR) {
        out << "Error on line " << token.GetLinenum() << " (" << token.GetLexeme()
					<< ")\n";
    }
    return out;
}

Lex error(string &currentExpression, int &linenum) {
	linenum++;
	Lex error(ERR, currentExpression, linenum);
	return error;
}

bool single_char_match(const char &c, const int &expressionNumber) {
	// an efficient way of seeing if the next char continues to fit an expression
	// if checking for identifier or int, simply see if the next character is valid,
	// rather than checking the entire expression again + the next char
	if (expressionNumber == ID) {
		if (isdigit(c) || isalpha(c)) {
			return true;
		}
	} else if (expressionNumber == INT) {
		if (isdigit(c)) {
			return true;
		}
	}
	return false;
}

Token first_char_match(const char &c) {
	// match the first char of a new token with an expression
	if (isalpha(c))
		return ID;
	else if (isdigit(c))
		return INT;
	else if (c == '"')
		return STR;
	else if (c == '+')
		return PLUS;
	else if (c == '-')
		return MINUS;
	else if (c == '*')
		return STAR;
	else if (c == '!')
		return BANG;
	else if (c == '/')
		return SLASH;
	else if (c == '(')
		return LPAREN;
	else if (c == ')')
		return RPAREN;
	else if (c == ';')
		return SC;
	return ERR;
}

Lex getNextToken(istream &in, int &linenum) {
	string currentLexeme = "";
	Token expectedType = PRINT;
	char currentChar;
	while (true) {
		currentChar = in.get();
		if (in.eof()) { // Done
			Lex result(DONE, "", linenum);
			return result;
		}
		// ignore spaces
		if (isspace(currentChar) && expectedType != STR) {
			if (currentChar == '\n') {
				linenum++;
			}
			if (expectedType != PRINT) {
				return error(currentLexeme, linenum);
			}
			continue;
		}

		// if on the first iteration, do first iteration protocol
		if (expectedType == PRINT) {
			expectedType = first_char_match(currentChar);
			if (expectedType != STR) {
				currentLexeme += currentChar;
			}
			if (expectedType == ERR) {
				Lex theError = error(currentLexeme, linenum);
				return theError;
			}
			if (expectedType >= PLUS && expectedType <= SC) { // 1 character finished
				if (expectedType == SLASH) { // handle potential comments
					if (in.peek() == '/') {
						linenum++;
						// move the in buffer to the end of the line, then discard it and restart
						getline(in, currentLexeme);
						currentLexeme = "";
						expectedType = PRINT;
						continue;
					}
				}
				Lex result(expectedType, currentLexeme, linenum);
				return result;
			} else if (expectedType != STR) {
				if (!single_char_match(in.peek(), expectedType)) {
					Lex result(expectedType, currentLexeme, linenum);
					return result;
				}
			}
		} else if (expectedType != PRINT) {// if not first iteration, match the rest of the pattern
			if (expectedType != STR) {
				currentLexeme += currentChar;
				// check if the next char doesn't fit the pattern, if it doesn't then the token's finished
				if (!single_char_match(in.peek(), expectedType)) {
					// ensure it's not a keyword
					for (int i = PRINT; i <= END; i++) {
						if (currentLexeme == expressions::strExpressionMap[i]) {
							Lex result((Token) i, currentLexeme, linenum);
							return result;
						}
					}
					Lex result(expectedType, currentLexeme, linenum);
					return result;
				}
			} else { // handle string cases
				if (currentChar == '\\') {
					// ignore \ in escaped characters
					char nextVal = in.get();
					if (in.eof() || nextVal == '\n') { // ensure the program doesn't end with \ mid string
						currentLexeme = '"' + currentLexeme + '\\';
						Lex theError = error(currentLexeme, linenum);
						return theError;
					} else if (nextVal == 'n') {
						// convert \n to newline
						currentLexeme += '\n';
						continue;
					}
					currentLexeme += nextVal;
					continue;
				} else if (currentChar == '"') {
                    // string ended
					Lex strResult(STR, currentLexeme, linenum);
					return strResult;
				} else if (currentChar == '\n') {
                    // string didn't end on the same line it started
					currentLexeme = '"' + currentLexeme + '\n';
					Lex theError = error(currentLexeme, linenum);
					return theError;
				} else {
                    // nothing special happens, just add an element in the string
					currentLexeme += currentChar;
					continue;
				}
			}
		}
	}
	return Lex{};
}
