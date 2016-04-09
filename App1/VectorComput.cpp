#pragma once
#include "pch.h"
#include "VectorComput.h"
#include <math.h>
#include <iostream>

typedef std::vector<Variable*> Args;

Variable* dot(Args args) {//2 args
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	double result_data = 0;

	for (int j = 0; j < v1->data.size(); j++)
		result_data += v_data1[j] * v_data2[j];

	vNumber* result = new vNumber(result_data);

	return result;
}

Variable* vectest(Args args) {
	std::cout << "casting !!!!!!!" << std::endl;
	return new vVector();
}

Variable* addition(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	std::vector<double> result_data;

	for (int j = 0; j < v1->data.size(); j++)
		result_data.push_back(v_data1[j] + v_data2[j]);

	vVector* result = new vVector();
	result->setVectorData(result_data);

	return result;
}

Variable* scalar_M(Args args) {
	Variable* vector1 = args[0];
	Variable* num = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (num->gettype() != vType::vector)
		throw vException::TypeException + num->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *n = dynamic_cast<vVector*>(num);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> n_data = n->getVectorData();
	std::vector<double> result_data;

	for (int j = 0; j < v1->data.size(); j++)
		result_data.push_back(v_data1[j] * n_data[0]);

	vVector* result = new vVector();
	result->setVectorData(result_data);

	return result;
}

Variable* norm_Vector(Args args) {
	Variable* num = args[0];
	Variable* vector1 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (num->gettype() != vType::number)
		throw vException::TypeException + num->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vNumber *n = dynamic_cast<vNumber*>(num);

	std::vector<double> v_data1 = v1->getVectorData();
	double n_data = n->data;
	double temp = 0;
	double result_data = 0;

	for (int j = 0; j < v1->data.size(); j++) {
		if (v_data1[j] < 0)
			v_data1[j] = -v_data1[j];
		for (int k = 0; k < n_data; k++) {
			if (k == 0)
				temp = v_data1[j];
			else
				temp = temp * v_data1[j];
		}
		result_data += temp;
	}

	double r1 = 0.5*result_data; //計算更號
	double r2 = 0.5*(r1 + result_data / r1);

	while ((r1>r2) || (r2 > r1))
		r1 = r2, r2 = 0.5*(r1 + result_data / r1);

	vNumber* result = new vNumber(r2);

	return result;
}

Variable* normalization(Args args) {
	Variable* vector = args[0];

	if (vector->gettype() != vType::vector)
		throw vException::TypeException + vector->name;

	vVector *v1 = dynamic_cast<vVector*>(vector);

	std::vector<double> v_data = v1->getVectorData();
	double temp = 0;

	for (int j = 0; j < v1->data.size(); j++)
		temp += v_data[j] * v_data[j];;

	double r1 = 0.5*temp;//計算更號
	double r2 = 0.5*(r1 + temp / r1);

	while ((r1>r2) || (r2 > r1))
		r1 = r2, r2 = 0.5*(r1 + temp / r1);

	for (int k = 0; k < v1->data.size(); k++)
		v_data[k] = v_data[k] / r2;

	vVector* result = new vVector();
	result->setVectorData(v_data);

	return result;
}

Variable* cross(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	std::vector<double> result_data;

	result_data.push_back(v_data1[1] * v_data2[2] - v_data1[2] * v_data2[1]);
	result_data.push_back(v_data1[2] * v_data2[0] - v_data1[0] * v_data2[2]);
	result_data.push_back(v_data1[0] * v_data2[1] - v_data1[1] * v_data2[0]);

	vVector* result = new vVector();
	result->setVectorData(result_data);

	return result;
}

Variable* comp(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	double result_data = 0;
	double temp = 0;

	for (int j = 0; j < v1->data.size(); j++)
		result_data += v_data1[j] * v_data2[j];

	for (int j = 0; j < v1->data.size(); j++)
		temp += v_data2[j] * v_data2[j];;

	double r1 = 0.5*temp;//計算更號
	double r2 = 0.5*(r1 + temp / r1);

	while ((r1>r2) || (r2 > r1))
		r1 = r2, r2 = 0.5*(r1 + temp / r1);

	result_data = result_data / r2;

	vNumber* result = new vNumber(result_data);
	return result;
}

