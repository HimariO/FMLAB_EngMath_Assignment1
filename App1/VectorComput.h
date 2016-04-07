#pragma once
#include "Variable.h"
#include <iostream>
#include <stdarg.h> 

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
	std::vector<double> result_data;

	for (int j = 0; j < v1->data.size(); j++)
		result_data.push_back(v_data1[j] * v_data2[j]);
	
	vVector* result = new vVector();
	result->setVectorData(result_data);

	return result;
}

Variable* vectest(Args args) {
	std::cout << "casting !!!!!!!" << std::endl;
	return new vVector();
}