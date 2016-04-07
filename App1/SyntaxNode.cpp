#pragma once
#include "pch.h"
#include "SyntaxNode.h"

#include <regex>
#include <iostream>
#include <map>
#include <cstdlib>



SyntaxNode::SyntaxNode() {}
SyntaxNode::SyntaxNode(vFunPtr * ptr)
{
	this->type = tSyntaxNode::function;
	this->var = ptr;
};


SyntaxNode::SyntaxNode(std::string str_content, Variable *var) {
	this->str_content = str_content;

	std::string reg_val = "\\-?\\d+\\.*\\d*";
	std::string reg_var = "\\w+";
	std::string reg_vector = "\\{.*\\}";
	std::string reg_index = "\\[([^\\[\\]]+)\\]";

	if (std::regex_match(str_content.begin(), str_content.end(), std::regex(reg_val))) {
		this->type = tSyntaxNode::num;
		this->var = new vNumber(stod(str_content));
	}
	else if (std::regex_match(str_content.begin(), str_content.end(), std::regex(reg_var)) || var != NULL) {
		this->type = tSyntaxNode::var;
		//if var is already defined, arg_var will be assign. otherwise it will be NULL.
		this->var = var;
	}
	else if (std::regex_match(str_content.begin(), str_content.end(), std::regex(reg_vector))) {
		this->type = tSyntaxNode::var;
	}
	else if (std::regex_match(str_content.begin(), str_content.end(), std::regex(reg_index))) {
		this->type = tSyntaxNode::index;
	}
	else {
		switch (str_content[0]) {
		case '*':
		case '/':
		case '-':
		case '+':
			this->type = tSyntaxNode::opertor;
			break;
		case '=':
			this->type = tSyntaxNode::assign;
			break;
		default:
			this->type = tSyntaxNode::error;
		}
	}
};


Variable* SyntaxNode::execute_op() {
	if (this->type == tSyntaxNode::var || this->type == tSyntaxNode::num)
		return this->var;

	if (this->type == tSyntaxNode::function) {
		vFunPtr *temp = dynamic_cast<vFunPtr*>(this->var);
		std::string fun_name = temp->fun_name;
		FnPtr exFn = temp->function_ref;

		return (*exFn)(temp->args);
	}

	Variable *left_v = left_node->execute_op();
	Variable *right_v = right_node->execute_op();


	if (left_v->gettype() == vType::number) {
		vNumber *temp = dynamic_cast<vNumber*>(left_v);

		switch (str_content[0]) {
		case '*':
			if (right_v->gettype() == vType::vector) {
				vVector *temp2 = dynamic_cast<vVector*>(right_v);
				return *temp2 * (*temp);
			}
			return *temp * (*right_v);
		case '/':
			if (right_v->gettype() == vType::vector) {
				vVector *temp2 = dynamic_cast<vVector*>(right_v);
				return *temp2 / (*temp);
			}
			return *temp / *right_v;
		case '-':
			return *temp - *right_v;
		case '+':
			return *temp + *right_v;
		case '=':
			temp->data = dynamic_cast<vNumber*>(right_v)->data;
			return temp;
		}
	}
	else if (left_v->gettype() == vType::string) {
		vString *temp = dynamic_cast<vString*>(left_v);

		switch (str_content[0]) {
		case '*':
			//return *temp * (*right_v);
		case '/':
			//return *temp / *right_v;
		case '-':
			//return *temp - *right_v;
		case '+':
			return *temp + *right_v;
		case '=':
			right_v->name = left_v->name;
			return right_v;
			break;
		}
	}
	else if (left_v->gettype() == vType::vector) {
		auto *temp = dynamic_cast<vVector*>(left_v);
		
		switch (str_content[0]) {
		case '*':
			return *temp * (*right_v);
		case '/':
			return *temp / *right_v;
	/*	case '-':
			return *temp - *right_v;*/
		case '+':
			return *temp + *right_v;
		case '=':
			temp->data = dynamic_cast<vVector*>(right_v)->data;
			return temp;
			break;
		}
	}
	else {
		switch (str_content[0]) {
		case '=':
			right_v->name = left_v->name;
			//if left_node is undefined variable. then apply new name to result var and return.
			return right_v;
		}
	}

}

