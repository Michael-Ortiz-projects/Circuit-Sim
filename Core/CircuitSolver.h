#pragma once
#include <iostream>
#include <algorithm> 
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

#include <iomanip>  
#include <cmath>
#include <memory>
#include "Component.h"
#include <Eigen/Dense>


class CircuitSolver {
public:
    std::vector<Component*> components;
    std::unordered_map<int, std::vector<std::pair<int, int>>> adjacencyList; // key = node ID, vector = list of (neighborNode, componentID)
    std::unordered_map<int, int> nodeToIndex;

    int referenceNode = -1;
    Eigen::MatrixXd A;
    Eigen::VectorXd b;
    Eigen::VectorXd x;
    std::vector<std::string> variables;

    CircuitSolver();

    void AddComponent(Component& component);

    int ConfigureCircuit();

    void SolveCircuit(float deltaTime);

    void ResetCircuit();
};