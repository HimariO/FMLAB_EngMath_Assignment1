#pragma once

using namespace Platform;

namespace App1
{
	public ref class VarDisplay sealed {
		Platform::String ^rName;
		Platform::String ^rVar = "Var";
	public:
		VarDisplay(String^ A, String^ B);

		property String^ Name {
			String^ get() { return this->rName; };
			void set(String^ s) { this->rName = s; };
		};

		property String^ Var {
			String^ get() {
				return this->rVar;
			};
			void set(String^ s) { this->rVar = s; };
		};
	};

	public ref class OutputDisplay sealed {
		Platform::String ^rIn;
		Platform::String ^rOut;
	public:
		OutputDisplay(String^ A, String^ B);

		property String^ In {
			String^ get() { return this->rIn; };
			void set(String^ s) { this->rIn = ">> " + s; };
		};

		property String^ Out {
			String^ get() {
				return this->rOut;
			};
			void set(String^ s) { this->rOut = s; };
		};
	};
};