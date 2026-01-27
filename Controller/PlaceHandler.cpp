#include "PlaceHandler.h"

PlaceHandler::PlaceHandler(std::vector<SchematicComponent>& comps, Circuit& Circuit, AssetManager& Assets)
	: components(comps), circuit(Circuit), assets(Assets) {}

void PlaceHandler::onMousePress(const sf::Vector2f& worldPos) {
	placeComponent(worldPos);
}

void PlaceHandler::onMouseMove(const sf::Vector2f& worldPos) {

}


bool PlaceHandler::shouldRelease() const {
	return true;
}

void PlaceHandler::onKeyPress(const sf::Event::KeyEvent& event) {
	if (event.code == sf::Keyboard::Delete) {//this probably shouldnt be here
		deleteComponents();
	}
	if (event.code == sf::Keyboard::R) {
		
	}
}

void PlaceHandler::placeComponent(const sf::Vector2f& worldPos) {
	int newID = circuit.addComponent(Component(-1, -1, type, 100), worldPos);
	std::cout << "placed component\n";
}

void PlaceHandler::deleteComponents() {
	std::vector<int> toDelete;

	for (const auto& c : components) {
		if (c.selected) {
			toDelete.push_back(c.componentID);
		}
	}

	for (const auto& id : toDelete)
		circuit.removeComponent(id);
}
void PlaceHandler::setComponentType(ComponentType comp_type) {
	type = comp_type;
}