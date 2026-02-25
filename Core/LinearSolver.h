#pragma once
#include "MNASystem.h"

class LinearSolver {
public:
	LinearSolver() {}

	bool solve(MNASystem& sys);

private:
};