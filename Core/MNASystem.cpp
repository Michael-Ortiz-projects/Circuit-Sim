#include "MNASystem.h"

MNASystem::MNASystem() { }

MNASystem::MNASystem(int nodeCount, int extraVars) {
    setSystem(nodeCount, extraVars);
}

void MNASystem::setSystem(int nodeCount, int extraVars) {
    n = nodeCount;
    m = extraVars;

    A.resize(n + m, n + m);
    A.setZero();          // sparse zero-initialization
    b = Eigen::VectorXd::Zero(n + m);

    A_static.resize(n + m, n + m);
    A_static.setZero();
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

void MNASystem::setA_static_FromTriplets() {
    A_static.setFromTriplets(staticTriplets.begin(), staticTriplets.end());
    A_static.makeCompressed();
}

void MNASystem::addToAStatic(int j, int k, double value) {
    staticTriplets.emplace_back(j, k, value);
}

void MNASystem::addTobStatic(int j, double value) {
    b_static(j) += value;
}

void MNASystem::addToADynamic(int i, int j, double value) {
    A.coeffRef(i, j) += value;
}

void MNASystem::addTobDynamic(int i, double value) {
    b(i) += value;
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
