#include "LinearSolver.h"

void LinearSolver::analyzeSystem(MNASystem& sys) {
	sys.setA_static_FromTriplets();
	solver.analyzePattern(sys.getA_static());
}

void LinearSolver::factorize(MNASystem& sys) {
	solver.factorize(sys.getA());
	if (solver.info() != Eigen::Success)
	{
		std::cout << "Factorization failed\n";
	}
}


bool LinearSolver::solve(MNASystem& sys) {
	sys.getx() = solver.solve(sys.getb());
	if (solver.info() != Eigen::Success)
	{
		std::cout << "Solve failed\n";
		return false;
	}
	return true;
}