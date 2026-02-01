#pragma once
#include <iostream>
#include <Eigen//Dense>

class MNASystem {
public:
	MNASystem();
	MNASystem(int nodeCount, int extraVars);

	void setSystem(int nodeCount, int extraVars);
	void addToA(int j, int k, double value);
	void addTob(int j, double value);

	void printA();
	void printb();
private:
	int n, m;
	Eigen::MatrixXd A;
	Eigen::VectorXd b;
	
};