#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <stack>
#include <sstream>
#include <Windows.h>
#define M_PI           3.14159265358979323846  /* pi */
#define M_PI2          M_PI/2.
#define M_E            2.71828182845904523536
#define endl '\n'

enum TokenType1 {
	_func,
	_var,
	_const,
	_oper,
	_brack,
	_bad
};

std::map <std::string, std::function<double(double)>> functions{
	{"sin", [](double x) {return std::sin(x); }},
	{"sinh", [](double x) {return std::sinh(x); }},
	{"asin", [](double x) {return std::asin(x); }},
	{"cos", [](double x) {return std::cos(x); }},
	{"cosh", [](double x) {return std::cosh(x); }},
	{"acos", [](double x) {return std::acos(x); }},
	{"tan", [](double x) {return std::tan(x); }},
	{"tanh", [](double x) {return std::tanh(x); }},
	{"cot", [](double x) {return 1. / std::tan(x); }},
	{"atan", [](double x) {return std::atan(x); }},
	{"acot", [](double x) {return M_PI2 - std::atan(x); }},

	{"exp", [](double x) {return std::exp(x); }},
	{"sqrt", [](double x) {return std::sqrt(x); }},
	{"sqr", [](double x) {return x * x; }},
	{"round", [](double x) {return std::round(x); }},
	{"int", [](double x) {return floor(x); }},

	{"lg", [](double x) {return std::log10(x); }},
	{"ln", [](double x) {return std::log(x); }},
	{"log2", [](double x) {return std::log2(x); }},
	{"abs", [](double x) {return std::abs(x); }}
};


std::map <char, std::function<double(double, double)>> operators{
	{'+', [](double l, double r) {return l + r; }},
	{'-', [](double l, double r) {return l - r; }},
	{'*', [](double l, double r) {return l * r; }},
	{'/', [](double l, double r) {return l / r; }},
	{'^', [](double l, double r) {return std::pow(l, r); }},
	{'%', [](double l, double r) {return l - r * int64_t(l / r); }},
};


bool isNumber(const std::string& str) {
	if (str.empty()) return false;
	size_t i = 0;
	if (str[0] == '+' or str[0] == '-') {
		i = 1;
	}

	bool hasDigits = false, hasDecimalPoint = false;
	for (; i < str.size(); i++) {
		if (std::isdigit(str[i])) {
			hasDigits = true;
		}
		else if (str[i] == '.' and !hasDecimalPoint) {
			hasDecimalPoint = true;
		}
		else {
			return false;
		}
	}
	return hasDigits;
}


TokenType1 GetToken(const std::string& s) {
	if (s.size() == 1) {
		if (s[0] == 'x') return TokenType1::_var;
		if (s[0] == 'e') return TokenType1::_var;
		if (s[0] == '(' or s[0] == ')') return TokenType1::_brack;
		if (operators.count(s[0])) return TokenType1::_oper;
	}
	if (s == "pi") return TokenType1::_var;
	if (functions.count(s)) return TokenType1::_func;
	if (isNumber(s)) return TokenType1::_const;
	return TokenType1::_bad;
}


struct TreeNode1 {
	TreeNode1(const std::string& val) : value(val), type(GetToken(val)) {}
	TokenType1 type;
	std::string value;
	TreeNode1* left{ nullptr };
	TreeNode1* right{ nullptr };
	double Solve(double x) {
		switch (type)
		{
		case _func:
			return SolveFunc(value, right->Solve(x));
		case _var:
			if (value == "e") {
				return M_E;
			}
			if (value == "pi") {
				return M_PI;
			}
			return x;
		case _const:
			return std::stod(value);
		case _oper:
			return SolveOper(value[0], left->Solve(x), right->Solve(x));
		}
		return 0;
	}
private:
	static double SolveFunc(const std::string& func, double x) {
		return functions[func](x);
	}
	static double SolveOper(char op, double x, double y) {
		return operators[op](x, y);
	}
};


struct Solver {
	bool Load(const std::string& expression) {
		auto spltstr = Split(expression);

		if (!TestExpression(spltstr, &error)) {

			return false;
		}
		tree = buildExpressionTree(spltstr, &error);
		if (!tree) {
			return false;
		}
		return true;
	}
	double Solve(double x = 0) {
		return tree->Solve(x);
	}
	void Print() {
		printTree(tree);
	}
	void SetPrecision(size_t _prec) {
		prec = _prec;
	}
	std::string error;
private:
	size_t prec{ (size_t)1e4 };
	TreeNode1* tree{ nullptr };

