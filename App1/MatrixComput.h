#pragma once
#include "Variable.h"
#include <algorithm>
#include <iostream>

typedef std::vector<Variable*> Args;
typedef  std::vector<std::vector<double>> Matrix;
typedef  std::vector<double> Row;


int pivCheck(Row row, bool backword = false) {
	if (backword) {
		for (int pos = row.size()-1; pos >= 0; pos--)
			if (row[pos] != 0 )
				return pos;
	}
	else {
		for (int pos = 0; pos < row.size(); pos++)
			if (row[pos] != 0)
				return pos;
	}
	return -1;
}


Variable* slice(Args args) {// (matrix, n_0, n_1, m_0, m_1)args
	Variable* matrix = args[0];
	

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);

	vVector *vmatrix = dynamic_cast<vVector*>(matrix);
	int n_start = dynamic_cast<vNumber*>(args[1])->data;
	int n_end = dynamic_cast<vNumber*>(args[2])->data;
	int m_start = dynamic_cast<vNumber*>(args[3])->data;
	int m_end = dynamic_cast<vNumber*>(args[4])->data;

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);
	
	Matrix mtx = vmatrix->getMatrixData();
	Matrix slice_mtx;

	if (n_start < 0 || m_start < 0 || n_end >= mtx.size() || m_end >= mtx[0].size())
		throw std::runtime_error(vException::IndexOutOfRangeException + matrix->name);

	//get matrix slice  mtx[n_start:n_end][m_start:m_end]

	for (int n = n_start; n <= n_end; n++) {
		Row new_row;
		for (int m = m_start; m <= m_end; m++) {
			new_row.push_back(mtx[n][m]);
		}
		slice_mtx.push_back(new_row);
	}

	vVector *result = new vVector();
	result->setMatrixData(slice_mtx);
	return result;
}


Variable* concat(Args args) {// (matrix_1, matrix_1, concat_at_bottom:bool)args
	Variable* matrix = args[0];
	Variable* matrix_2 = args[1];

	if (matrix->gettype() != vType::vector || matrix_2->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);

	vVector *vmatrix = dynamic_cast<vVector*>(matrix);
	vVector *vmatrix_2 = dynamic_cast<vVector*>(matrix_2);
	vBool *isTopdownStyle = dynamic_cast<vBool*>(args[2]);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);
	//get matrix slice  mtx[n_start:n_end][m_start:m_end]

	Matrix mtx = vmatrix->getMatrixData();
	Matrix mtx_2 = vmatrix_2->getMatrixData();
	Matrix mtx_new;
	bool istop = isTopdownStyle->data;

	for (int n = 0; n < mtx.size() + mtx_2.size(); n++) {
		Row newrow;
		if (!istop && n >= mtx.size())
			break;

		for (int m = 0; m < mtx[0].size() + mtx_2[0].size(); m++) {
			if (! istop) {
				if (m >= mtx[0].size())
					newrow.push_back(mtx_2[n][m - mtx[0].size()]);
				else 
					newrow.push_back(mtx[n][m]);
			}
			else {
				if (m >= mtx[0].size())
					break;
				else if (n >= mtx.size()) 
					newrow.push_back(mtx_2[n - mtx.size()][m]);
				else
					newrow.push_back(mtx[n][m]);
			}
		}
		mtx_new.push_back(newrow);
	}

	vVector *result = new vVector();
	result->setMatrixData(mtx_new);
	return result;
}


Variable* up_tri(Args args) {
	// standar opertaion to start a MatrixComput function.
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);

	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();

	// get Row Reduced Echelon Form(computing part)
	std::map<int, int> order_record;
	std::vector<int> work_through;

	for (int i = 0; i < mtx.size(); i++) {
		int piv_ind = i;
		int counter = 0;

		while (true) {
			Row piv_row = mtx[piv_ind];
			if (counter >= mtx.size())
				break;

			int check = pivCheck(piv_row);
			if (piv_row[i] == 0
				|| check != i || check == -1) {
				piv_ind = (piv_ind + 1) % mtx.size();
				counter++;
				continue;
			}

			for (int down_row = 0; down_row < mtx.size(); down_row++) {
				if (down_row == piv_ind
					|| std::find(work_through.begin(), work_through.end(), down_row)!=work_through.end())
					continue;

				double scalar = mtx[down_row][i]/mtx[piv_ind][i];
				for (int row_index = 0; row_index < piv_row.size(); row_index++) {
					mtx[down_row][row_index] -= mtx[piv_ind][row_index] * scalar;

					if (std::abs(mtx[down_row][row_index]) < 1.e-10)
						mtx[down_row][row_index] = 0;
				}
			}

			std::cout << "i:" << i << "  work:" << work_through.size() << std::endl;
			for (auto rows : mtx) {
				std::cout  << "{";
				for (auto ele : rows) {
					std::cout << ele << ",";
				}
				std::cout  << "}" << std::endl;
			}
			std::cout << std::endl;

			order_record.insert(std::pair<int, int>(i, piv_ind));
			work_through.push_back(piv_ind);
			break;
		}

	}
	//sorting to upper matrix form.
	Matrix sorted;
	for (auto pair : order_record) {
		sorted.push_back(mtx[pair.second]);
	}

	// standar way to return result.
	vVector * result = new vVector();
	result->setMatrixData(mtx);

	return result;
}


