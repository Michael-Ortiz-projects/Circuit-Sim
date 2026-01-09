#include "WireHandler.h"

WireHandler::WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components)
	: circuit(Circuit), schematicComponents(components) { }

void WireHandler::onMousePress(const sf::Vector2f& worldPos) {
    switch (wireState) {
    case WireState::Null:
        if (interaction.hasLead() && circuit.leadIsEmpty(interaction.connection)) {
            std::cout << "Beginning Wire Creation\n";
            beginWireFromConnection(interaction.connection);
            activeWire->updatePreview(worldPos);
        }

        else if (interaction.hasWireNode()) {
            std::cout << "Beginning Drag on node " << interaction.wire_node.nodeID << std::endl;
            beginNodeDrag(interaction.wire_node);
            Node& clickedNode = activeWire->getNode(interaction.wire_node.nodeID);

            clickedNode.selected = true;
            for (int neighborID : clickedNode.neighbors) {
                activeWire->getNode(neighborID).selected = true;
            }
        }
        else if (wireSegment.valid) {
            activeWire = circuit.getWire(wireSegment.wireID);
            activeWire->getNode(wireSegment.segment.nodeA).selected = true;
            activeWire->getNode(wireSegment.segment.nodeB).selected = true;
            wireState = WireState::Selecting;
        }
        break;
    
    case WireState::Selecting:
        if (interaction.invalid() && !wireSegment.valid) {
            activeWire->unselect();
            wireState = WireState::Null;
        }
        else if (interaction.hasWireNode()) {
            std::cout << "Beginning Drag on node " << interaction.wire_node.nodeID << std::endl;
            beginNodeDrag(interaction.wire_node);
            Node& clickedNode = activeWire->getNode(interaction.wire_node.nodeID);

            clickedNode.selected = true;
            for (int neighborID : clickedNode.neighbors) {
                activeWire->getNode(neighborID).selected = true;
            }
        }
        else if (wireSegment.valid) {
            activeWire = circuit.getWire(wireSegment.wireID);
            activeWire->getNode(wireSegment.segment.nodeA).selected = true;
            activeWire->getNode(wireSegment.segment.nodeB).selected = true;
        }
        break;

    case WireState::Creating:
        if (activeWire && snapPositionToGrid(worldPos) != activeWire->getGraph().at(activeWire->getStemNode()).position) {
            if (interaction.hasLead()) {
                finishWireAtConnection(interaction.connection);
                std::cout << "Ending Wire Creation\n";
            }

            else if (interaction.hasWireNode()) {
                std::cout << "Finishing Wire At Node\n";
                finishWireAtNode(interaction.wire_node);
                std::cout << "Finished Wire At Node\n";
            }

            else if (wireSegment.valid) {
                std::cout << "Snapped position:";
                Debug::printVector2f(wireSegment.segment.snappedPosition);
                std::cout << "Finishing Wire At Segment\n";
                finishWireAtSegment(wireSegment);
                std::cout << "Finished Wire At Segment\n";
            }

            else {
                std::cout << "Appending Node\n";
                activeWire->commitPreview();
                activeWire->updatePreview(worldPos);
            }
            break;
        }
        break;
    default:
        break;
    }
}

void WireHandler::onMouseMove(const sf::Vector2f& worldPos) {

    switch (wireState) {
    case WireState::Creating:
        if (activeWire) {
            activeWire->updatePreview(worldPos);
        }
        break;

    case WireState::DraggingNode:
        if (activeWire) {

            WireMoveResult moveResult = activeWire->moveNode(interaction.wire_node.nodeID, worldPos);

            switch (moveResult) {
            

            case WireMoveResult::NodeRemoved:
                interaction.wire_node = { -1, -1 };
                wireState = WireState::Null;
                break;

            case WireMoveResult::Valid:
                wireState = WireState::DraggingNode;
                activeWire->mergeCoincidentNodes(interaction.wire_node.nodeID);
                break;

            case WireMoveResult::Invalid:
                std::cout << "Invalid Wire Move\n";
                interaction.wire_node = { -1, -1 };
                wireState = WireState::Null;
            }
        }
        break;

    default:
        break;
    }
}

void WireHandler::onMouseRelease(const sf::Vector2f& worldPos) {
    interaction.connection = { -1, Lead::Null };

    if (wireState == WireState::DraggingNode) {
        interaction.wire_node = { -1, -1 };
        wireState = WireState::Selecting;
    }
    
}

bool WireHandler::shouldRelease() const {
	return wireState == WireState::Null;
}

sf::Vector2f WireHandler::snapPositionToGrid(const sf::Vector2f& position) {
	return {
		std::round(position.x / gridSize) * gridSize,
		std::round(position.y / gridSize) * gridSize
	};
}

sf::Vector2f WireHandler::positionOfConnection(ElectricalConnection& connection) {
	int id = connection.componentID;
	auto it = std::find_if(schematicComponents.begin(), schematicComponents.end(),
		[id](const SchematicComponent& c) {
			return c.componentID == id;
		}
	);

	if (it == schematicComponents.end()) {
		return { -1, -1};
	}

	switch (connection.lead) {
	case Lead::A: return it->getLeadPositionA();
	case Lead::B: return it->getLeadPositionB();
	default:      return { -1.f, -1.f };
	}
}

