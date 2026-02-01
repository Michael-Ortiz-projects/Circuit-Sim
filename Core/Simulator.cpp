#include "Simulator.h"

Simulator::Simulator() { }

void Simulator::setSystem(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes) {
	int index = buildMNAMap(netlist, eNodes);
	simComponents.clear();
	std::cout << "debug 1\n";

	for (const auto& C : netlist) {
		auto simulationComp = buildSimulationComponent(C);
		if (simulationComp) simComponents.push_back(std::move(simulationComp));
	}
	std::cout << "debug 2\n";
	//need to assign extra variable indexes for voltage sources and inductors and such

	int extraIndex = index;
	for (auto& c : simComponents) {
		c->setExtraVariableIndex(extraIndex);
		extraIndex += c->extraVariables();
	}
	std::cout << "debug 3\n";


	system.setSystem(index, extraIndex - index);
	std::cout << "debug 4\n";

	/*temporary solving code to just generate the matrices to print to console*/

	for (auto& C : simComponents) {
		std::cout << "Stamping Component: " << C->getID() << "\n";
		C->stamp(system);
	}
	std::cout << "debug 5\n";

	system.printA();
	system.printb();
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
		return std::make_unique<SimResistor>(n1, n2, netlistComp.value, netlistComp.id);
	case ComponentType::CurrentSource:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		n2 = getMNAIndex(netlistComp.terminals[1].electricalNode);
		return std::make_unique<SimCurrentSource>(n1, n2, netlistComp.value, netlistComp.id);
	case ComponentType::Ground:
		n1 = getMNAIndex(netlistComp.terminals[0].electricalNode);
		return std::make_unique<SimGround>(n1, netlistComp.id);
	}
}

int Simulator::buildMNAMap(const std::vector<NetlistComponent>& netlist, const std::unordered_map<int, ElectricalNode>& eNodes) {
	std::cout << "buildMNAMap() running\n";
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

bool Simulator::runDC() {
	return true;
}