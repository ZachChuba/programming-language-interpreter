#ifndef VAL_H
#define VAL_H

#include <string>
#include <cmath>
using namespace std;
enum ValType {
	ISINT, ISSTR, ISERR
};

extern void swap(string &str, const int &location1, const int &location2);
extern void runtime_err(const int& linenum, const string& errorMessage);

class Val {
	int i;
	string s;
	ValType vt;
public:
	Val() :
			i(0), vt(ISERR) {
	}
	Val(int i) :
			i(i), vt(ISINT) {
	}
	Val(string s) :
			i(0), s(s), vt(ISSTR) {
	}
	Val(ValType vt, string errmsg) :
			i(0), s(errmsg), vt(ISERR) {
	}

	ValType getVt() const {
		return vt;
	}

	bool isErr() const {
		return vt == ISERR;
	}
	bool isInt() const {
		return vt == ISINT;
	}
	bool isStr() const {
		return vt == ISSTR;
	}

	int ValInt() const {
		if (isInt())
			return i;
		throw "This Val is not an Int";
	}
	string ValString() const {
		if (isStr())
			return s;
		throw "This Val is not a Str";
	}

	friend ostream& operator<<(ostream &out, const Val &v) {
		// IMPLEMENT
		if (v.isStr())
			out << v.ValString();
		if (v.isInt())
			out << v.ValInt();
		return out;
	}

	string GetErrMsg() const {
		if (isErr())
			return s;
		throw "This Val is not an Error";
	}

	Val operator+(const Val &op) const {
		if (isInt() && op.isInt())
			return ValInt() + op.ValInt();
		if (isStr() && op.isStr())
			return ValString() + op.ValString();
		return Val(ISERR, "Type mismatch on operands of +");
	}

	Val operator-(const Val &op) const {
		// IMPLEMENT
		if (isInt() && op.isInt())
			return ValInt() - op.ValInt();
		return Val(ISERR, "Type mismatch on operands of -");
	}

	Val operator*(const Val &op) const {
		// IMPLEMENT
		if (isInt() && op.isInt())
			return ValInt() * op.ValInt();
		if (isStr() && op.isInt()) {
			if (op.ValInt() <= 0) {
				if (op.ValInt() == 0) {
					return Val("");
				}
				return Val(ISERR, "Cannot multiply string by negative int");
			}
			string strCopy = ValString();
			for (int i = 0; i < op.ValInt() - 1; i++) {
				strCopy += ValString();
			}
			return strCopy;
		}
		if (isInt() && op.isStr()) {
			if (ValInt() <= 0) {
				if (ValInt() == 0) {
					return Val("");
				}
				return Val(ISERR, "Cannot multiply string by negative int");
			}
			string strCopy = op.ValString();
			for (int i = 0; i < ValInt() - 1; i++) {
				strCopy += op.ValString();
			}
			return strCopy;
		}
		return Val(ISERR,
				"Type mismatch on operands of *");
	}

	Val operator/(const Val &op) const {
		// IMPLEMENT
		if (op.isInt() && op.ValInt() == 0)
			return Val(ISERR, "Divide by zero error");
		if (isInt() && op.isInt())
			return ValInt() / op.ValInt();
		return Val(ISERR,
				"Type mismatch on operands of /");
	}
	Val operator!() const {
		// IMPLEMENT
		if (isInt()) {
			int vCopy = this->ValInt();
			int reversed = 0;
			int sigFig = log10(abs(vCopy));
			while (abs(vCopy) > 0) {
				reversed += vCopy % 10 * pow(10, sigFig--);
				vCopy /= 10;
			}
			return reversed;
		}
		if (isStr()) {
			string strCopy = string(ValString());
			int length = strCopy.length();
			for (int i = 0; i < length / 2; i++) {
				swap(strCopy, i, length - i - 1);
			}
			return strCopy;
		}
		return Val(ISERR, "Invalid type, only string an integers allowed");
	}

};

#endif
