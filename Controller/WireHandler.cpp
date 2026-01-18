#include "WireHandler.h"

WireHandler::WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components)
	: circuit(Circuit), schematicComponents(components) { }

void WireHandler::onMousePress(const sf::Vector2f& worldPos) {
    switch (wireState) {

    case WireState::Null:

        if (hit.type == HitResult::Type::Lead &&
            circuit.leadIsEmpty(hit.lead)) {

            beginWireFromConnection(hit.lead);
            wireState = WireState::Creating;
            activeWire->updatePreview(worldPos);
            return;
        }

        if (hit.type == HitResult::Type::WireNode) {
            activeWire = circuit.getWire(hit.wireNode.wireID);
            
            Node& hitNode = activeWire->getNode(hit.wireNode.nodeID);
            if (!hitNode.isAnchor && hitNode.neighbors.size() <= 1) {
                //node is hanging
                editWireFromHangingNode(hit.wireNode);
            }
            else {
                wireState = WireState::DraggingNode;
            }
            

            return;
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
        wireState = WireState::Null;
        break;

    default:
        break;
    }
}

bool WireHandler::shouldRelease() const {
    bool null = wireState == WireState::Null;
    //std::cout << "shouldRelease() says WireState == Null " << null << std::endl;
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
    sf::Vector2f pos = positionOfConnection(connection);

    int wireID = circuit.createWire(pos);
    activeElectricalNodeID = wireID;

    circuit.addConnectionToElectricalNode(activeElectricalNodeID, connection);
    circuit.updateComponentLead(wireID, 0, activeElectricalNodeID, connection);

    activeWire = circuit.getWire(wireID);
    activeWire->selected = true;

    wireState = WireState::Creating;
    std::cout << "WireState = Creating\n\n";
}

void WireHandler::editWireFromHangingNode(WireNodeReference reference) {
    wireState = WireState::Creating;
    activeWire->selected = true;
    activeWire->setStem(reference.nodeID);
    activeWire->updatePreview(activeWire->getNode(reference.nodeID).position);
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
    const sf::Vector2f& lastPos = activeWire->getNode(activeWire->getStemNode()).position;

    if (snapped == lastPos)
        return;

    activeWire->commitPreview();
    activeWire->updatePreview(worldPos);

}

void WireHandler::finishWireAtConnection(ElectricalConnection& end) {
   if (wireState != WireState::Creating || !activeWire) return;

    int finalNodeID = activeWire->commitPreview();
    int wireID = activeWire->ID;

    // Circuit owns electrical truth
    circuit.addConnectionToElectricalNode(wireID, end);
    circuit.updateComponentLead(wireID, finalNodeID, wireID, end);

    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
    std::cout << "WireState = Null in finishWireAtConnection\n\n";

}

void WireHandler::finishWireAtNode(WireNodeReference reference) {
    if (wireState != WireState::Creating || !activeWire) return;

    int finalNodeID = activeWire->commitPreview();
    int activeID = activeWire->ID;
    int primaryID = reference.wireID;
    std::cout << "got here1\n";
    if (activeID == primaryID) {
        // geometry only
        for (int n : activeWire->getNode(finalNodeID).neighbors) {
            activeWire->connectNodes(n, reference.nodeID);
        }
        activeWire->removeNode(finalNodeID);
    }
    else {
        std::cout << "got here 2\n";
        Wire& primary = *circuit.getWire(primaryID);
        std::cout << "got here 3\n";

        mergeActiveWireIntoPrimary(primary, *activeWire);
        std::cout << "got here 4\n";

        // move electrical connections
        circuit.absorbElectricalNode(primaryID, activeID);
        std::cout << "got here 5\n";

        circuit.eraseWire(activeID, true);
        std::cout << "got here 6\n";

    }

    activeWire = nullptr;
    wireState = WireState::Null;
    std::cout << "WireState = Null in finishWireAtNode\n\n";

}

void WireHandler::finishWireAtSegment(WireHit wireSegment) {
    if (wireState != WireState::Creating || !activeWire) return;

    int finalNodeID = activeWire->commitPreview();
    int activeID = activeWire->ID;
    int primaryID = wireSegment.wireID;

    Wire& primary = *circuit.getWire(primaryID);

    if (activeID == primaryID) {
        primary.disconnectNodes(wireSegment.segment.nodeA, wireSegment.segment.nodeB);
        primary.connectNodes(wireSegment.segment.nodeA, finalNodeID);
        primary.connectNodes(wireSegment.segment.nodeB, finalNodeID);
    }
    else {
        int inserted = primary.createNode(activeWire->getNode(finalNodeID).position);
        primary.disconnectNodes(wireSegment.segment.nodeA, wireSegment.segment.nodeB);
        primary.connectNodes(wireSegment.segment.nodeA, inserted);
        primary.connectNodes(wireSegment.segment.nodeB, inserted);

        mergeActiveWireIntoPrimary(primary, *activeWire);
        circuit.absorbElectricalNode(primaryID, activeID);
        circuit.eraseWire(activeID, true);
    }

    activeWire = nullptr;
    wireState = WireState::Null;
    std::cout << "WireState = Null in finishWireAtSegment\n\n";

}

void WireHandler::mergeActiveWireIntoPrimary(Wire& primaryWire, Wire& activeWire) {
    std::cout << "MergeActiveWireIntoPrimary staring\n";
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

    std::cout << "got here 1\n";
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
    std::cout << "got here 2\n";

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
    std::cout << "got here 3\n";

    // update anchors
    for (auto& [activeID, activeNode] : activeGraph) {
        if (!activeNode.isAnchor) continue;

        int primaryID = idRemap.at(activeID);
        primaryGraph.at(primaryID).isAnchor = true;
    }

    std::cout << "got here 4\n";

    // update component leads
    for (ElectricalConnection& c : circuit.getElectricalNode(activeWire.ID)->connections) {
        Component* component = circuit.getComponent(c.componentID);
        switch (c.lead) {
        case Lead::A:
            circuit.updateComponentLead(primaryWire.ID, idRemap[component->A_WireNodeReference.nodeID], activeWire.ID, c);
            break;

        case Lead::B:
            circuit.updateComponentLead(primaryWire.ID, idRemap[component->B_WireNodeReference.nodeID], activeWire.ID, c);
            break;
        }
    }
    std::cout << "got here 5\n";

}

void WireHandler::setHitResult(const HitResult& h) {
    hit = h;
}