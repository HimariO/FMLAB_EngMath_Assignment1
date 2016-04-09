#pragma once
#include "pch.h"
#include "Parser.h"
#include "MatrixComput.h"
#include "VectorComput.h"

#include <regex>
#include <iostream>
#include <map>
#include <cstdlib>


using std::cout;
using std::cin;

std::map<std::string, Variable*> Parser::global_scope;
std::map<std::string, FnPtr> Parser::standar_fun_talbe{
	{ "vectest", vectest },
	{"mtxtest", mtxtest },
	{ "dot", dot },
	{ "rref", RREF },
	{"trans", trans},
	{"slice", slice},
	{"det", det},
	{"cofactor", cofactor },
	{"adj", adj},
	{"inverse", inverse },
	{"up_tri", up_tri },
	{ "low_tri", low_tri },
	{"solveLinearSys", solveLinearSys },
	{"LeastSquare", LeastSquare },
	{"eig", eig},
	{"QR_Q", QR_Q },
	{"rank", rank }
};

bool Parser::std_table_init = false;

Parser::Parser()
{
}

Parser::Parser(std::map<std::string, Variable*> outter_S)
{
}

Variable* Parser::parseRaw(std::string input, bool crossSection)
{
	
	std::map<std::string, SyntaxNode*> node_dic;
	bool  find_something = false;

	if (crossSection)
		node_dic = this->node_storage;

	std::string reg_node_or_valvar = "(<#\\d+\\/>|\\w+|\\-?\\d+\\.?\\d*)";
	std::string reg_node = "<#\\d\\/>";
	//std::string reg_vetor = "\\{.*\\}";
	//std::string reg_vetor = "^\\{.*\\}|(?!(\\(\\s*))(\\{.*\\})";
	std::string reg_vetor = "^\\s*\\{|(?![\\{\\(\\,]\\s*)\\{\\d+|[\\+\\-\\*\\/\\=]\\s*\\{";
	std::string reg_num = "\\-?\\d+\\.?\\d*";
	std::string reg_var = "\\w+";
	std::string reg_fun = "(<#\\d\\/>|\\w+)\\s*\\(";
	std::string reg_index = "(<#\\d\\/>|\\w+)(\\[[^\\[\\]]+\\])";

	std::regex reg_inher[] = {
		std::regex("^\\(\\w+|\\W\\(\\w+"),
		std::regex("(" + reg_vetor + ")"),
		std::regex(reg_index),
		std::regex(reg_fun),
		std::regex(reg_node_or_valvar + "\\s*([\\*\\/])\\s*" + reg_node_or_valvar),
		std::regex(reg_node_or_valvar + "\\s*([\\+\\-])\\s*" + reg_node_or_valvar),
		std::regex(reg_node_or_valvar + "\\s*([=])\\s*" + reg_node_or_valvar),
		std::regex("(\\w+)(?=(\\s))|(" + reg_num + ")(?!(\\/))|^(\\w+)")
	};


	for (int j = 0; j < 8; j++) {
		if(this->debug)
			std::cout << "----------------" << std::to_string(j) << "---------------/" << input << "/---------------" << std::endl;

		std::smatch match;
		std::regex_search(input, match, reg_inher[j]);

		while (match.length() > 0) {
			find_something = true;

			if (this->debug)
				for (int i = 0; i < match.length(); i++) {
					std::cout << std::to_string(i) << ": " << match[i] << std::endl;
				}

			std::string node_key = "<#" + std::to_string(node_dic.size() + 1) + "/>";
			// <#1/><#2/>...<#?/>

			if (j == 0) { // braket wrapped.
				std::string in_BK= this->matchBracket(&input, match.position(), node_key);
				Variable *bk_result = this->parseRaw(in_BK);

				//[special method to avoid <#n> tag not found in another this->parseRaw call.]
				input = std::regex_replace(input, std::regex(node_key), bk_result->toComputString(), std::regex_constants::format_first_only);
				
				//node_dic.insert(std::pair<std::string, SyntaxNode*>(node_key, new SyntaxNode("bk_result", bk_result)));

			}
			else if (j == 2) { //isarray indexing.
				
				std::string m_1 = match[1], m_2 = match[2];
				SyntaxNode *var_node = new SyntaxNode(m_1);
				SyntaxNode *arg_node = new SyntaxNode(m_2);
				arg_node->right_node = var_node;

				node_dic.insert(std::pair<std::string, SyntaxNode*>(node_key, arg_node));

			}
			else if (j == 3) {	// funtion call 
				std::string m_1 = match[1];
				std::string funarg_str = this->matchBracket(&input, match.position()+match[0].length()-1, node_key);
				//[use matchBracket Only replace function args with node_key but wont replace function name. ]
				int AA = match.position();
				int BB = match[0].length();
				int CC = match[1].length();
				//input.replace(match.position(), match.position() + m_1.length(), "");
				input = std::regex_replace(input, std::regex(m_1), "", std::regex_constants::format_first_only);

				vFunPtr *fun_var = new vFunPtr(m_1, this->ArgParse("("+funarg_str+")", true));
				fun_var->function_ref = this->get_functionRef(m_1);

				SyntaxNode *fun_node = new SyntaxNode(fun_var);

				node_dic.insert(std::pair<std::string, SyntaxNode*>(node_key, fun_node));

			}
			else if (j ==  1) { //is vector ... or matrix
				std::string m_0 = match[0];
				std::string args = this->matchBracket(&input, match.position() + m_0.length() - 1, node_key, true);

				SyntaxNode *vector_node = new SyntaxNode("{" + args + "}");
				vector_node->var = new vVector(this->ArgParse("{"+args+"}"));

				node_dic.insert(std::pair<std::string, SyntaxNode*>(node_key, vector_node));
			}
			else if (j>3 && j <= 6) {	//is operator
				SyntaxNode *op_node = new SyntaxNode(match[2]);
				std::string m_1 = match[1], m_3 = match[3];

				if (std::regex_match(m_1.begin(), m_1.end(), std::regex(reg_num))) {
					op_node->left_node = new SyntaxNode(m_1);
				}
				else if (std::regex_match(m_1.begin(), m_1.end(), std::regex(reg_node))) {
					op_node->left_node = node_dic[m_1];
				}
				else if (std::regex_match(m_1.begin(), m_1.end(), std::regex(reg_var))) {
					// create new syntax node and assign var ref to node.
					Variable *finded = get_variable(m_1);
					op_node->left_node = new SyntaxNode(m_1, finded);
				}
				else {
					std::cout << m_1 << "[parse error]" << std::endl;
					//raise not parsable error
				}


				if (std::regex_match(m_3.begin(), m_3.end(), std::regex(reg_num))) {
					op_node->right_node = new SyntaxNode(m_3);
				}
				else if (std::regex_match(m_3.begin(), m_3.end(), std::regex(reg_node))) {
					op_node->right_node = node_dic[m_3];
				}
				else if (std::regex_match(m_3.begin(), m_3.end(), std::regex(reg_var))) {
					// create new syntax node and assign var ref to node.
					Variable *finded = get_variable(m_3);
					op_node->right_node = new SyntaxNode(m_3, finded);
				}
				else {
					//raise not parsable error
				}

				node_dic.insert(std::pair<std::string, SyntaxNode*>(node_key, op_node));

			}
			else if (j == 7) {  //is single variable or num
				std::string m_1 = match[0];
				SyntaxNode *var_node = new SyntaxNode(m_1);

				if (!std::regex_match(m_1.begin(), m_1.end(), std::regex(reg_num)))
					var_node->var = this->get_variable(m_1);

				node_dic.insert(std::pair<std::string, SyntaxNode*>(node_key, var_node));

			}

			if(j!=0 && j!=3 && j != 1)  //regex can't deal with full bracket matching task. replacing mathch part in Parser::matchBracket.
				input = std::regex_replace(input, reg_inher[j], node_key, std::regex_constants::format_first_only);

			std::regex_search(input, match, reg_inher[j], std::regex_constants::format_first_only);
		}//while end
	}//for end

	if(!find_something&& node_dic.find("<#" + std::to_string(node_dic.size()) + "/>") == node_dic.end())
		throw std::runtime_error(vException::WTF);

	SyntaxNode *last = node_dic["<#" + std::to_string(node_dic.size()) + "/>"];
	Variable *exe_result = last->execute_op();

	if (last->str_content == "=") {
		Variable *up_result = this->udpate_variable(exe_result);
	}

	return exe_result;
}


