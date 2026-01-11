#include "WireHandler.h"

WireHandler::WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components)
	: circuit(Circuit), schematicComponents(components) { }

void WireHandler::onMousePress(const sf::Vector2f& worldPos) {
    switch (wireState) {

    case WireState::Null:
    case WireState::Selecting:

        if (hit.type == HitResult::Type::Lead &&
            circuit.leadIsEmpty(hit.lead)) {

            beginWireFromConnection(hit.lead);
            wireState = WireState::Creating;
            activeWire->updatePreview(worldPos);
            return;
        }

        if (hit.type == HitResult::Type::WireNode) {
            beginNodeDrag(hit.wireNode);
            //selectNodeWithNeighbors(hit.wireNode.nodeID);
            wireState = WireState::DraggingNode;
            return;
        }

        if (hit.type == HitResult::Type::WireSegment) {
            activeWire = circuit.getWire(hit.wireSegment.wireID);
            activeWire->unselect();
            activeWire->getNode(hit.wireSegment.segment.nodeA).selected = true;
            activeWire->getNode(hit.wireSegment.segment.nodeB).selected = true;

            wireState = WireState::Selecting;
            return;
        }

        // Clicked empty space
        if (wireState == WireState::Selecting) {
            activeWire->unselect();
            wireState = WireState::Null;
            std::cout << "WireState = Null in onMousePress()\n\n";

        }
        return;

    case WireState::Creating:
        handleWireCreationClick(worldPos);
        return;

    default:
        return;
    }
}

void WireHandler::onMouseMove(const sf::Vector2f& worldPos) {

    if (!activeWire)
        return;

    switch (wireState) {

    case WireState::Creating:
        // Live preview follows the mouse
        activeWire->updatePreview(worldPos);
        break;

    case WireState::DraggingNode: {
        WireMoveResult result =
            activeWire->moveNode(hit.wireNode.nodeID, worldPos);

        if (result == WireMoveResult::Valid) {
            activeWire->mergeCoincidentNodes(hit.wireNode.nodeID);
            break;
        }

        // Drag failed -> cancel interaction
        wireState = WireState::Null;
        std::cout << "WireState = Null in onMouseMove\n\n";

        break;
    }

    default:
        break;
    }
}

void WireHandler::onMouseRelease(const sf::Vector2f& worldPos) {
    switch(wireState) {

    case WireState::DraggingNode:
        wireState = WireState::Selecting;
        break;

    default:
        break;
    }
}

bool WireHandler::shouldRelease() const {
    bool null = wireState == WireState::Null;
    std::cout << "shouldRelease() says WireState == Null " << null << std::endl;
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

void WireHandler::handleWireCreationClick(const sf::Vector2f& worldPos) {
    std::cout << "handling wire creation click\n";
    if (!activeWire)
        return;

    if (hit.type == HitResult::Type::Lead) {
        finishWireAtConnection(hit.lead);
        wireState = WireState::Null;
        std::cout << "WireState = Null in handleWireCreationClick\n\n";

        return;
    }

    if (hit.type == HitResult::Type::WireNode) {
        finishWireAtNode(hit.wireNode);
        wireState = WireState::Null;
        std::cout << "WireState = Null in handleWireCreationClick\n\n";

        return;
    }

    if (hit.type == HitResult::Type::WireSegment) {
        finishWireAtSegment(hit.wireSegment);
        wireState = WireState::Null;
        std::cout << "WireState = Null in handleWireCreationClick\n\n";

        return;
    }

    sf::Vector2f snapped = snapPositionToGrid(worldPos);
    const sf::Vector2f& lastPos =
        activeWire->getNode(activeWire->getStemNode()).position;

    if (snapped == lastPos)
        return;

    activeWire->commitPreview();
    activeWire->updatePreview(worldPos);

}

void WireHandler::finishWireAtConnection(ElectricalConnection& end) {
    if (wireState != WireState::Creating || !activeWire) return;

    int finalNodeID = activeWire->commitPreview();

    circuit.addConnectionToNode(activeWire->ID, end);
    circuit.updateComponentLead(activeWire->ID, finalNodeID, activeElectricalNodeID, end);
    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
    std::cout << "WireState = Null in finishWireAtConnection\n\n";

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
    std::cout << "WireState = Null in finishWireAtNode\n\n";

}

void WireHandler::finishWireAtSegment(WireHit wireSegment) {
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
    std::cout << "WireState = Null in finishWireAtSegment\n\n";

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

void WireHandler::setHitResult(const HitResult& h) {
    hit = h;
}

void WireHandler::selectNodeWithNeighbors(int nodeID) {
    Node& node = activeWire->getNode(nodeID);
    node.selected = true;

    for (int n : node.neighbors)
        activeWire->getNode(n).selected = true;
}