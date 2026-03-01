#include "Simulator.h"

Simulator::Simulator(std::vector<std::unique_ptr<SimulationComponent>>& simComps)
	: simComponents(simComps) { }

bool Simulator::setSystem(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes) {
	int index = buildMNAMap(netlist, eNodes);
	if (index == -1) {
		std::cout << "No Ground Nodes Located in MNA System\n";
		return false;
	}
	simComponents.clear();

	for (const auto& C : netlist) {
		auto simulationComp = buildSimulationComponent(C);
		if (simulationComp) simComponents.push_back(std::move(simulationComp));
	}

	std::cout << "simulation components built\n\n";
	system.extraVars.clear();
	int extraIndex = index++;
	for (auto& c : simComponents) {
		c->setExtraVariableIndex(extraIndex);
		for (auto info : c->getExtraVarInfo()) {
			info.index = extraIndex;
			system.extraVars.push_back(info);
		}
		extraIndex += c->extraVariables();
	}
	std::cout << "setting graph variables\n\n";

	setGraphVars();
	std::cout << "graph variables set\n\n";

	system.setSystem(index, extraIndex - index);


	return true;
}

std::unique_ptr<SimulationComponent> Simulator::buildSimulationComponent(const NetlistComponent& netlistComp) {
	std::cout << "building Simulation component from component " << netlistComp.id;
	std::cout << ", nodes are ";
	for (auto T : netlistComp.terminals) {
		std::cout << T.electricalNode << " ";
	}
	std::cout << "\n";
	int n1, n2;
	switch (netlistComp.type) {
	case ComponentType::Resistor:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		n2 = getMNAIndex(netlistComp.terminals[1].electricalNode);
		return std::make_unique<SimResistor>(n1, n2, netlistComp.value, netlistComp.id, netlistComp.label);
	case ComponentType::CurrentSource:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		n2 = getMNAIndex(netlistComp.terminals[1].electricalNode);
		return std::make_unique<SimCurrentSource>(n1, n2, netlistComp.value, netlistComp.id, netlistComp.label);
	case ComponentType::VoltageSource:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		n2 = getMNAIndex(netlistComp.terminals[1].electricalNode);
		return std::make_unique<SimVoltageSource>(n1, n2, netlistComp.value, netlistComp.id, netlistComp.label);
	case ComponentType::Capacitor:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		n2 = getMNAIndex(netlistComp.terminals[1].electricalNode);
		return std::make_unique<SimCapacitor>(n1, n2, netlistComp.value, netlistComp.id, netlistComp.label);
	case ComponentType::Inductor:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		n2 = getMNAIndex(netlistComp.terminals[1].electricalNode);
		return std::make_unique<SimInductor>(n1, n2, netlistComp.value, netlistComp.id, netlistComp.label);
	case ComponentType::Ground:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		return std::make_unique<SimGround>(n1, netlistComp.id);
	}
}

int Simulator::buildMNAMap(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes) {
	std::cout << "buildMNAMap() running\n";
	eNodeToMNA.clear();
	std::set<int> nonGroundNodes;
	std::set<int> groundNodes;
	std::set<ElectricalConnection> groundConnections;

	for (NetlistComponent N : netlist) {
		if (N.type == ComponentType::Ground) {
			for (NetlistTerminal T : N.terminals) {
				ElectricalConnection conn(N.id, T.terminalID);
				groundConnections.insert(conn);
			}
		}
	}
	
	for (const auto& [ID, eNode] : eNodes) {
		for (ElectricalConnection C : eNode.connections) {
			if (groundConnections.contains(C)) groundNodes.insert(eNode.id);
		}
	}
	if (groundNodes.empty()) return -1;
	

	for (const auto& [ID, eNode] : eNodes) {
		if (!groundNodes.contains(ID) && !eNode.connections.empty()) nonGroundNodes.insert(ID);
	}
	int index = 1;
	for (int n : nonGroundNodes) {
		eNodeToMNA[n] = index++;
	}
	for (int n : groundNodes) {
		eNodeToMNA[n] = 0;
	}

	std::cout << "---- eNodeToMNA MAP----\n";
	for (const auto& [eNodeID, mnaIndex] : eNodeToMNA) {
		std::cout << "   [" << eNodeID
			<< " -> " << mnaIndex << "]\n";
	}
	std::cout << "Largest index : " << index << "\n";
	std::cout << "--------------------------\n";
	return index; //returns largest MNA index

}

