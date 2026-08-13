#pragma once
#include <iostream>
#include <Eigen//Dense>
#include <Eigen/Sparse>
#include <Eigen/Core>

enum class ExtraVarType { Current, Voltage };

struct ExtraVarInfo {
	int index;
	ExtraVarType type;
	int componentID;
	std::string label;
};

class MNASystem {
public:
	std::vector<ExtraVarInfo> extraVars;

	MNASystem();
	MNASystem(int nodeCount, int extraVars);

	void setSystem(int nodeCount, int extraVars);
	void setZero();
	void resetStatic();

	void setA_static_FromTriplets();
	void addToAStatic(int j, int k, double value);
	void addTobStatic(int j, double value);

	void addToADynamic(int i, int j, double value);
	void addTobDynamic(int i, double value);
	


	void printA();
	void printb();
	void printx();

	Eigen::SparseMatrix<double>& getA() { return A; }
	Eigen::SparseMatrix<double>& getA_static() { return A_static; }
	Eigen::VectorXd& getb() { return b; }
	Eigen::VectorXd& getx() { return x; }

private:
	int n, m;
	Eigen::SparseMatrix<double> A_static, A;
	Eigen::VectorXd b_static, b;

	std::vector<Eigen::Triplet<double>> staticTriplets;

	Eigen::VectorXd x;

	/*
	I should make functions addToAStatic and addToADynamic (same with b vector) and static doesnt actually stamp to the matrix but uses the triplet buffers while dynamic uses coeff ref to stamp right into the matrices
	void MNASystem::addToAStatic(int i, int j, double value) {
		staticTriplets.emplace_back(i, j, value);
	}

	void MNASystem::addTobStatic(int i, double value) {
		b_static(i) += value;
	}

	void MNASystem::addToADynamic(int i, int j, double value) {
		A.coeffRef(i, j) += value;
	}

	void MNASystem::addTobDynamic(int i, double value) {
		b(i) += value;
	}


	for each timestep:

	A = A_static
	b = b_static

	for dynamic components
		comp->stampDynamic(system)

	solver.factorize(A)
	x = solver.solve(b)

	*/
};