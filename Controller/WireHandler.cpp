#include "WireHandler.h"

WireHandler::WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components)
	: circuit(Circuit), schematicComponents(components) { }

void WireHandler::onMousePress(const sf::Vector2f& worldPos) {
	if (circuit.leadIsEmpty(attemptedConnection)) { // checks if a connection to a component was attempted
		std::cout << "\nLead is empty\n";
		if (wireState == WireState::Null) {
			std::cout << "Wirestate = Null\n";
			int newNodeID = circuit.createElectricalNode();
			std::cout << "Adding connection to node\n";
			circuit.addConnectionToNode(newNodeID, attemptedConnection);
			std::cout << "getting connection Position\n";
			sf::Vector2f position = positionOfConnection(attemptedConnection);
			Debug::printVector2f(position);
			std::cout << "Creating Wire\n";
			int newWireID = circuit.createWire(snapPositionToGrid(position));
			std::cout << "setting activeWire\n";
			activeWire = circuit.getWire(newWireID);
			wireState = WireState::Creating;
			attemptedConnection = { -1, Lead::Null };
		}
		else if (wireState == WireState::Creating) {
			std::cout << "WireState = Creating\n Adding connection to node\n";
			circuit.addConnectionToNode(activeWire->ID, attemptedConnection);
			std::cout << "Adding new node to wire\n";
			activeWire->addNewNodeToWire(snapPositionToGrid(worldPos));
		}
	}	

	else {
		std::cout << "\nLead is not empty\n";
		if (wireState == WireState::Creating) {
			std::cout << "wirestate = Creating\n";
			std::cout << "Adding new node to wire\n";
			Debug::printVector2f(worldPos);
			activeWire->addNewNodeToWire(snapPositionToGrid(worldPos));//for some reason the node position is messed up bad
			//seems like the node position is not actually setting it
		}
	}
	Debug::debugPrintWire(*activeWire);
}

void WireHandler::onMouseMove(const sf::Vector2f& worldPos) {

}

void WireHandler::onMouseRelease(const sf::Vector2f& worldPos) {
	attemptedConnection = { -1, Lead::Null };
}

bool WireHandler::shouldRelease() const {
	return wireState == WireState::Null;
}

void WireHandler::createWire(const sf::Vector2f& worldPos) {

}

sf::Vector2f& WireHandler::snapPositionToGrid(const sf::Vector2f& position) {
	
	sf::Vector2f snappedPos(std::round(position.x / gridSize) * gridSize, std::round(position.y / gridSize) * gridSize);
	return snappedPos;
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