#pragma once
#include <string>
#include <vector>

enum vType { base, number, vector, string, logic, function};

namespace vException {
	static std::string TypeException = "Unexpected Variable Type From:";
	static std::string UndefinedFunctionException = "Undefined Function:";
	static std::string VectorComputTypeException = "Incompatible Variable Type From Vector:";
	static std::string MatrixComputTypeException = "Incompatible Variable Type From Matrix:";
	static std::string MatrixNotCompatiableShapeException = "Incompatible Matrix Shape From Matrix:";
	static std::string IndexOutOfRangeException = "Index Out OF Range At:";
	static std::string WTF = "I Dont Know What The Hall You're Typing :(";
	static std::string NaN = "This Will Result NaN.";
}


class Variable {
public:
	std::string name = "undefine";
	std::string str_content;
	std::string data;

	virtual  vType  gettype() const;
	virtual  std::string  toString() const;
	virtual std::string toComputString() const;
	virtual Variable* selfCast();
	//virtual std::string getdata();

};


class vNumber: public Variable{
public:
	vNumber(double data);
	vNumber(std::string data);
	double data;

	vType gettype() const override;
	std::string  toString()const override;
	std::string toComputString()const override;
	vNumber* selfCast() override;

	vNumber* operator+(const Variable &n2);
	vNumber* operator-(const Variable &n2);
	vNumber* operator*(const Variable &n2);
	vNumber* operator/(const Variable &n2);
};

class vString : public Variable{
public:
	vString();

	//template<class T>
	vString* operator+(const Variable &n2);
};

class vBool : public Variable {
public:
	vBool();
	vBool(bool data);
	bool data;

	vType gettype() const override;
	std::string  toString()const override;
	std::string toComputString()const override;
	//template<class T>
};


class vVector : public Variable {
public:
	vVector();
	vVector(std::string data);
	vVector(std::vector<Variable*> data);

	std::vector<Variable*> data;

	vType gettype() const override;
	std::string  toString()const override;
	std::string toComputString()const override;

	bool isMarix();
	bool isVector();
	bool shapeCompatible(Variable* m2);

	std::vector<std::vector<double>> getMatrixData();
	void setMatrixData(std::vector<std::vector<double>> data);

	std::vector<double> getVectorData();
	void setVectorData(std::vector<double> data);
	

	vVector* operator*(const Variable &n2);
	vVector* operator+(const Variable &n2);
	vVector* operator-(const Variable &n2);
	vVector* operator/(const Variable &n2);
};


typedef Variable*(*FnPtr)(std::vector<Variable*>);

class vFunPtr : public Variable {
public:
	vFunPtr();
	vFunPtr(std::string fun_name, std::vector<Variable*> args);

	std::string fun_name;
	FnPtr function_ref;
	std::vector<Variable*> args;
	bool stander = false;

	vType gettype() const override;
};