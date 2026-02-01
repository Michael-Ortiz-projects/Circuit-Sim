#pragma once
#include <iostream>
#include <set>
#include "LinearSolver.h"
#include "MNASystem.h"
#include "NetlistComponent.h"
#include "SimComps/SimulationComponent.h"
#include "SimComps/SimResistor.h"
#include "SimComps/SimCurrentSource.h"
#include "SimComps/SimGround.h"

class Simulator {
public:
	Simulator();

	void setSystem(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes);

	std::unique_ptr<SimulationComponent> buildSimulationComponent(const NetlistComponent& netlistComp);
	int buildMNAMap(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes);


	bool runDC();

private:

	LinearSolver solver;
	MNASystem system;
	std::vector<std::unique_ptr<SimulationComponent>> simComponents;
	std::unordered_map<int, int> eNodeToMNA;
	

	int getMNAIndex(const int eNode);


};