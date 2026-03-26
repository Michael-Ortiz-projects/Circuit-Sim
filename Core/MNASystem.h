#pragma once
#include <iostream>
#include <Eigen//Dense>

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

	void addToAStatic(int j, int k, double value);
	void addTobStatic(int j, double value);
	
	void addToA(int j, int k, double value);
	void addTob(int j, double value);

	void printA();
	void printb();
	void printx();

	Eigen::MatrixXd& getA() { return A; }
	Eigen::VectorXd& getb() { return b; }
	Eigen::VectorXd& getx() { return x; }

private:
	int n, m;
	Eigen::MatrixXd A_static;
	Eigen::VectorXd b_static;

	Eigen::MatrixXd A;
	Eigen::VectorXd b;
	Eigen::VectorXd x;
};