int Simulator::getMNAIndex(const int eNode) {
	auto it = eNodeToMNA.find(eNode);
	if (it == eNodeToMNA.end()) {
		std::cout << "eNode ID: " + std::to_string(eNode) + " not found in eNodeToMNA map\n";
	}
	return it->second;
}

bool Simulator::runDC(bool printToConsole) {

	/*temporary solving code to just generate the matrices to print to console*/
	if (printToConsole) {
		system.printA();
		system.printb();
	}
	

	for (auto& C : simComponents) {
		std::cout << "Stamping Component: " << C->getID() << "\n";
		C->stamp(SimulationType::DC, system);
	}

	if (printToConsole) {
		system.printA();
		system.printb();
		system.printx();
	}
	solver.solve(system);

	return true;
}

std::vector<TransientSimulationState> Simulator::runTransient(Config config) {
	size_t numSteps = static_cast<size_t>(std::ceil((config.tEnd - config.tStart) / config.timeStep)) + 1;
	size_t step = 0;
	std::vector<TransientSimulationState> results(numSteps);
	std::vector<Eigen::VectorXd> resultVector(numSteps);
	std::vector<double> timeVector(numSteps);

	// -------------------------
	// 1) Run DC operating point
	// -------------------------
	//runDC(false);

	Eigen::VectorXd previousX = system.getx();

	// Step 0 = initial condition
	results[0].time = config.tStart;
	results[0].deltaT = 0.0;
	results[0].resultsVector = previousX;
	results[0].previousResultsVector = previousX;

	for (auto& v : resultVector)
		v = Eigen::VectorXd::Zero(system.getx().size());

	for (size_t step = 1; step < numSteps; ++step) {
		double t = config.tStart + step * config.timeStep;		
		double dt = (step == numSteps - 1 && config.tEnd - t > 0) ? config.tEnd - t : config.timeStep;
		if (step == numSteps - 1) t = config.timeStep * step + dt;
		if (dt <= 0.0) dt = 1e-12;


		for (auto& C : simComponents) C->stamp(SimulationType::Transient, system, dt);
		
		solver.solve(system);
		// update inductor currents for the next step
		for (auto& comp : simComponents) {
			if (auto ind = dynamic_cast<SimInductor*>(comp.get())) {
				int idx = ind->getExtraVarInfo()[0].index;  // extraVarIndex in MNASystem
				double i_new = system.getx()[idx];          // solved current
				ind->setCurrent(i_new);
			}
		}

		results[step].time = t;
		results[step].deltaT = dt;
		results[step].resultsVector = system.getx();
		results[step].previousResultsVector = results[step - 1].resultsVector;

		resultVector[step] = system.getx();
		system.setZero();
	}
	
	transientResults = results;
	return transientResults;
}

void Simulator::setGraphVars() {
	graphVariables.clear();

	addNodeVoltages();
	addComponentVars();
}

void Simulator::addNodeVoltages() {
	std::cout << "adding Node Voltages\n";
	for (auto [node, mnaIndex] : eNodeToMNA)
	{
		TransientGraphVariable var;
		var.label = "V(Node " + std::to_string(node) + ")";

		var.evaluator =
			[mnaIndex](const TransientSimulationState& state)
			{
				return state.resultsVector[mnaIndex];
			};

		graphVariables.push_back(var);
	}
}

void Simulator::addComponentVars() {
	std::cout << "adding component variables\n";
	for (auto& comp : simComponents) {
		std::cout << "comp ID: " << comp->getID() << "\n";
		comp->addGraphVariables(graphVariables, eNodeToMNA);
		std::cout << "added variable\n";
	}
}