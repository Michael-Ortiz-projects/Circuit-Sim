#include "UI_Manager.h"
#include "../Config.h"

UI_Manager::UI_Manager(Controller& Controller) 
	: controller(Controller) { }

void UI_Manager::initialize(AssetManager& assets) {
	menu_map.try_emplace(MenuID::Place, assets.mainFont, "Place", sf::Vector2f(20, 0), sf::Vector2f(70, 25));


	menu_map.at(MenuID::Place).addOption("Voltage Source", UICommand::PlaceVoltageSource);
	menu_map.at(MenuID::Place).addOption("Resistor", UICommand::PlaceResistor);
	menu_map.at(MenuID::Place).addOption("Current Source", UICommand::PlaceCurrentSource);
	menu_map.at(MenuID::Place).addOption("Capacitor", UICommand::PlaceCapacitor);
	menu_map.at(MenuID::Place).addOption("Inductor", UICommand::PlaceInductor);
	menu_map.at(MenuID::Place).addOption("Switch", UICommand::PlaceSwitch);
}

bool UI_Manager::handleEvent(const sf::Event& event) { //true if event consumed by UI
	bool consumed = false;

	switch (event.type) {
	case sf::Event::MouseButtonPressed: {
		sf::Vector2f pixelPos(event.mouseButton.x, event.mouseButton.y);
		consumed = onMousePress(pixelPos);
		break;
	}

	case sf::Event::MouseMoved: {
		sf::Vector2f pixelPos(event.mouseButton.x, event.mouseButton.y);
		consumed = onMouseMove(pixelPos);
		break;
	}

	case sf::Event::MouseButtonReleased: {
		sf::Vector2f pixelPos(event.mouseButton.x, event.mouseButton.y);
		consumed = onMouseRelease(pixelPos);
		break;
	}
	default:
		break;
	}

	return consumed;
}

bool UI_Manager::pollCommand(UICommand& outputCommand) {
	for (auto& [id, menu] : menu_map) {
		if (menu.poll(outputCommand)) {
			return true;
		}
	}
	return false;
}

bool UI_Manager::onMousePress(const sf::Vector2f& pixelPos) {
	for (auto& [id, menu] : menu_map) {
		if (menu.contains(pixelPos)) {
			menu.onMousePress(pixelPos);
			return true; 
		}
	}
	return false;
}

bool UI_Manager::onMouseMove(const sf::Vector2f& pixelPos) {
	for (auto& [id, menu] : menu_map) {
		menu.onMouseMove(pixelPos);
	}
	return false;
}

bool UI_Manager::onMouseRelease(const sf::Vector2f& pixelPos) {
	for (auto& [id, menu] : menu_map) {
		if (menu.onMouseRelease(pixelPos)) {
			return true;
		}
	}
	return false;
}