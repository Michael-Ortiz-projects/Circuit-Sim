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

        else if (interaction.hasNode()) {
            std::cout << "Beginning Drag\n";
            beginNodeDrag(interaction.wire_node);

        }
        break;
    
    case WireState::Creating:
        if (interaction.hasLead() && circuit.leadIsEmpty(interaction.connection)) {
            finishWireAtConnection(interaction.connection);
            std::cout << "Ending Wire Creation\n";
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
            activeWire->moveNode(interaction.wire_node.nodeID, worldPos, WireMoveIntent::Edit);
        }
        break;

    default:
        break;
    }
}

void WireHandler::onMouseRelease(const sf::Vector2f& worldPos) {
    interaction.connection = { -1, Lead::Null };
    
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

    int nodeID = circuit.createElectricalNode();
    circuit.addConnectionToNode(nodeID, connection);
    circuit.updateComponentLead(nodeID, connection);

    sf::Vector2f pos = positionOfConnection(connection);
    int wireID = circuit.createWire(pos);

    activeWire = circuit.getWire(wireID);
    activeWire->selected = true;


    wireState = WireState::Creating;
    std::cout << "WireState = Creating\n\n";
}

void WireHandler::finishWireAtConnection(ElectricalConnection& end) {
    if (wireState != WireState::Creating || !activeWire) return;

    circuit.addConnectionToNode(activeWire->ID, end);
    activeWire->commitPreview();

    activeWire->selected = false;
    activeWire = nullptr;
    wireState = WireState::Null;
}

void WireHandler::beginNodeDrag(WireNodeReference& ref) {
    activeWire = circuit.getWire(ref.wireID);

    wireState = WireState::DraggingNode;
}

void WireHandler::setInteractionContext(WireInteraction context) {
    interaction = context;
}