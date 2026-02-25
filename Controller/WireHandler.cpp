#include "WireHandler.h"

WireHandler::WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components)
	: circuit(Circuit), schematicComponents(components) { }

void WireHandler::onMousePress(const sf::Vector2f& worldPos) {

    std::cout << "this ran\n";
    switch (wireState) {

    case WireState::Null:
        std::cout << "this ran again\n";
        std::cout << "Terminal is empty == " << circuit.terminalIsEmpty(hit.lead) << "hit.type == HitResult::Type::Lead = ";
        if (hit.type == HitResult::Type::Lead && circuit.terminalIsEmpty(hit.lead)) {

            beginWireFromConnection(hit.lead);
            std::cout << "beginWireFromConnection() ran\n";
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

        if (hit.type == HitResult::Type::None) {
            beginWireFromNull(worldPos);
            activeWire->updatePreview(worldPos);
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
    SchematicComponent* c = circuit.getSchematicComponent(connection.componentID);

    if (!c) return { -1, -1 };

    return c->getPosition() + c->schematicTerminals.at(connection.terminalID).offset;
}

void WireHandler::beginWireFromConnection(ElectricalConnection& connection) {
    if (wireState != WireState::Null) return;
    sf::Vector2f pos = positionOfConnection(connection);

    int wireID = circuit.createWire(pos);
    activeElectricalNodeID = wireID;

    circuit.addConnectionToElectricalNode(activeElectricalNodeID, connection);
    circuit.updateComponentTerminal({ wireID, 0 }, activeElectricalNodeID, connection);

    activeWire = circuit.getWire(wireID);
    activeWire->selected = true;

    wireState = WireState::Creating;
    std::cout << "WireState = Creating\n\n";
}

void WireHandler::beginWireFromNull(sf::Vector2f worldPos) {
    if (wireState != WireState::Null) return;

    sf::Vector2f snappedPosition = snapPositionToGrid(worldPos);
    int wireID = circuit.createWire(snappedPosition);
    activeElectricalNodeID = wireID;

    activeWire = circuit.getWire(wireID);
    activeWire->selected = true;
    wireState = WireState::Creating;
    std::cout << "Wirestate = Creating\n";
}
void WireHandler::editWireFromHangingNode(WireNodeReference reference) {
    wireState = WireState::Creating;
    activeWire->selected = true;
    activeWire->setStem(reference.nodeID);
    activeWire->updatePreview(activeWire->getNode(reference.nodeID).position);
}

void WireHandler::handleWireCreationClick(const sf::Vector2f& worldPos) {
    std::cout << "handling wire creation click\n";
    if (!activeWire) {
        std::cout << "No activeWire\n";
        return;
    }

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

    int primaryWireID = circuit.getNetlistComponent(end.componentID)->terminals.at(end.terminalID).electricalNode;
    int finalNodeID = activeWire->commitPreview();
    int wireID;

    if (primaryWireID == -1) {
        wireID = activeWire->ID;

        circuit.addConnectionToElectricalNode(wireID, end);
        circuit.updateComponentTerminal({ wireID, finalNodeID }, activeElectricalNodeID, end);

        activeWire->selected = false;
        activeWire = nullptr;
        wireState = WireState::Null;
        std::cout << "WireState = Null in finishWireAtConnection\n\n";
        return;
    }

    else {
        Wire& primaryWire = *circuit.getWire(primaryWireID);
        circuit.addConnectionToElectricalNode(primaryWireID, end);
        mergeActiveWireIntoPrimary(primaryWire, *activeWire);
        circuit.absorbElectricalNode(primaryWireID, activeElectricalNodeID);
        circuit.eraseWire(activeWire->ID, true);
        activeWire = nullptr;
        wireState = WireState::Null;
        std::cout << "WireState = Null in finishWireAtConnection\n\n";
        return;
    }
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
        int terminalID = c.terminalID;
        int componentID = c.componentID;
        NetlistComponent* ncomp = circuit.getNetlistComponent(componentID);
        if (!ncomp->terminalValid(terminalID)) {
            std::cout << "Terminal " << terminalID << "Not Valid\n";
            return;
        }
        SchematicComponent* scomp = circuit.getSchematicComponent(componentID);
        int terminalNodeReference = scomp->schematicTerminals.at(terminalID).wireNodeReference.nodeID;


        circuit.updateComponentTerminal({ primaryWire.ID, idRemap[terminalNodeReference] }, activeWire.ID, c);
    }
}

void WireHandler::setHitResult(const HitResult& h) {
    hit = h;
}