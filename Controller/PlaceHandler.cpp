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
	if (event.code == sf::Keyboard::Delete) {
		deleteComponents();
	}
}


void PlaceHandler::placeComponent(const sf::Vector2f& worldPos) {
	int newID = circuit.AddComponent(type);

	if (newID != -1) {
		components.emplace_back(newID, worldPos, 0.f, type);
		components.back().setTexture(assets.getTexture(type));
		components.back().dragTo(worldPos);
	}
}

void PlaceHandler::deleteComponents() {
	std::vector<int> toDelete;

	for (const auto& c : components) {
		if (c.selected) {
			toDelete.push_back(c.componentID);
		}
	}
}
void PlaceHandler::setComponentType(ComponentType comp_type) {
	type = comp_type;
}

void PlaceHandler::setMode(EditMode m) {
	mode = m;
}