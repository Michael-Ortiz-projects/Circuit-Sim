#pragma once
#include <set>
#include "LinearSolver.h"
#include "MNASystem.h"
#include "NetlistComponent.h"
#include "SimComps/SimulationComponent.h"
#include "SimComps/SimResistor.h"
#include "SimComps/SimCurrentSource.h"
#include "SimComps/SimGround.h"
#include "SimComps/SimInductor.h"
#include "SimComps/SimVoltageSource.h"
#include "SimComps/SimCapacitor.h"
#include "SimComps/SimVoltageControlledVoltageSource.h"
#include "SimComps/SimVoltageControlledCurrentSource.h"
#include "SimComps/SimCurrentControlledCurrentSource.h"
#include "SimComps/SimCurrentControlledVoltageSource.h"

struct Config {
	//TRANSIENT CONFIG
	double timeStep = .00001;
	double tStart = 0;
	double tEnd = .2;

	bool transientValid() const {
		return timeStep > 0 && tEnd > tStart && timeStep < abs(tEnd - tStart);
	}
};



class Simulator {
public:
	std::unordered_map<int, int> eNodeToMNA;
	MNASystem system;
	std::vector<TransientSimulationState> transientResults;
	std::vector<TransientGraphVariable> graphVariables;

	Simulator(std::vector<std::unique_ptr<SimulationComponent>>& simComps);

	bool setSystem(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes);

	std::unique_ptr<SimulationComponent> buildSimulationComponent(const NetlistComponent& netlistComp);
	int buildMNAMap(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes);


	bool runDC(bool printToConsole);

	std::vector<TransientSimulationState> runTransient(Config config);

	void setGraphVars();

	void addNodeVoltages();
	void addComponentVars();

private:

	LinearSolver solver;	
	std::vector<std::unique_ptr<SimulationComponent>>& simComponents;	

	int getMNAIndex(const int eNode);


};