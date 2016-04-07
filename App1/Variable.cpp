#pragma once
#include "pch.h"
#include <iostream>
#include <string>
#include <regex>
#include "Variable.h"


vType Variable::gettype() const
{
	return vType::base;
}

std::string Variable::toString() const
{
	return this -> data;
}

std::string Variable::toComputString() const
{
	return "";
}

Variable* Variable::selfCast() {
	return this;
}


//Number Section Strat......

vNumber::vNumber(double data)
{
	this->data = data;
}

vNumber::vNumber(std::string data)
{
	this->data = std::stod(data);
}


vType vNumber::gettype() const {
	return vType::number;
}


std::string vNumber::toString() const
{
	return std::to_string(this->data);
}

std::string vNumber::toComputString() const
{
	return std::to_string(this->data);
}


vNumber* vNumber::selfCast() {
	return this;
}


vNumber* vNumber::operator+( const Variable &n2) {

	if (n2.gettype() == vType::number) {
		vNumber &cas = (vNumber&)(n2);
		return new vNumber(this->data + cas.data);
	}
	return NULL;
}


vNumber* vNumber::operator-(const Variable & n2)
{
	if (n2.gettype() == vType::number) {
		vNumber &cas = (vNumber&)(n2);
		return new vNumber(this->data - cas.data);
	}

	return NULL;
}


vNumber * vNumber::operator*(const Variable & n2)
{
	if (n2.gettype() == vType::number) {
		vNumber &cas = (vNumber&)(n2);
		return new vNumber(this->data * cas.data);
	}
	return nullptr;
}


vNumber * vNumber::operator/(const Variable & n2)
{
	if (n2.gettype() == vType::number) {
		vNumber &cas = (vNumber&)(n2);
		return new vNumber(this->data / cas.data);
	}
	return nullptr;
}


// vString Section ......


vString::vString() {
}


vString * vString::operator+(const Variable & n2)
{
	return nullptr;
}


//vVector  Section......
vVector::vVector(std::vector<Variable*> data) {
	this->data = data;
}


vVector::vVector()
{
}


vVector::vVector(std::string data) //  [Discard for now]
{
	std::smatch match;
	std::regex_search(data, match, std::regex("\\{(.*)\\}"));
	std::string content = match[1];

	size_t pos = 0; //remove redunden space
	while ((pos = content.find(" ", pos)) != std::string::npos) {
		content.replace(pos, 1, "");
	}

	std::vector<Variable*> var_vec;
	pos = 0;
	while ((pos = content.find(",", pos)) != std::string::npos) {
		//num_vec.push_back(new vNumber(content.substr(0, pos)));
		
		content.replace(0, pos+1, "");
	}
	if (content.size() > 0)
		var_vec.push_back(new vNumber(content));
	

}


vType vVector::gettype() const
{
	return vType::vector;
}


std::string vVector::toString() const
{
	std::string output = "{";

	for (auto v : this->data) { 
		if (v->gettype() == vType::vector)
			output += v->toString() + ",\n";
		else
			output += v->toString() +",";
	}
	output += "}";
	return output;
}

std::string vVector::toComputString() const
{
	std::string output = "{";

	for (auto v : this->data) {
		output += v->toString() + ",";
	}
	output += "}";
	return output;
}


vVector * vVector::operator*(const Variable & n2)
{
	if (this->isMarix()) {
		if (n2.gettype() != vType::vector)
			throw std::runtime_error(vException::TypeException + n2.name);

		const vVector* Vec2 = dynamic_cast<const vVector*>(&n2);
		vVector* cast_Vec2 = const_cast<vVector*>(Vec2);

		if (cast_Vec2->isMarix()) {

			std::vector<std::vector<double>> dMatrix_1 = this->getMatrixData();
			std::vector<std::vector<double>> dMatrix_2 = cast_Vec2->getMatrixData();

			std::vector<std::vector<double>> result_Matrix;

			for (int n = 0; n < dMatrix_1.size(); n++) {
				std::vector<double> *row = new std::vector<double>;

				for (int m = 0; m < dMatrix_2[0].size(); m++) {
					double n_m=0;
					
					for (int i = 0; i < dMatrix_1.size(); i++) {
						double lef = dMatrix_1[n][i];
						double righ = dMatrix_2[i][m];
						n_m += dMatrix_1[n][i] * dMatrix_2[i][m];

					}
					row->push_back(n_m);
				}
				result_Matrix.push_back(*row);
			}

			vVector *result = new vVector();
			result->setMatrixData(result_Matrix);

			return result;
		}
	}
	else { // scalar multiplication with vector 
		if (n2.gettype() != vType::number)
			throw std::runtime_error(vException::TypeException + n2.name);

		std::vector<Variable*> *nVector = new std::vector<Variable*>;

		for (int i = 0; i < this->data.size();i++) {
			vNumber *tem = dynamic_cast<vNumber*>(this->data[i]);
			Variable *res = *tem * n2;	
			nVector->push_back(res);
		}

		return new vVector(*nVector);
	}

}