Variable* low_tri(Args args) {
	// standar opertaion to start a MatrixComput function.
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);

	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();

	// get Row Reduced Echelon Form(computing part)
	std::map<int, int> order_record;
	std::vector<int> work_through;

	for (int i = mtx.size()-1; i >=0 ; i--) {
		int piv_ind = i;
		int counter = 0;

		while (true) {
			Row piv_row = mtx[piv_ind];
			if (counter >= mtx.size())
				break;

			int check = pivCheck(piv_row, true);
			if (piv_row[i] == 0
				|| check != i || check == -1) {
				piv_ind = (piv_ind + 1) % mtx.size();
				counter++;
				continue;
			}

			for (int j = 0; j< piv_row.size(); j++) 
				mtx[piv_ind][j] /= mtx[piv_ind][i];
				
			for (int down_row = 0; down_row < mtx.size(); down_row++) {
				if (down_row == piv_ind
					|| std::find(work_through.begin(), work_through.end(), down_row) != work_through.end())
					continue;

				double scalar =  mtx[down_row][i];
				for (int row_index = 0; row_index < piv_row.size(); row_index++) {
					mtx[down_row][row_index] -= mtx[piv_ind][row_index] * scalar;
				}
			}

			order_record.insert(std::pair<int, int>(i, piv_ind));
			work_through.push_back(piv_ind);
			break;
		}

	}

	//sorting to lower matrix form.
	Matrix sorted;
	for (auto pair : order_record) {
		sorted.push_back(mtx[pair.second]);
	}

	// standar way to return result.
	vVector * result = new vVector();
	result->setMatrixData(sorted);

	return result;
}


Variable* RREF(Args args) {
// standar opertaion to start a MatrixComput function.
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);
	
	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if(!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();

	// get Row Reduced Echelon Form(computing part)
	std::map<int,  int> order_record;

	for (int i = 0; i < mtx.size(); i++) {
		int piv_ind = i;
		int counter = 0;

		while (true) {
			Row piv_row = mtx[piv_ind];
			if (counter >= mtx.size())
				break;
			
			int check = pivCheck(piv_row);
			if (piv_row[i] == 0 
				||  check != i || check ==-1 ) {
				piv_ind = (piv_ind + 1) % mtx.size();
				counter++;
				continue;
			}
			
			for (int j = piv_row.size() - 1; j >= 0; j--)
				mtx[piv_ind][j] /= mtx[piv_ind][i];

			for (int down_row = 0; down_row < mtx.size(); down_row++) {
				if (down_row == piv_ind)
					continue;

				double scalar = mtx[down_row][i];
				for (int row_index = 0; row_index < piv_row.size(); row_index++) {
					mtx[down_row][row_index] -= mtx[piv_ind][row_index] * scalar;
				}
			}

			order_record.insert(std::pair<int, int>(i, piv_ind));
			break;
		}

	}
	//sorting to upper matrix form.
	Matrix sorted;
	for (auto pair : order_record) {
		sorted.push_back(mtx[pair.second]);
	}

	// standar way to return result.
	vVector * result = new vVector();
	result->setMatrixData(sorted);

	return result;
}


Variable* trans(Args args) { // matrix transpose
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);

	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();
	Matrix new_m;

	for (int n = 0; n < mtx[0].size(); n++) {
		Row row;
		for (int m = 0; m < mtx.size(); m++) {
			row.push_back(mtx[m][n]);
		}
		new_m.push_back(row);
	}

	vVector *result = new vVector();
	result->setMatrixData(new_m);
	return result;
}