Variable* proj(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	std::vector<double> result_data;
	double dot_re = 0;
	double temp = 0;

	for (int j = 0; j < v1->data.size(); j++)
		dot_re += v_data1[j] * v_data2[j];

	for (int j = 0; j < v1->data.size(); j++)
		temp += v_data2[j] * v_data2[j];;

	double r1 = 0.5*temp;//計算更號
	double r2 = 0.5*(r1 + temp / r1);

	while ((r1>r2) || (r2 > r1))
		r1 = r2, r2 = 0.5*(r1 + temp / r1);
	for (int k = 0; k < v2->data.size(); k++)
		result_data.push_back((dot_re / (r2*r2)) * v_data2[k]);

	vVector* result = new vVector();
	result->setVectorData(result_data);

	return result;
}

Variable* parallel(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	bool result = true;

	double time = v_data2[0] / v_data1[0];

	for (int j = 1; j < v1->data.size(); j++) {
		if (v_data2[j] / v_data1[j] != time) {
			result = false;
			break;
		}
	}

	vBool* re = new vBool(result);
	return re;
}

Variable* area(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	double temp1 = 0;
	double temp2 = 0;
	double temp3 = 0;

	for (int j = 0; j < v1->data.size(); j++) {
		temp1 += v_data1[j] * v_data1[j];
		temp2 += v_data2[j] * v_data2[j];
		temp3 += v_data1[j] * v_data2[j];
	}

	double temp_c = (temp1 * temp2) - (temp3*temp3);
	double r1 = 0.5*temp_c;//計算更號
	double r2 = 0.5*(r1 + temp_c / r1);

	while ((r1>r2) || (r2 > r1))
		r1 = r2, r2 = 0.5*(r1 + temp_c / r1);

	vNumber* result = new vNumber(r2 / 2);
	return result;
}

Variable* orthogonal(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	double result_data = 0;
	bool re = false;

	for (int j = 0; j < v1->data.size(); j++)
		result_data += v_data1[j] * v_data2[j];

	if (result_data == 0)
		re = true;

	vBool* result = new vBool(re);
	return result;
}


Variable* plane_normal(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	std::vector<double> result_data;

	result_data.push_back(v_data1[1] * v_data2[2] - v_data1[2] * v_data2[1]);
	result_data.push_back(v_data1[2] * v_data2[0] - v_data1[0] * v_data2[2]);
	result_data.push_back(v_data1[0] * v_data2[1] - v_data1[1] * v_data2[0]);

	vVector* result = new vVector();
	result->setVectorData(result_data);

	return result;
}


Variable* angle(Args args) {
	Variable* vector1 = args[0];
	Variable* vector2 = args[1];

	if (vector1->gettype() != vType::vector)
		throw vException::TypeException + vector1->name;
	else if (vector2->gettype() != vType::vector)
		throw vException::TypeException + vector2->name;

	vVector *v1 = dynamic_cast<vVector*>(vector1);
	vVector *v2 = dynamic_cast<vVector*>(vector2);

	std::vector<double> v_data1 = v1->getVectorData();
	std::vector<double> v_data2 = v2->getVectorData();
	double coss = 0;
	double dot = 0;
	double norm1 = 0;
	double norm2 = 0;
	double result_data;

	for (int j = 0; j < v1->data.size(); j++)
		dot += v_data1[j] * v_data2[j];
	for (int j = 0; j < v1->data.size(); j++) {
		norm1 += v_data1[j] * v_data1[j];
		norm2 += v_data2[j] * v_data2[j];
	}
	norm1 = sqrt(norm1);
	norm2 = sqrt(norm2);
	coss = dot / (norm1*norm2);
	result_data = acos(coss) * 180 / 3.14;

	vNumber* result = new vNumber(result_data);
	return result;
}