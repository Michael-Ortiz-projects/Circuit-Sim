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
}

void MNASystem::addToA(int j, int k, double value) { 
    std::cout << "addToA() is running\n";
    std::cout << "index = (" << j << ", " << k << ")\n";
    if (j >= 0 && k >= 0) {
        std::cout << "condition is true\n";
        std::cout << "adding " << value << " to index (" << j << ", " << k << ")\n";
        A(j, k) += value;
        std::cout << "got here\n";
    }
}
void MNASystem::addTob(int j, double value){
    std::cout << "addTob() running\n";
    std::cout << "index = " << j << "\n";
    if (j >= 0) {
        std::cout << "adding " << value << " to index (" << j << ")\n";
        b(j) = value;
    }
    std::cout << "addTob() finished\n";
}

void MNASystem::printA() {
    std::cout << "\nMatrix A print:\n";
    std::cout << A << "\n\n";
}

void MNASystem::printb() {
    std::cout << "\RHS Vector print:\n";
    std::cout << b << "\n\n";
}
