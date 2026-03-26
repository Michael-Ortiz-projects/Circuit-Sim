#include "MNASystem.h"

MNASystem::MNASystem() { }

MNASystem::MNASystem(int nodeCount, int extraVars) {
    setSystem(nodeCount, extraVars);
}

void MNASystem::setSystem(int nodeCount, int extraVars) {
    n = nodeCount;
    m = extraVars;
    A = Eigen::MatrixXd::Zero(n + m, n + m);
    b = Eigen::VectorXd::Zero(n + m);
    A_static = Eigen::MatrixXd::Zero(n + m, n + m);
    b_static = Eigen::VectorXd::Zero(n + m);
    x = Eigen::VectorXd::Zero(n + m);
}

void MNASystem::setZero() {
    A.setZero();
    b.setZero();
}

void MNASystem::resetStatic() {
    A = A_static;
    b = b_static;
}

void MNASystem::addToAStatic(int j, int k, double value) {
    if (j >= 0 && k >= 0) {
        A_static(j, k) += value;
    }
}

void MNASystem::addTobStatic(int j, double value) {
    if (j >= 0) {
        b_static(j) += value;
    }
}

void MNASystem::addToA(int j, int k, double value) { 

    if (j >= 0 && k >= 0) {
        A(j, k) += value;
    }
}
void MNASystem::addTob(int j, double value){
    if (j >= 0) {
        b(j) = value;
    }
}

void MNASystem::printA() {
    std::cout << "\nMatrix A print:\n";
    std::cout << A << "\n\n";
}

void MNASystem::printb() {
    std::cout << "\RHS Vector print:\n";
    std::cout << b << "\n\n";
}

void MNASystem::printx() {
    std::cout << "\n X vector:\n";
    std::cout << x << "\n\n";
}
