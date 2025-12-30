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
        if (interaction.hasLead() && circuit.leadIsEmpty(interaction.connection)) {
            finishWireAtConnection(interaction.connection);
            std::cout << "Ending Wire Creation\n";
        }

        if (interaction.hasLead() && !circuit.leadIsEmpty(interaction.connection)) {
            
        }

        if (!interaction.hasLead()) {
            std::cout << "This ran\n";
            if (wireSegment.valid) {
                std::cout << "Snapped position:";
                Debug::printVector2f(wireSegment.segment.snappedPosition);
                std::cout << "Appending Node\n";
                finishWireAtSegment(wireSegment);
                
            }
            else {
                std::cout << "No valid segment hit to snap to\n";
            }
        }



        else {
            if (activeWire) {

                std::cout << "Appending Node\n";
                activeWire->commitPreview();
                activeWire->updatePreview(worldPos);
            }
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

            WireMoveResult moveResult = activeWire->moveNode(interaction.wire_node.nodeID, worldPos, WireMoveIntent::Edit);

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
    activeWire->anchorNodes.emplace(activeElectricalNodeID, connection);
    activeWire->selected = true;


    wireState = WireState::Creating;
    std::cout << "WireState = Creating\n\n";
}

void WireHandler::finishWireAtConnection(ElectricalConnection& end) {
    if (wireState != WireState::Creating || !activeWire) return;

    int finalWireNodeID = activeWire->commitPreview();
    circuit.addConnectionToNode(activeWire->ID, end);
    circuit.updateComponentLead(activeWire->ID, finalWireNodeID, activeElectricalNodeID, end);
    activeWire->anchorNodes.emplace(finalWireNodeID, end);
    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
}

void WireHandler::beginNodeDrag(WireNodeReference& ref) {
    activeWire = circuit.getWire(ref.wireID);

    wireState = WireState::DraggingNode;
}

void WireHandler::finishWireAtSegment(WireHit& wireSegment) {

    if (wireState != WireState::Creating || !activeWire || wireSegment.wireID < 0) return;
    
    activeWire->commitPreview();

    Wire& primaryWire = *circuit.getWire(wireSegment.wireID);
    auto& primaryGraph = primaryWire.getGraph();
    std::map<sf::Vector2f, int, Vector2fCompare> primaryPositionToNode;

    for (auto& [primaryID, node] : primaryGraph) {
        primaryPositionToNode[node.position] = primaryID;
    }

    std::unordered_map<int, int> idRemap;

    for (auto& [activeID, node] : activeWire->getGraph()) {
        auto it = primaryPositionToNode.find(node.position);

        if (it != primaryPositionToNode.end()) {
            idRemap[activeID] = it->second;
        }

        else {
            int newID = primaryWire.insertNode(node.position);
            idRemap[activeID] = newID;
            primaryPositionToNode[node.position] = newID;
        }
    }

    for (auto& [activeID, activeNode] : activeWire->getGraph()) { // uses ID map to update neighbor lists
        int primaryID = idRemap[activeID]; 

        for (int activeNeighbor : activeNode.neighbors) {
            int primaryNeighbor = idRemap[activeNeighbor]; 

            if (primaryID == primaryNeighbor) continue;

            auto& neighbors = primaryGraph.at(primaryID).neighbors;

            if (std::find(neighbors.begin(), neighbors.end(), primaryNeighbor) == neighbors.end()) {
                neighbors.push_back(primaryNeighbor);
                primaryGraph[primaryNeighbor].neighbors.push_back(primaryID);
            }
        }
    }

    for (auto& [activeNodeID, connection] : activeWire->anchorNodes) {
        int primaryID = idRemap[activeNodeID];
        primaryWire.anchorNodes[primaryID] = connection;
    }
    
    //do remapping for junction nodes when I get to junction merging logic


    int primaryElectricalNodeID = circuit.wireIDToElectricalNode.at(primaryWire.ID);

    circuit.mergeElectricalNodes(primaryElectricalNodeID, activeElectricalNodeID);

    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
    //need to work on erasing wires from circuit and junction moving logic (they should be treated like anchors in a way)
}

void WireHandler::setInteractionContext(WireInteraction context) {
    interaction = context;
}

void WireHandler::setSegmentContext(WireHit context) {
    wireSegment = context;
}