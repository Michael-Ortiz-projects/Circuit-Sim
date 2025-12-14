#include "UI_Manager.h"
#include "../Config.h"

UI_Manager::UI_Manager() {}

void UI_Manager::initialize(AssetManager& assets) {
	placeMenu.emplace(assets.mainFont, "Place", sf::Vector2f(20, 0), sf::Vector2f(70, 25));

	placeMenu->addOption("Voltage Source", UICommand::PlaceVoltageSource);
	placeMenu->addOption("Resistor", UICommand::PlaceResistor);
}

bool UI_Manager::pollCommand(UICommand& outputCommand) {
	if (!placeMenu) return false;

	return placeMenu->poll(outputCommand);
}

bool UI_Manager::onMousePress(const sf::Vector2f& point) {
	if (!placeMenu) return false;

	if (placeMenu->contains(point)) {
		placeMenu->onMousePress(point);
		return true;
	}
	return false;
}

bool UI_Manager::onMouseMove(const sf::Vector2f& point) {
	if (placeMenu) {
		placeMenu->onMouseMove(point);
	}
	return false;
}

bool UI_Manager::onMouseRelease(const sf::Vector2f& point) {
	if (placeMenu) {
		return placeMenu->onMouseRelease(point);
	}
	return false;
}

void UI_Manager::draw(sf::RenderWindow& window) {
	placeMenu->draw(window);
}