Variable* det(Args args) { //matrix determinant (matrix)1 args
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);
	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();
	double result_d = 0;

	if (mtx.size() == 2 && mtx[0].size() == 2) {
		return new vNumber(mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]);
	}
	else {
		vmatrix = dynamic_cast<vVector*>(up_tri(Args{ matrix }));
		mtx = vmatrix->getMatrixData();
		int limit = mtx[0].size() < mtx.size() ? mtx[0].size() : mtx.size();
		for (int n = 0; n < limit; n++)
			result_d *= mtx[n][n];
	}

	vNumber *result = new vNumber(result_d);
	return result;
}


Variable* cofactor(Args args) { //matrix determinant (matrix)1 args
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);
	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();
	Matrix cof_mtx;
	vVector *result = new vVector();

	if (mtx.size() == 2 && mtx[0].size() == 2) {
		result->setMatrixData(Matrix{ Row{ mtx[1][1], -mtx[1][1] }, Row{ -mtx[0][1], mtx[0][0] } });
	}
	else {
		for (int i = 0; i < mtx.size(); i++) {
			Row cof_row;
			for (int j = 0; j < mtx[0].size(); j++) {
				double scalar = 0;
				if (i % 2 == 0)
					scalar = (j % 2 == 0) ? 1 : -1;
				else
					scalar = (j % 2 == 0) ? -1 : 1;

				Matrix det_mtx;
				for (int n = 0; n < mtx.size(); n++){
					Row det_row;
					if (n == i)
						continue;
					for (int m = 0; m < mtx[0].size(); m++) {
						if (m == j)
							continue;
						det_row.push_back(mtx[n][m]);
					}
					det_mtx.push_back(det_row);
				}
				vVector *temp_mtx = new vVector();
				temp_mtx->setMatrixData(det_mtx);

				vNumber *det_ij= dynamic_cast<vNumber*>(det(Args{ temp_mtx }));
				cof_row.push_back(scalar * det_ij->data);
			}
			cof_mtx.push_back(cof_row);
		}
		result->setMatrixData(cof_mtx);
	}

	return result;
}


Variable* adj(Args args) { // Adjugate matrix (matrix)1 args
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);
	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();
	Matrix new_mtx;
	vVector *result = new vVector();

	if (mtx.size() == 2 && mtx[0].size() == 2) {
		result->setMatrixData(Matrix{ Row{mtx[1][1], -mtx[0][1]}, Row{ -mtx[1][0], mtx[0][0] } });
	}
	else {
		//Variable *cof_m = trans(Args{ cofactor(Args{matrix}) }); [not sure this sure transpose or not...]
		Variable *cof_m = cofactor(Args{ matrix });
		result = dynamic_cast<vVector*>(cof_m);
	}

	return result;
}