	static bool TestExpression(std::vector<std::string>& expr, std::string* error) {
		if (expr.empty()) {
			
			*error = "Empty expression";
			return false;
		}

		size_t l{ 0 };
		for (size_t i = 0; i < expr.size(); i++)
		{
			if (expr[i] == "(") {
				l = i;
				continue;
			}
			if (expr[i] == ")") {
				if (i - l == 1) {
					expr.erase(expr.begin() + l, expr.begin() + i + 1);
					i -= 2;
				}
			}
		}

		//for (int i = expr.size() - 1; i >= 0; i--)
		//{
		//	if (GetToken(expr[i]) == TokenType1::_oper) {
		//		expr.pop_back();
		//	}
		//	else {
		//		break;
		//	}
		//}

		size_t cnt_brack{ 0 };
		bool oper_now{ false };
		std::stack<char> bracketStack;
		bool hasNeedExpr = false;
		for (auto& u : expr) {
			if (GetToken(u) == TokenType1::_oper) {
				if (oper_now) {
					*error = "Too many operators";
					return false;
				}
				oper_now = true;
			}
			else {
				oper_now = false;
			}

			if (GetToken(u) == TokenType1::_bad) {
				*error = "Bad word \"" + u + "\"";
				return false;
			}

			if (u == "(") {
				bracketStack.push('(');
			}
			else if (u == ")") {
				if (bracketStack.empty() or bracketStack.top() != '(') {
					*error = "Unmatched closing parenthesis";
					return false;
				}
				bracketStack.pop();
			}

			if (GetToken(u) == TokenType1::_const or GetToken(u) == TokenType1::_var) {
				hasNeedExpr = true;
			}
		}

		if (!bracketStack.empty()) {
			*error = "Unmatched opening parenthesis";
			return false;
		}

		if (!hasNeedExpr) {
			*error = "The expression must be a constant or a variable";
			return false;
		}


		for (size_t i = 0; i < expr.size(); i++)
		{
			if (GetToken(expr[i]) == TokenType1::_oper) {
				if (i != 0 and (GetToken(expr[i - 1]) == TokenType1::_const
					or GetToken(expr[i - 1]) == TokenType1::_var
					or expr[i - 1] == ")"
					or GetToken(expr[i - 1]) == TokenType1::_func)) {
					if (i + 1 != expr.size() and (GetToken(expr[i + 1]) == TokenType1::_const
						or GetToken(expr[i + 1]) == TokenType1::_var
						or expr[i + 1] == "("
						or GetToken(expr[i + 1]) == TokenType1::_func)) {			// ok
					}
					else { // right
						*error = "Right operand not found";
						return false;
					}
				}
				else { // left
					if (expr[i] == "-" or expr[i] == "+") {
						expr.insert(expr.begin() + i, "0");
						i++;
					}
					else {
						*error = "Left operand not found";
						return false;
					}
				}
			}
		}

		return true;
	}
	static char asciitolower(char in) {
		if (in <= 'Z' and in >= 'A')
			return in - ('Z' - 'z');
		return in;
	}
	static std::vector<std::string> Split(std::string const& _ex) {
		std::string ex = _ex;
		ex += ' ';
		std::vector<std::string> out;
		size_t l{ 0 };
		for (size_t r = 0; r < ex.size(); r++)
		{
			while (ex[l] == ' ') {
				if (l + 1 == ex.size()) return out;
				l++;
				r = l;
			}

			if (r > 0 and ex[r - 1] == ' ' and ex[r] == ' ') {
				l = r;
				continue;
			}
			if (ex[r] == ' ') { // пробел
				std::string sbstr = ex.substr(l, r - l);
				std::transform(sbstr.begin(), sbstr.end(), sbstr.begin(), asciitolower);
				out.push_back(sbstr);
				l = r;
				continue;
			}
			if (operators.count(ex[r])) { // оператор
				std::string sbstr = ex.substr(l, r - l), op; op += ex[r];
				std::transform(sbstr.begin(), sbstr.end(), sbstr.begin(), asciitolower);
				if (!sbstr.empty()) out.push_back(sbstr);
				out.push_back(op);
				l = r; 
				l++;
				continue;
			}
			if (ex[r] == '(' or ex[r] == ')') { // скобки
				std::string sbstr = ex.substr(l, r - l), op; op += ex[r];
				std::transform(sbstr.begin(), sbstr.end(), sbstr.begin(), asciitolower);
				if (!sbstr.empty()) out.push_back(sbstr);
				out.push_back(op);
				l = r; l++;
				continue;
			}
		}
		return out;
	}


