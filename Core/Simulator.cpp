#include "Simulator.h"

Simulator::Simulator(std::vector<std::unique_ptr<SimulationComponent>>& simComps)
	: simComponents(simComps) { }

bool Simulator::setSystem(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes) {
	std::cout << "setSystem() running\n";
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
		for (ExtraVarInfo info : c->getExtraVarInfo()) {
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
	std::vector<int> nodes;
	for (auto T : netlistComp.terminals) {
		std::cout << T.electricalNode << " ";
		nodes.push_back(getMNAIndex(T.electricalNode));
	}
	std::cout << "\n";
	

	std::cout << "debug1\n";
	ExpressionEvaluator f(netlistComp.expressionString);
	
	switch (netlistComp.type) {
	case ComponentType::Resistor:
		return std::make_unique<SimResistor>(nodes[0], nodes[1], netlistComp.value, netlistComp.id, netlistComp.label);

	case ComponentType::CurrentSource:
		return std::make_unique<SimCurrentSource>(nodes[0], nodes[1], netlistComp.value, netlistComp.id, netlistComp.label);
	
	case ComponentType::VCCS:
		return std::make_unique<VoltageControlledCurrentSource>(nodes[0], nodes[1], nodes[2], nodes[3], netlistComp.value, netlistComp.id, netlistComp.label);
	
	case ComponentType::VCVS:
		return std::make_unique<VoltageControlledVoltageSource>(nodes[0], nodes[1], nodes[2], nodes[3], netlistComp.value, netlistComp.id, netlistComp.label);
	
	case ComponentType::CCCS:
		return std::make_unique<CurrentControlledCurrentSource>(nodes[0], nodes[1], nodes[2], nodes[3], netlistComp.value, netlistComp.id, netlistComp.label);
	
	case ComponentType::CCVS:
		return std::make_unique<CurrentControlledVoltageSource>(nodes[0], nodes[1], nodes[2], nodes[3], netlistComp.value, netlistComp.id, netlistComp.label);
	
	case ComponentType::VoltageSource:
		return std::make_unique<SimVoltageSource>(nodes[0], nodes[1], netlistComp.value, netlistComp.id, netlistComp.label);
	
	case ComponentType::Capacitor:
		return std::make_unique<SimCapacitor>(nodes[0], nodes[1], netlistComp.value, netlistComp.id, netlistComp.label);

	case ComponentType::Inductor:
		return std::make_unique<SimInductor>(nodes[0], nodes[1], netlistComp.value, netlistComp.id, netlistComp.label);
	
	case ComponentType::ACCurrentSource:
		return std::make_unique<SimACCurrentSource>(nodes[0], nodes[1], f, netlistComp.id, netlistComp.label);

	case ComponentType::ACVoltageSource:
		return std::make_unique<SimACVoltageSource>(nodes[0], nodes[1], f, netlistComp.id, netlistComp.label);

	case ComponentType::Ground:
		return std::make_unique<SimGround>(nodes[0], netlistComp.id);
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
	// to make this faster, make 2 matrices, static and dynamic, resistors and indep. sources dont change so stamping them every step is dumb
	// the inductor search loop could be done once at the start of the function then stored to reduce the amount of lookups
	// i can use LU factorization to make solving more efficient
	// fixing timestep is a must

	// ==========================
	// INITIALIZE STEPS & RESULTS	
	// ==========================
	
	size_t numSteps = static_cast<size_t>(std::ceil((config.tEnd - config.tStart) / config.timeStep)) + 1;
	std::cout << "Matrix Size: " <<  system.getA().size() << "\n";
	size_t step = 0;
	std::vector<TransientSimulationState> results(numSteps);
	Eigen::VectorXd previousX = system.getx();
	results[0].time = config.tStart;
	results[0].deltaT = 0.0;
	results[0].resultsVector = previousX;
	results[0].previousResultsVector = previousX;

	std::cout << "CLASSIFYING COMPONENTS\n";

	// ==========================
	// CLASSIFY COMPONENTS
	// ==========================

	std::vector<SimulationComponent*> staticComponents;
	std::vector<SimulationComponent*> dynamicComponents;
	std::vector<SimInductor*> inductors;

	for (auto& comp : simComponents) {
		if (comp->isStatic())
			staticComponents.push_back(comp.get());
		else
			dynamicComponents.push_back(comp.get());

		if (auto ind = dynamic_cast<SimInductor*>(comp.get()))
			inductors.push_back(ind);
	}

	std::cout << "Static components: " << staticComponents.size() << "\n";
	std::cout << "Dynamic components: " << dynamicComponents.size() << "\n";
	std::cout << "Inductors Size: " << inductors.size() << "\n";

	std::cout << "SETTING STATIC MATRICES\n";

	// ==========================
	// SET STATIC MATRICES
	// ==========================
	for (auto* static_comp : staticComponents) {
		static_comp->stampStatic(SimulationType::Transient, system);
	}
	

	std::cout << "timestep Solve starting\n";
	double dt = config.timeStep;

	for (size_t step = 1; step < numSteps; ++step) {

		double t = config.tStart + step * config.timeStep;		


		system.resetStatic();


		for (auto* comp : dynamicComponents) {
			comp->stampDynamic(SimulationType::Transient, system, dt, t);
		}

		//std::cout << "A = \n" << system.getA() << "\n\n b = \n" << system.getb() << "\n\n";

		solver.solve(system);
		

		// update inductor currents for the next step
		for (auto& ind : inductors) {
			int idx = ind->getExtraVarInfo()[0].index;  // extraVarIndex in MNASystem
			double i_new = system.getx()[idx];          // solved current
			ind->setCurrent(i_new);
		}

		results[step].time = t;
		results[step].deltaT = dt;
		results[step].resultsVector = system.getx();
		results[step].previousResultsVector = results[step - 1].resultsVector;

	}
	
	transientResults = results;
	std::cout << "finished simulation\n";
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