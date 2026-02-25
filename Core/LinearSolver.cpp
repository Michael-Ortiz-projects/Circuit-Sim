#include "LinearSolver.h"

bool LinearSolver::solve(MNASystem& sys) {
	sys.getx() = sys.getA().colPivHouseholderQr().solve(sys.getb());
	return true;
}