	static void printTree(TreeNode1* root, int indent = 0) {
		if (root) {
			printTree(root->right, indent + 4);
			for (int i = 0; i < indent; i++) std::cout << " ";
			std::cout << root->value << "\n";
			printTree(root->left, indent + 4);
		}
	}


	static int getPrecedence(const std::string& op) {
		if (op == "+" or op == "-") {
			return 1;
		}
		if (op == "*" or op == "/" or op == "%") {
			return 2;
		}
		if (op == "^") {
			return 3;
		}
		if (functions.count(op)) {
			return 4;
		}
		return 0;
	}


	static TreeNode1* buildExpressionTree(const std::vector<std::string>& expression, std::string* error) {
		std::stack<TreeNode1*> nodeStack;
		std::stack<std::string> opStack;

		for (size_t i = 0; i < expression.size(); i++)
		{
			if (GetToken(expression[i]) == TokenType1::_const or GetToken(expression[i]) == TokenType1::_var) {
				nodeStack.push(new TreeNode1(expression[i]));
			}
			else if (i > 0 and (GetToken(expression[i - 1]) == TokenType1::_const or GetToken(expression[i - 1]) == TokenType1::_var) and expression[i] == "(") {
				opStack.push("*");
				opStack.push(expression[i]);
			}
			else if (expression[i] == "(") {
				opStack.push(expression[i]);
			}
			else if (expression[i] == ")") {
				while (!opStack.empty() and opStack.top() != "(") {
					std::string op = opStack.top();
					opStack.pop();

					if (nodeStack.empty()) {
						*error = "The required number of arguments for " + op + " function was not found";
						return nullptr;
					}

					TreeNode1* right = nodeStack.top();
					nodeStack.pop();
					TreeNode1* left{ nullptr };
					if (!functions.count(op) and !nodeStack.empty()) {
						left = nodeStack.top();
						nodeStack.pop();
					}
					TreeNode1* opNode = new TreeNode1(op);
					if (left != nullptr) {
						opNode->left = left;
						opNode->right = right;
					}
					else {
						opNode->right = right;
					}
					nodeStack.push(opNode);

				}
				opStack.pop();  // Pop the "("
			}
			else if (GetToken(expression[i]) == TokenType1::_oper or GetToken(expression[i]) == TokenType1::_func) {
				while (!opStack.empty() and getPrecedence(opStack.top()) >= getPrecedence(expression[i])) {

					std::string op = opStack.top();
					opStack.pop();

					if (nodeStack.empty()) {
						*error = "The required number of arguments for " + op + " function was not found";
						return nullptr;
					}

					TreeNode1* right = nodeStack.top();
					nodeStack.pop();

					TreeNode1* left{ nullptr };
					if (!functions.count(op) and !nodeStack.empty()) {
						left = nodeStack.top();
						nodeStack.pop();
					}


					TreeNode1* opNode = new TreeNode1(op);
					if (left != nullptr) {
						opNode->left = left;
						opNode->right = right;
					}
					else {
						opNode->right = right;
					}
					nodeStack.push(opNode);
				}
				opStack.push(expression[i]);
			}
		}

		while (!opStack.empty()) {

			std::string op = opStack.top();
			opStack.pop();

			if (nodeStack.empty()) {
				*error = "The required number of arguments for " + op + " function was not found";
				return nullptr;
			}

			TreeNode1* right = nodeStack.top();
			nodeStack.pop();

			TreeNode1* left{ nullptr };
			if (!functions.count(op) and !nodeStack.empty()) {
				left = nodeStack.top();
				nodeStack.pop();
			}

			TreeNode1* opNode = new TreeNode1(op);
			opNode->left = left;
			opNode->right = right;

			nodeStack.push(opNode);
		}

		return nodeStack.top();
	}
};

//static std::string getline() {
//	std::string s;
//	char c = std::cin.get();
//	std::getline(std::cin, s);
//	if (c != '\n' and c != EOF) {
//		s = c + s;
//	}
//	return s;
//}

int main() {
	int n;
	std::cin >> n;
	std::cin.ignore();
	for (int i = 0; i < n; i++) {
		std::string str;
		std::getline(std::cin,str);
		Solver sol;
		sol.Load(str);
		std::cout << "Expression tree" << endl;
		sol.Print();
		std::cout << endl;
		if (!sol.error.empty()) {
			MessageBoxA(NULL, ("Error\n" + sol.error).c_str(), NULL, NULL);
			std::cout << sol.error << endl;
			continue;
		}
		double ans = sol.Solve();
		std::cout << std::format("Result: {:.2f}\n",ans) << endl;
	}
}
