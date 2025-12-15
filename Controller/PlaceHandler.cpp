#include "PlaceHandler.h"

PlaceHandler::PlaceHandler(std::vector<SchematicComponent>& comps, Circuit& Circuit, AssetManager& Assets)
	: components(comps), circuit(Circuit), assets(Assets) {}

void PlaceHandler::onMousePress(const sf::Vector2f& worldPos) {
	int newID = circuit.AddComponent(type);

	if (newID != -1) {
		components.emplace_back(newID, worldPos, 0.f, type);
		components.back().setTexture(assets.getTexture(type));
	}
}



bool PlaceHandler::shouldRelease() const {
	return true;
}
void PlaceHandler::setComponentType(ComponentType comp_type) {
	type = comp_type;
}
