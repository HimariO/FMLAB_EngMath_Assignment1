
#include "pch.h"
#include "UIdataContainer.h"

using namespace Platform;
using namespace App1;

VarDisplay::VarDisplay(Platform::String^ A, Platform::String^ B) {
	this->Name = A;
	this->Var = B;
};

OutputDisplay::OutputDisplay(Platform::String^ A, Platform::String^ B) {
	this->In = A;
	this->Out = B;
}