vVector * vVector::operator+(const Variable & n2)
{
	if(n2.gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + n2.name);

	return nullptr;
}

vVector * vVector::operator-(const Variable & n2)
{
	return nullptr;
}

vVector * vVector::operator/(const Variable & n2)
{
	return nullptr;
}


bool vVector::isMarix()
{

	for (auto d1_content : this->data) {
		if (d1_content->gettype() != vType::vector)
			return false;
	}
	
	return true;
}

bool vVector::isVector()
{
	for (auto ele : this->data)
		if (ele->gettype() != vType::number)
			return false;
	return true;
}


bool vVector::shapeCompatible(Variable * m2)
{
	vVector *m2_v = dynamic_cast<vVector*>(m2);
	if (m2_v->isMarix()) {
		std::vector<std::vector<double>> mtx = this->getMatrixData();
		std::vector<std::vector<double>> mtx2 = m2_v->getMatrixData();

		for (auto row : mtx2)
			if (row.size() == mtx.size())
				return false;

		return true;
	}
	return false;
}


std::vector<std::vector<double>> vVector::getMatrixData()
{
	std::vector<std::vector<double>> result;

	for (auto v_vector : this->data) {
		std::vector<double> *t = new std::vector<double>();
		vVector *tv = dynamic_cast<vVector*>(v_vector);

		for (auto v_var : tv->data) {
			t->push_back(std::stod(v_var->toString()));
		}
		result.push_back(*t);
	}

	return result;
}

void vVector::setMatrixData(std::vector<std::vector<double>> data)
{
	this->data.clear();

	for (auto ivect : data) {
		std::vector<Variable*> tem;
		for (auto value : ivect) {
			tem.push_back(new vNumber(value));
		}
		this->data.push_back(new vVector(tem));
	}

	
}

std::vector<double> vVector::getVectorData()
{
	std::vector<double> result;

	for (auto v_e : this->data) {
		if (v_e->gettype() != vType::number)
			throw std::runtime_error(vException::VectorComputTypeException + v_e->name);
		vNumber *n = dynamic_cast<vNumber*>(v_e);
		result.push_back(n->data);
	}

	return result;
}

void vVector::setVectorData(std::vector<double> data)
{
	this->data.clear();

	for (auto num : data) 
		this->data.push_back(new vNumber(num)); 
}




vFunPtr::vFunPtr()
{
}

vFunPtr::vFunPtr(std::string fun_name, std::vector<Variable*> args)
{
	this->name = fun_name;
	this->fun_name = fun_name;
	this->args = args;

	for (int i = 0; i < args.size(); i++) {
		Variable *arg = args[i];
		if (arg->gettype() == vType::function) {
			vFunPtr *fn = dynamic_cast<vFunPtr*>(arg);
			FnPtr _fun_ref = fn->function_ref;
			args[i] = (*_fun_ref)(fn->args);
		}
	}
}

vType vFunPtr::gettype() const
{
	return vType::function;
}

vBool::vBool()
{
}

vBool::vBool(bool data)
{
	this->data = data;
}

vType vBool::gettype() const
{
	return vType::logic;
}

std::string vBool::toString() const
{
	if (this->data)
		return "true";
	return "false";
}

std::string vBool::toComputString() const
{
	if (this->data)
		return "true";
	return "false";
}
