#include "DeleteHandler.h"
#include <algorithm>


DeleteHandler::DeleteHandler(Circuit& c, Selection& s)
	: circuit(c), selection(s) { }

void DeleteHandler::onKeyPress(const sf::Event::KeyEvent& event) {
	if (event.code != sf::Keyboard::Delete) return;

	deleteSelection();
}

bool DeleteHandler::shouldRelease() const {
	return release;
}

void DeleteHandler::deleteSelection() {
	Selection simplified_selection = simplifySelection();
	
	deleteComponents(simplified_selection.componentIDs);
	deleteSegments(simplified_selection.segments);	
	deleteNodes(simplified_selection.nodes);	
	deleteWires(simplified_selection.wireIDs);
	updateWires();
}

void DeleteHandler::deleteComponents(std::unordered_set<int> component_IDs) {
	for (const auto ID : component_IDs) {
		Component& c = *circuit.getComponent(ID);
		ElectricalConnection connection_A(ID, Lead::A);
		ElectricalConnection connection_B(ID, Lead::B);

		if (c.nodeA != -1) {
			circuit.removeConnectionFromElectricalNode(ID, connection_A);
			circuit.getWire(c.A_WireNodeReference.wireID)->getNode(c.A_WireNodeReference.nodeID).isAnchor = false;
		}

		if (c.nodeB != -1) {
			circuit.removeConnectionFromElectricalNode(ID, connection_B);
			circuit.getWire(c.B_WireNodeReference.wireID)->getNode(c.B_WireNodeReference.nodeID).isAnchor = false;
		}
		circuit.removeComponent(ID);
	}
}

void DeleteHandler::deleteSegments(std::unordered_set<WireSegmentReference> segments) {
	for (const WireSegmentReference segment : segments) {
		modifiedWires.insert(segment.wireID);

		circuit.getWire(segment.wireID)->disconnectNodes(segment.nodeA, segment.nodeB);
	}
}

void DeleteHandler::deleteNodes(std::unordered_set<WireNodeReference> nodes) {
	for (const WireNodeReference wire_node : nodes) {
		modifiedWires.insert(wire_node.wireID);
		circuit.getWire(wire_node.wireID)->removeNode(wire_node.nodeID);
	}
}

void DeleteHandler::deleteWires(std::unordered_set<int> wire_IDs) {
	for (const int wire_ID : wire_IDs) {
		bool updateComponents = true;
		circuit.eraseWire(wire_ID, updateComponents);
	}
}

Selection DeleteHandler::simplifySelection() { // returns the minimum amount of elements to delete
	Selection simplified_selection = selection;

	// bucket nodes by wire
	std::unordered_map<int, std::unordered_set<int>> nodesByWire;
	for (const auto& n : simplified_selection.nodes) {
		if (!n.isValid()) continue;
		nodesByWire[n.wireID].insert(n.nodeID);
	}

	// remove segments whose both endpoints are selected
	std::erase_if(simplified_selection.segments, [&](const WireSegmentReference& s) {
		auto it = nodesByWire.find(s.wireID);
		if (it == nodesByWire.end()) return false;
		const auto& selectedNodes = it->second;
		return selectedNodes.contains(s.nodeA) && selectedNodes.contains(s.nodeB);
		});

	// promote fully selected wires
	for (const auto& [wireID, nodeSet] : nodesByWire) {
		Wire& wire = *circuit.getWire(wireID);
		if (nodeSet.size() == wire.getGraph().size()) {

			simplified_selection.wireIDs.insert(wireID);

			// remove all nodes and segments for this wire
			std::erase_if(simplified_selection.nodes, [&](const WireNodeReference& n) {
				return n.wireID == wireID;
				});
			std::erase_if(simplified_selection.segments, [&](const WireSegmentReference& s) {
				return s.wireID == wireID;
				});
		}
	}

	return simplified_selection;
}

