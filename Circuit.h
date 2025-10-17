#pragma once
#include <iostream>
#include <algorithm> 
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

#include <iomanip>  
#include <cmath>
#include "HelperFunctions.h"
#include <memory>
#include "global_variables.h"
#include "Component.h"
#include "Wire.h"
#include "TextBox.h"
#include <Eigen/Dense>


class Circuit {
public:
    std::vector<Component*> components;
    std::unordered_map<int, std::vector<std::pair<int, int>>> adjacencyList; //map with node as key, to a vector of neigboring nodes and the index of connecting component in the components vector
    std::unordered_map<int, int> nodeToIndex;
    
    int referenceNode = -1; 
    Eigen::MatrixXd A;
	Eigen::VectorXd b;
    Eigen::VectorXd x;
    std::vector<std::string> variables;


    void AddComponent(Component& component);

    int ConfigureCircuit();

    void SolveCircuit(float deltaTime);

    void ResetCircuit();
};