Variable* Parser::udpate_variable(Variable *var) {
	if (Parser::global_scope.find(var->name) != Parser::global_scope.end()) {
		Parser::global_scope.insert(std::pair<std::string, Variable*>(var->name, var));
		return Parser::global_scope[var->name];
	}
	else {
		this->scope.insert(std::pair<std::string, Variable*>(var->name, var));
		return this->scope[var->name];
	}
}

Variable * Parser::get_variable(std::string name)
{//check if variable is already defined.
	if (Parser::global_scope.find(name) != Parser::global_scope.end())
		return Parser::global_scope[name];
	else if (this->scope.find(name) != this->scope.end())
		return this->scope[name];
	else {
		Variable *temp = new Variable();
		temp->name = name;
		return temp;
	}
}


FnPtr Parser::get_functionRef(std::string fun_name)
{
	if (Parser::standar_fun_talbe.find(fun_name) != Parser::standar_fun_talbe.end())
		return Parser::standar_fun_talbe[fun_name];
	else if (this->fun_talbe.find(fun_name) != this->fun_talbe.end())
		return this->fun_talbe[fun_name];

	throw std::runtime_error(vException::UndefinedFunctionException + fun_name);
}


std::vector<Variable*> Parser::ArgParse(std::string data, bool isFunArg)
{
	std::regex reg_num( "\\-?\\d+\\.?\\d*");
	std::regex reg_lf, reg_l, reg_r;
	reg_lf = std::regex("[\\(\\{](.*)[\\)\\}]");
	reg_l = std::regex(".*[\\(\\{].*");
	reg_r = std::regex(".*[\\)\\}].*");

	if(this->debug)
		std::cout << "[ArgParse Start]" << data<< std::endl;

	std::smatch match;
	std::regex_search(data, match, reg_lf);
	std::string content = match[1];

	size_t pos = 0; //remove redunden space
	while ((pos = content.find(" ", pos)) != std::string::npos) {
		content.replace(pos, 1, "");
	}

	std::vector<Variable*> var_vec;

	pos = 0;
	std::string inner_bk = "";
	bool inother_bk = false;
	int bk_counter = 0;

	while ((pos = content.find(",", pos)) != std::string::npos) {
		std::string between_bk = content.substr(0, pos);

		if ( std::regex_match(between_bk.begin(), between_bk.end(), reg_l) ) {
			inner_bk += between_bk + ",";
			
			bk_counter += (std::count(between_bk.begin(), between_bk.end(), '(') + std::count(between_bk.begin(), between_bk.end(), '{'));
			bk_counter -= (std::count(between_bk.begin(), between_bk.end(), ')') + std::count(between_bk.begin(), between_bk.end(), '}'));
			inother_bk = bk_counter!=0 ? true : inother_bk;

			if (bk_counter == 0) {
				var_vec.push_back(this->parseRaw(inner_bk));
				inner_bk = "";
				inother_bk = false;
			}
		}
		else if (std::regex_match(between_bk.begin(), between_bk.end(), reg_r)) {
			bk_counter += (std::count(between_bk.begin(), between_bk.end(), '(') + std::count(between_bk.begin(), between_bk.end(), '{'));
			bk_counter -= (std::count(between_bk.begin(), between_bk.end(), ')') + std::count(between_bk.begin(), between_bk.end(), '}'));
			
			inner_bk += bk_counter == 0 && inother_bk ? between_bk : between_bk + ",";		
			if (bk_counter == 0) {
				var_vec.push_back(this->parseRaw(inner_bk));
				inner_bk = "";
				inother_bk = false;
			}
		}
		else if (!inother_bk) {
			std::regex_search(between_bk, match, reg_num, std::regex_constants::format_first_only);
			if (match.length() > 0 && match[0].length() == between_bk.length())
				var_vec.push_back(new vNumber(between_bk));
			else
				var_vec.push_back(this->parseRaw(between_bk));
		}
		else
			inner_bk += between_bk + ",";

		content.replace(0, pos + 1, "");
		pos = 0;
	}

	if (content.size() > 0) { // some thing behind last ','
		if (!inother_bk) { // ex: {1, 2,3,4," 5"}
			/*var_vec.push_back(this->parseRaw(content));*/
			std::regex_search(content, match, reg_num, std::regex_constants::format_first_only);
			if (match.length() > 0 && match[0].length() == content.length())
				var_vec.push_back(new vNumber(content));
			else
				var_vec.push_back(this->parseRaw(content));
		}
		else {// ex: {1, 2,3,4,{1,2,3,4,"5}"}
			inner_bk += content;
			var_vec.push_back(this->parseRaw(inner_bk));
		}
	}

	if (this->debug)
		std::cout << "[ArgParse End]" << std::endl;

	return var_vec;
}


std::string Parser::matchBracket(std::string * input_ref, int start_pos, std::string node_key, bool isVecArg)
{
	char  bk_l, bk_r;
	std::string sp;
	if (isVecArg) {
		bk_l = '{';
		bk_r = '}';
		sp = "\\{";
	}
	else {
		bk_l = '(';
		bk_r = ')';
		sp = "\\(";
	}
	std::smatch match;
	std::regex_search(*input_ref, match, std::regex(sp));

	std::string search_part = input_ref->substr(match.position()+1);
	std::string in_bk = "";
	int left_bk_c = 1;
	int end_pos = -1;

	for (int i = 0; i < search_part.length(); i++) {
		if (search_part[i] == bk_l)
			left_bk_c++;
		else if (search_part[i] == bk_r) {
			left_bk_c--;
			if (left_bk_c == 0) {
				end_pos = i;
				break;
			}
		}
	}

	if (end_pos != -1) {
		in_bk = search_part.substr(0, end_pos);
		input_ref->replace(match.position(), end_pos + 2, node_key);
	}

	return in_bk;
}

