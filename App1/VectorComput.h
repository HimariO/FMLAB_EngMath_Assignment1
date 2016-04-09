#pragma once
#include "Variable.h"

typedef std::vector<Variable*> Args;

Variable* dot(Args args);
Variable* vectest(Args args);
Variable* addition(Args args);
Variable* scalar_M(Args args);
Variable* norm_Vector(Args args);
Variable* normalization(Args args);
Variable* cross(Args args);
Variable* comp(Args args);
Variable* proj(Args args);
Variable* parallel(Args args);
Variable* area(Args args);
Variable* orthogonal(Args args);
Variable* plane_normal(Args args);
Variable* angle(Args args);