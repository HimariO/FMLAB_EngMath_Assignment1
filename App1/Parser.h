#pragma once
#include <regex>
#include <map>
#include "SyntaxNode.h"

typedef Variable* (*FnPtr)(std::vector<Variable*>);

class Parser {
public:
	Parser();
	Parser(std::map<std::string, Variable*> outter_S);

	bool debug = false;
	std::map<std::string, Variable*> scope;
	static std::map<std::string, Variable*> global_scope;

	std::map<std::string, FnPtr> fun_talbe;
	static std::map<std::string, FnPtr> standar_fun_talbe;
	static bool std_table_init;

	std::map<std::string, SyntaxNode*> node_storage;

	Variable* parseRaw(std::string input, bool crossSection=false);

	Variable* udpate_variable(Variable *var);
	Variable* get_variable(std::string name);

	FnPtr get_functionRef(std::string fun_name);

	std::vector<Variable*> ArgParse(std::string, bool isFunArg = false);
	std::string matchBracket(std::string* input_ref, int start_pos, std::string node_key, bool isVecArg = false);

};