void DeleteHandler::updateWires() {
	std::cout << " update Wires started\n";

	for (int wireID : modifiedWires) {
		Wire* oldWire = circuit.getWire(wireID);
		if (!oldWire) continue;

		auto sections = findWireSections(*oldWire);
		if (sections.size() <= 1) {
			std::cout << "only 1 section\n";
			continue;
		}

		//capture components attached to this section of the wire
		std::vector < ComponentAttachment > attachments;
		std::cout << "this ran\n";
		const ElectricalNode* eNode = circuit.getElectricalNode(wireID);
		
		for (const ElectricalConnection& connection : eNode->connections) {
			const Component* comp = circuit.getComponent(connection.componentID);
			if (!comp) continue;

			if (connection.lead == Lead::A && comp->A_WireNodeReference.wireID == wireID) {
				attachments.push_back({ connection.componentID, Lead::A, comp->A_WireNodeReference.nodeID });
			}

			if (connection.lead == Lead::B && comp->B_WireNodeReference.wireID == wireID) {
				attachments.push_back({ connection.componentID, Lead::B, comp->B_WireNodeReference.nodeID });
			}
		}
		std::map<int, Node> oldGraph = oldWire->getGraph();
		circuit.eraseWire(wireID, true);

		for (auto& section : sections) {
			if (section.size() < 2) continue;
			rebuildWireFromSection(section, attachments, oldGraph);
			std::cout << "Rebuilding Wire from Section\n";
		}
	}

}

void DeleteHandler::dfsSection(int start, const std::map<int, Node>& graph, std::unordered_set<int>& visited, WireSection& out) {

	std::stack<int> stack;
	stack.push(start);
	visited.insert(start);

	while (!stack.empty()) {
		int id = stack.top();
		stack.pop();

		out.push_back(id);

		const Node& n = graph.at(id);
		for (int neigh : n.neighbors) {
			if (!visited.contains(neigh)) {
				visited.insert(neigh);
				stack.push(neigh);
			}
		}
	}
}

std::vector<WireSection> DeleteHandler::findWireSections(Wire& wire) {
	std::vector<WireSection> sections;
	std::unordered_set<int> visited;

	const auto& graph = wire.getGraph();

	for (const auto& [nodeID, node] : graph) {
		if (visited.contains(nodeID)) continue;

		WireSection section;
		dfsSection(nodeID, graph, visited, section);
		sections.push_back(std::move(section));
	}

	return sections;
}

void DeleteHandler::rebuildWireFromSection(WireSection& section, std::vector<ComponentAttachment>& attachments, std::map<int, Node> graph) {
	const Node& firstOldNode = graph.at(section[0]);
	int newWireID = circuit.createWire(firstOldNode.position);
	int newElectricalNodeId = newWireID;

	std::unordered_map<int, int> idMap;
	idMap[section[0]] = 0;


	Wire& newWire = *circuit.getWire(newWireID);


	// create remaining nodes 
	for (size_t i = 1; i < section.size(); ++i) {
		int oldID = section[i];
		const Node& oldNode = graph.at(oldID);
		int newID = newWire.createNode(oldNode.position);
		idMap[oldID] = newID;
	}


	
	// recreate connections
	for (int oldID : section) {
		const Node& oldNode = graph.at(oldID);
		int newID = idMap[oldID];

		for (int oldNeighbor : oldNode.neighbors) {
			// only connect inside this section
			if (!idMap.contains(oldNeighbor)) continue;

			int newNeighbor = idMap[oldNeighbor];
			if (newID < newNeighbor) { // avoid double connect
				newWire.connectNodes(newID, newNeighbor);
			}
		}
	}

	// update leads and eNodes
	for (const auto& a : attachments) {
		if (!idMap.contains(a.oldNodeID)) continue;
		Component* comp = circuit.getComponent(a.componentID);

		//detach component from previous eNode
		circuit.updateComponentLead(-1, -1, -1, { a.componentID, a.lead });

		//update WireNode Ref
		WireNodeReference& reference = (a.lead == Lead::A) ? comp->A_WireNodeReference : comp->B_WireNodeReference;

		reference.wireID = newWireID;
		reference.nodeID = idMap[a.oldNodeID];

		newWire.getNode(idMap[a.oldNodeID]).isAnchor = true;
		circuit.getElectricalNode(newWireID)->connections.push_back({ a.componentID, a.lead });
	}	
}
// order for deletion components -> wires -> segments -> nodes -> then cleanup wires and check for splits and all that
// delete all your elements in the selection, then do dfs to check for a split then split wires by doing dfs on every node in each wire's remaining graph