#include "PlaceHandler.h"

PlaceHandler::PlaceHandler(Circuit& Circuit, AssetManager& Assets)
	: circuit(Circuit), assets(Assets) {}

void PlaceHandler::onMousePress(const sf::Vector2f& worldPos) {
	placeComponent(worldPos);
}

void PlaceHandler::onMouseMove(const sf::Vector2f& worldPos) {

}


bool PlaceHandler::shouldRelease() const {
	return true;
}

void PlaceHandler::onKeyPress(const sf::Event::KeyEvent& event) {

}

void PlaceHandler::placeComponent(const sf::Vector2f& worldPos) {
	int newID = circuit.addComponent(type, worldPos);
	SchematicComponent* comp = circuit.getSchematicComponent(newID);

	comp->setTexture(assets.getTexture(comp->getType()));

	comp->startDrag(worldPos);
	comp->dragTo(worldPos);
	comp->stopDrag();
}

void PlaceHandler::setComponentType(ComponentType comp_type) {
	type = comp_type;
}