#pragma once
#include "Variable.h"
#include <algorithm>
#include <iostream>

typedef std::vector<Variable*> Args;
typedef  std::vector<std::vector<double>> Matrix;
typedef  std::vector<double> Row;

int pivCheck(Row row, bool backword = false);
Variable* slice(Args args);
Variable* concat(Args args);
Variable* up_tri(Args args);
Variable* low_tri(Args args);
Variable* RREF(Args args);
Variable* rank(Args args);
Variable* trans(Args args);
Variable* det(Args args);
Variable* cofactor(Args args);
Variable* adj(Args args);
Variable* inverse(Args args);
Variable* solveLinearSys(Args args);
Variable* LeastSquare(Args args);
Variable* QR_Q(Args args);
Variable* eig(Args args);
Variable* mtxtest(Args args);