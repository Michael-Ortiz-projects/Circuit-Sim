#pragma once
#include "MNASystem.h"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
class LinearSolver {
public:
	LinearSolver() {}

	void analyzeSystem(MNASystem& sys);
	void factorize(MNASystem& sys);

	bool solve(MNASystem& sys);

private:
	Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
};