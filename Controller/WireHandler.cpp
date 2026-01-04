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

            WireMoveResult moveResult = WireMoveResult::None;//activeWire->moveNode(interaction.wire_node.nodeID, worldPos, WireMoveIntent::Edit);
            switch (moveResult) {
            case WireMoveResult::NodeMerged:
                interaction.wire_node = { -1, -1 };
                wireState = WireState::Null;
                break;

            case WireMoveResult::NodeRemoved:
                interaction.wire_node = { -1, -1 };
                wireState = WireState::Null;
                break;

            case WireMoveResult::None:
                wireState = WireState::DraggingNode;
                break;
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
        wireState = WireState::Null;
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
    if (!wire_node.isValid() || !activeWire) return;

    activeWire->commitPreview();

    Wire& primaryWire = *circuit.getWire(wire_node.wireID);

    int targetNodeID = wire_node.nodeID;

    bool sameWire = (&primaryWire == activeWire);

    if (sameWire) {
        primaryWire.collapseNodeInto(targetNodeID, activeWire->getStemNode());
        

        activeWire->selected = false;
        wireState = WireState::Null;
        activeWire = &primaryWire;
        return;
    }

    auto& primaryGraph = primaryWire.getGraph();
    auto& activeGraph = activeWire->getGraph();

    // build a map of positions to nodes in primaryWire
    std::map<sf::Vector2f, int, Vector2fCompare> primaryPositionToNode;
    for (auto& [id, node] : primaryGraph)
        primaryPositionToNode[node.position] = id;

    std::unordered_map<int, int> idRemap;

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

    // connect neighbors 
    for (auto& [activeID, activeNode] : activeGraph) {
        int primaryID = idRemap[activeID];

        for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
            int activeNeighbor = activeNode.neighbors[d];
            if (activeNeighbor == -1) continue;

            auto it = idRemap.find(activeNeighbor);
            if (it == idRemap.end()) continue;

            int primaryNeighbor = it->second;
            if (primaryID == primaryNeighbor) continue;
            if (primaryGraph.find(primaryNeighbor) == primaryGraph.end()) continue;

            if (!primaryGraph.at(primaryID).neighbors.has(d))
                primaryWire.connectNodes(primaryID, primaryNeighbor);
        }
    }

    for (auto& [activeID, activeNode] : activeGraph) {
        if (activeNode.isAnchor) {
            int primaryID = idRemap[activeID];
            primaryWire.getGraph().at(primaryID).isAnchor = true;
        }
    }
    std::cout << "Debug 0\n";
    if (!sameWire) {
        int primaryElectricalNodeID = circuit.wireIDToElectricalNode.at(primaryWire.ID);
        circuit.mergeElectricalNodes(primaryElectricalNodeID, activeElectricalNodeID);

        activeWire->selected = false;
        int id = activeWire->ID;
        activeWire = nullptr;
        wireState = WireState::Null;
        std::cout << "debug 1\n";

        circuit.eraseWire(id);
        std::cout << "debug 2\n";
    }
}

void WireHandler::finishWireAtSegment(WireHit& wireSegment) {

    if (wireState != WireState::Creating || wireSegment.wireID < 0) return;
    
    activeWire->commitPreview();

    Wire& primaryWire = *circuit.getWire(wireSegment.wireID);
    auto& primaryGraph = primaryWire.getGraph();
    auto& activeGraph = activeWire->getGraph();
    SegmentHit& seg = wireSegment.segment;

    int junctionNodeID = -1;
    bool sameWire = (&primaryWire == activeWire);

    // build the position to Node map
    std::map<sf::Vector2f, int, Vector2fCompare> primaryPositionToNode;
    for (auto& [primaryID, node] : primaryWire.getGraph()) {
        primaryPositionToNode[node.position] = primaryID;
    }

    // check if there’s already a node at the snapped position
    if (seg.valid) {
        const sf::Vector2f& p = seg.snappedPosition;

        auto it = primaryPositionToNode.find(p);
        if (it != primaryPositionToNode.end()) {
            junctionNodeID = it->second; // node already exists
        }
        else {
            junctionNodeID = primaryWire.createNode(p);
            primaryPositionToNode[p] = junctionNodeID; // add it to the map
            // Connect to segment endpoints
            primaryWire.connectNodes(junctionNodeID, seg.nodeA);
            primaryWire.connectNodes(junctionNodeID, seg.nodeB);
        }
    }
    // update map of existing nodes for lookup

    for (auto& [primaryID, node] : primaryGraph) {
        primaryPositionToNode[node.position] = primaryID;
    }
    // remap active wire nodes into primary wire

    std::unordered_map<int, int> idRemap;

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

    for (auto& [activeID, activeNode] : activeGraph) { // uses ID map to update neighbor lists
        int primaryID = idRemap[activeID]; 

        for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
            int activeNeighbor = activeNode.neighbors[d];
            if (activeNeighbor == -1) continue;

            int primaryNeighbor = idRemap[activeNeighbor];
            if (primaryID == primaryNeighbor) continue;

            auto& primaryNode = primaryGraph.at(primaryID);
            if (!primaryNode.neighbors.has(d)) {
                primaryWire.connectNodes(primaryID, primaryNeighbor);
            }
        }
    }

    for (auto& [activeID, activeNode] : activeGraph) {
        if (activeNode.isAnchor) {
            int primaryID = idRemap[activeID];
            primaryWire.getGraph().at(primaryID).isAnchor = true;
        }
    }

   

    if (!sameWire) {
        int primaryElectricalNodeID = circuit.wireIDToElectricalNode.at(primaryWire.ID);
        circuit.mergeElectricalNodes(primaryElectricalNodeID, activeElectricalNodeID);

        activeWire->selected = false;
        int id = activeWire->ID;
        activeWire = nullptr;
        wireState = WireState::Null;

        circuit.eraseWire(id);
    }
    else {
        activeWire->selected = false;
        wireState = WireState::Null;
        activeWire = &primaryWire;
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