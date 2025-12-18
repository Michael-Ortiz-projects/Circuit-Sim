#include "UI_Manager.h"
#include "../Config.h"

UI_Manager::UI_Manager() {}

void UI_Manager::initialize(AssetManager& assets) {
	menu_map.try_emplace(MenuID::Place, assets.mainFont, "Place", sf::Vector2f(20, 0), sf::Vector2f(70, 25));


	menu_map.at(MenuID::Place).addOption("Voltage Source", UICommand::PlaceVoltageSource);
	menu_map.at(MenuID::Place).addOption("Resistor", UICommand::PlaceResistor);
	menu_map.at(MenuID::Place).addOption("Current Source", UICommand::PlaceCurrentSource);
	menu_map.at(MenuID::Place).addOption("Capacitor", UICommand::PlaceCapacitor);
	menu_map.at(MenuID::Place).addOption("Inductor", UICommand::PlaceInductor);
	menu_map.at(MenuID::Place).addOption("Switch", UICommand::PlaceSwitch);
}

bool UI_Manager::pollCommand(UICommand& outputCommand) {

	return menu_map.at(MenuID::Place).poll(outputCommand);
}

bool UI_Manager::onMousePress(const sf::Vector2f& point) {

	if (menu_map.at(MenuID::Place).contains(point)) {
		menu_map.at(MenuID::Place).onMousePress(point);
		return true;
	}
	return false;
}

bool UI_Manager::onMouseMove(const sf::Vector2f& point) {

	menu_map.at(MenuID::Place).onMouseMove(point);
	return false;
}

bool UI_Manager::onMouseRelease(const sf::Vector2f& point) {
	return menu_map.at(MenuID::Place).onMouseRelease(point);
}