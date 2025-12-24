#include "WireHandler.h"

WireHandler::WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components)
	: circuit(Circuit), schematicComponents(components) { }

void WireHandler::onMousePress(const sf::Vector2f& worldPos) {
    if (attemptedConnection.lead == Lead::Null)
    {
        if (wireState == WireState::Creating)
        {
            if (activeWire)
                activeWire->commitPreview();
        }
    }

    if (circuit.leadIsEmpty(attemptedConnection))
    {
        if (wireState == WireState::Null)
        {
            int newNodeID = circuit.createElectricalNode();
            circuit.addConnectionToNode(newNodeID, attemptedConnection);

            sf::Vector2f position = positionOfConnection(attemptedConnection);
            int newWireID = circuit.createWire(snapPositionToGrid(position));

            activeWire = circuit.getWire(newWireID);
            wireState = WireState::Creating;
            activeWire->selected = true;

            activeWire->updatePreview(snapPositionToGrid(worldPos));
            attemptedConnection = { -1, Lead::Null };
        }
        else if (wireState == WireState::Creating)
        {
            if (activeWire)
            {
                circuit.addConnectionToNode(activeWire->ID, attemptedConnection);

                activeWire->commitPreview();

                wireState = WireState::Null;
                activeWire->selected = false;
                activeWire = nullptr;
            }
        }
    }
    if (activeWire) Debug::debugPrintWire(*activeWire);
}

void WireHandler::onMouseMove(const sf::Vector2f& worldPos) {
	if (activeWire) activeWire->updatePreview(snapPositionToGrid(worldPos));
}

void WireHandler::onMouseRelease(const sf::Vector2f& worldPos) {
	attemptedConnection = { -1, Lead::Null };
}

bool WireHandler::shouldRelease() const {
	return wireState == WireState::Null;
}

void WireHandler::createWire(const sf::Vector2f& worldPos) {

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