Variable* inverse(Args args) { //matrix inverse (matrix)1 args
	Variable* matrix = args[0];

	if (matrix->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);
	vVector *vmatrix = dynamic_cast<vVector*>(matrix);

	if (!vmatrix->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx = vmatrix->getMatrixData();
	Matrix new_mtx;
	vVector *result = new vVector();

	Variable *det_v = det(Args{ matrix });
	vNumber *det = dynamic_cast<vNumber*>(det_v);
	
	if (det->data == 0)
		throw std::runtime_error(vException::NaN);

	Variable *cof_mtx_v = cofactor(Args{ matrix });
	vVector *cof_mtx = dynamic_cast<vVector*>(cof_mtx_v);

	for (auto row : cof_mtx->getMatrixData()) {
		Row n_row;
		for (auto ele : row) 
			n_row.push_back(ele / det->data);
		new_mtx.push_back(n_row);
	}

	result->setMatrixData(new_mtx);
	return result;
}


Variable* solveLinearSys(Args args) { //solve linear system (matrix, nx1 matrix)2 args
	Variable* matrix = args[0];
	Variable* vector_b = args[1];

	if (matrix->gettype() != vType::vector && vector_b->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);

	vVector *vmatrix = dynamic_cast<vVector*>(matrix);
	vVector *vvector_b = dynamic_cast<vVector*>(vector_b);

	if (!vmatrix->isMarix() || !vvector_b->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx_A = vmatrix->getMatrixData();
	Matrix mtx_B = vvector_b->getMatrixData();

	if (mtx_A[0].size() != mtx_B.size())
		throw std::runtime_error(vException::MatrixNotCompatiableShapeException +vmatrix->name +", "+vvector_b->name);

	Variable *sys_mtx_v = concat(Args{ vmatrix, vector_b, new vBool(false) });
	Variable *solved_mtx_v = RREF(Args{ sys_mtx_v });
	Variable *result = slice(Args{
		solved_mtx_v ,
		new vNumber(0),
		new vNumber(mtx_A.size()-1),
		new vNumber(mtx_A[0].size()),
		new vNumber(mtx_A[0].size()),
	});

	return result;
}


Variable* LeastSquare(Args args) {  //LeastSquare (matrix, nx1 matrix)2 args
	Variable* matrix = args[0];
	Variable* vector_b = args[1];

	if (matrix->gettype() != vType::vector && vector_b->gettype() != vType::vector)
		throw std::runtime_error(vException::TypeException + matrix->name);

	vVector *vmatrix = dynamic_cast<vVector*>(matrix);
	vVector *vmatrix_b = dynamic_cast<vVector*>(vector_b);

	if (!vmatrix->isMarix() || !vmatrix_b->isMarix())
		throw std::runtime_error(vException::TypeException + matrix->name);

	Matrix mtx_A = vmatrix->getMatrixData();
	Matrix mtx_B = vmatrix_b->getMatrixData();

	if (mtx_A[0].size() != mtx_B.size())
		throw std::runtime_error(vException::MatrixNotCompatiableShapeException + vmatrix->name + ", " + vmatrix_b->name);
	
	//At*A*x = At * b
	vmatrix_b = *dynamic_cast<vVector*>(trans(Args{ vmatrix })) * (*vmatrix_b); //At*b
	vmatrix = *dynamic_cast<vVector*>(trans(Args{ vmatrix })) * (*vmatrix); //At*A
	
	std::cout << "At*A:" << vmatrix->toString() << std::endl;
	std::cout << "At*b:" << vmatrix_b->toString() << std::endl;
	Variable *result= solveLinearSys(Args{ vmatrix, vmatrix_b });

	return result;
}


Variable* mtxtest(Args args) {// ?? args
	Args mix_arg{ args[0],args[1], new vBool(false ) };
	return concat(mix_arg);
}


//Variable* det(Args args) { //matrix determinant (matrix)1 args
//	Variable* matrix = args[0];
//
//	if (matrix->gettype() != vType::vector)
//		throw std::runtime_error(vException::TypeException + matrix->name);
//	vVector *vmatrix = dynamic_cast<vVector*>(matrix);
//
//	if (!vmatrix->isMarix())
//		throw std::runtime_error(vException::TypeException + matrix->name);
//
//	Matrix mtx = vmatrix->getMatrixData();
//	double result_d = 0;
//
//	if(mtx.size()==2 && mtx[0].size()==2){
//		return new vNumber(mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]);
//	}
//	else {	
//		Row row = mtx[0];
//
//		for (int m = 0; m < row.size(); m++) {
//			double subDet_d = 0;
//
//			if (m == 0) {
//				Variable *sliced = slice(Args{ matrix, new vNumber(1), new vNumber(mtx.size() - 1), new vNumber(1), new vNumber(row.size()-1)});
//				vNumber *subDet = dynamic_cast<vNumber*>(det(Args{ sliced }));
//				subDet_d = subDet->data;
//
//			}
//			else if (m == row.size() - 1) {
//				Variable *sliced = slice(Args{ matrix, new vNumber(1), new vNumber(mtx.size()-1), new vNumber(0), new vNumber(m - 1) });
//				
//				vNumber *subDet = dynamic_cast<vNumber*>(det(Args{ sliced }));
//				subDet_d = subDet->data;
//			}
//			else {
//				Variable *sliced_left = slice(Args{ matrix, new vNumber(1), new vNumber(mtx.size()-1), new vNumber(0), new vNumber(m - 1) });
//				Variable *sliced_right = slice(Args{ matrix, new vNumber(1), new vNumber(mtx.size()-1), new vNumber(m + 1), new vNumber(row.size() - 1) });
//				Variable *concated = concat(Args{ sliced_left, sliced_right, new vBool(false) });
//
//				vNumber *subDet = dynamic_cast<vNumber*>(det(Args{ concated }));
//				subDet_d = subDet->data;
//			}
//
//			if (m % 2 == 0) { //even cloume
//				result_d += row[m] * subDet_d;
//			}
//			else { //odd cloume
//				result_d -= row[m] * subDet_d;
//			}
//		}
//	}
//
//	vNumber *result = new vNumber(result_d);
//	return result;
//}