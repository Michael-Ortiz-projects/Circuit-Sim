#include "WireHandler.h"

WireHandler::WireHandler(Circuit& Circuit, std::vector<SchematicComponent> components)
	: circuit(Circuit), schematicComponents(components) { }

void WireHandler::onMousePress(const sf::Vector2f& worldPos) {
	if (circuit.leadIsEmpty(attemptedConnection)) { // checks if a connection to a component was attempted
		int newNodeID = circuit.createElectricalNode();

		circuit.addConnectionToNode(newNodeID, attemptedConnection.componentID, attemptedConnection.lead);

		sf::Vector2f position = positionOfConnection(attemptedConnection);
		int newWireID = circuit.createWire(snapPositionToGrid(position));
		activeWire = circuit.getWire(newWireID);
	}

	else { //place new wire point
		//int newWireNodeID = activeWire->addNodeToWire(snapPositionToGrid(worldPos));
	}
}

void WireHandler::onMouseMove(const sf::Vector2f& worldPos) {

}

void WireHandler::onMouseRelease(const sf::Vector2f& worldPos) {
	attemptedConnection = { -1, Lead::Null };
}

bool WireHandler::shouldRelease() const {
	return true;
}

void WireHandler::createWire(const sf::Vector2f& worldPos) {

}

sf::Vector2f& WireHandler::snapPositionToGrid(sf::Vector2f& position) {
	position.x = std::round(position.x / gridSize) * gridSize;
	position.y = std::round(position.y / gridSize) * gridSize;

	return position;
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

	if (connection.lead == Lead::A) {
		return it->getLeadPositionA();
	}
	if (connection.lead == Lead::B) {
		return it->getLeadPositionB();
	}
}