void WireHandler::beginWireFromConnection(ElectricalConnection& connection) {
    if (wireState != WireState::Null) return;

    activeElectricalNodeID = circuit.createElectricalNode();
    sf::Vector2f pos = positionOfConnection(connection);
    int wireID = circuit.createWire(pos);

    circuit.wireIDToElectricalNode[wireID] = activeElectricalNodeID;

    circuit.addConnectionToNode(activeElectricalNodeID, connection);
    circuit.updateComponentLead(wireID, 0, activeElectricalNodeID, connection);

    

    activeWire = circuit.getWire(wireID);
    activeWire->selected = true;


    wireState = WireState::Creating;
    std::cout << "WireState = Creating\n\n";
}

void WireHandler::finishWireAtConnection(ElectricalConnection& end) {
    if (wireState != WireState::Creating || !activeWire) return;

    int finalNodeID = activeWire->commitPreview();

    circuit.addConnectionToNode(activeWire->ID, end);
    circuit.updateComponentLead(activeWire->ID, finalNodeID, activeElectricalNodeID, end);
    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
}

void WireHandler::finishWireAtNode(WireNodeReference wire_node) {
    if (wireState != WireState::Creating || !activeWire) return;
    bool sameWire = activeWire->ID == wire_node.wireID;
    int finalNodeID = activeWire->commitPreview();
    Wire& primaryWire = *circuit.getWire(wire_node.wireID);

    if (sameWire) {
        for (int activeNodeNeighborID : activeWire->getNode(finalNodeID).neighbors) {
            activeWire->connectNodes(activeNodeNeighborID, wire_node.nodeID);
        }
        activeWire->removeNode(finalNodeID);
    }

    else {
        mergeActiveWireIntoPrimary(primaryWire, *activeWire);
        circuit.mergeElectricalNodes(circuit.wireIDToElectricalNode.at(primaryWire.ID), activeElectricalNodeID);
        circuit.eraseWire(activeWire->ID);
    }
    
    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
}

void WireHandler::finishWireAtSegment(WireHit wireSegment) { // project segment kind of correct, its made for axis aligned segments
    if (wireState != WireState::Creating || !activeWire) return;
    bool sameWire = activeWire->ID == wireSegment.wireID;
    int finalNodeID = activeWire->commitPreview();
    Wire& primaryWire = *circuit.getWire(wireSegment.wireID);

    if (sameWire) {
        primaryWire.disconnectNodes(wireSegment.segment.nodeA, wireSegment.segment.nodeB);
        primaryWire.connectNodes(wireSegment.segment.nodeA, finalNodeID);
        primaryWire.connectNodes(wireSegment.segment.nodeB, finalNodeID);
    }

    else {
        int inserted = primaryWire.createNode(activeWire->getNode(finalNodeID).position);
        primaryWire.disconnectNodes(wireSegment.segment.nodeA, wireSegment.segment.nodeB);
        primaryWire.connectNodes(wireSegment.segment.nodeA, inserted);
        primaryWire.connectNodes(wireSegment.segment.nodeB, inserted);
        mergeActiveWireIntoPrimary(primaryWire, *activeWire);
        circuit.mergeElectricalNodes(circuit.wireIDToElectricalNode.at(primaryWire.ID), activeElectricalNodeID);
        circuit.eraseWire(activeWire->ID);
    }
    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
}

void WireHandler::mergeActiveWireIntoPrimary(Wire& primaryWire, Wire& activeWire) {
    auto& primaryGraph = primaryWire.getGraph();
    auto& activeGraph = activeWire.getGraph();

    // primary position to nodeID lookup
    std::map<sf::Vector2f, int, Vector2fCompare> primaryPositionToNode;
    for (auto& [id, node] : primaryGraph) {
        primaryPositionToNode[node.position] = id;
    }

    // remap active IDs to primary IDs
    std::unordered_map<int, int> idRemap;
    idRemap.reserve(activeGraph.size());


    for (auto& [activeID, node] : activeGraph) {
        auto it = primaryPositionToNode.find(node.position);

        if (it != primaryPositionToNode.end()) {
            idRemap[activeID] = it->second;
        }

        else {
            int newID = primaryWire.createNode(node.position);
            idRemap[activeID] = newID;
            primaryPositionToNode[node.position] = newID;
        }
    }

    // reconnect neighbors
    for (auto& [activeID, activeNode] : activeGraph) {
        int primaryID = idRemap.at(activeID);

        for (int activeNeighborID : activeNode.neighbors) {

            auto it = idRemap.find(activeNeighborID);
            if (it == idRemap.end()) continue;
            int primaryNeighborID = it->second;

            if (primaryID == primaryNeighborID) continue;

            auto& primaryNode = primaryGraph.at(primaryID);

            if (!primaryNode.neighbors.contains(primaryNeighborID)) {
                primaryWire.connectNodes(primaryID, primaryNeighborID);
            }
        }
    }

    // update anchors
    for (auto& [activeID, activeNode] : activeGraph) {
        if (!activeNode.isAnchor) continue;

        int primaryID = idRemap.at(activeID);
        primaryGraph.at(primaryID).isAnchor = true;
    }
}


void WireHandler::beginNodeDrag(WireNodeReference& ref) {
    activeWire = circuit.getWire(ref.wireID);
    wireState = WireState::DraggingNode;
}

void WireHandler::setInteractionContext(WireInteraction context) {
    interaction = context;
}

void WireHandler::setSegmentContext(WireHit context) {
    wireSegment = context;
}