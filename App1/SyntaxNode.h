#pragma once
#include "Variable.h"

#include <regex>
#include <map>


enum class tSyntaxNode { num, str, var, opertor, statment, error, keyword, assign, logic, function, index};

class SyntaxNode {
public:
	SyntaxNode();
	SyntaxNode(vFunPtr *ptr);
	SyntaxNode(std::string str_content, Variable *var = NULL);

	std::string str_content;


	tSyntaxNode type;
	SyntaxNode * left_node, *right_node;
	Variable *var;

	Variable* execute_op();
};
