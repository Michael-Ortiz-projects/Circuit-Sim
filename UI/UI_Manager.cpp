#include "UI_Manager.h"
#include "../Config.h"

UI_Manager::UI_Manager(Renderer& rend)
	: renderer(rend) { }

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
	sf::Vector2f pixelPos;

	if (activeDialog != nullptr && activeDialog->isOpen()) {
		activeDialog->handleEvent(event);
		return true;
	}

	bool consumed = false;

	switch (event.type) {
	case sf::Event::MouseButtonPressed: {
		pixelPos = renderer.getWindow().mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), renderer.getUIView());
		consumed = onMousePress(pixelPos);
		break;
	}

	case sf::Event::MouseMoved: {
		pixelPos = renderer.getWindow().mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), renderer.getUIView());
		consumed = onMouseMove(pixelPos);
		break;
	}

	case sf::Event::MouseButtonReleased: {

		pixelPos = renderer.getWindow().mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), renderer.getUIView());
		consumed = onMouseRelease(pixelPos);
		break;
	}
	default:
		break;
	}

	return consumed;
}

bool UI_Manager::pollCommand(UICommand& outputCommand) {
	if (activeDialog && activeDialog->buttonPressed(outputCommand)) {
		return true;
	}

	for (auto& [id, menu] : menu_map) {
		if (menu.poll(outputCommand))
			return true;
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

void UI_Manager::openEditDialog(Component* target) {
	std::cout << "openEditDialog called\n";
	if (activeDialog) return;

	activeDialog = std::make_unique<Dialog>(sf::Vector2f(800.f, 150.f), sf::Vector2f(600, 200), renderer);

	activeDialog->addLabel("Edit Component Value", { 20.f, 20.f }, 18);
	activeDialog->addLabel("Value:", { 20.f, 70.f });

	activeDialog->addTextBox(
		std::to_string(target->value),
		{ 100.f, 65.f },
		{ 200.f, 30.f }
	);

	activeDialog->addButton("OK", UICommand::ApplyEdit, { 350.f, 130.f }, { 80.f, 30.f });
	activeDialog->addButton("Cancel", UICommand::CancelEdit, { 450.f, 130.f }, { 80.f, 30.f });

	activeDialog->open();
}

void UI_Manager::closeEditDialog() {
	activeDialog.reset();
}

bool UI_Manager::hasActiveDialog() const {
	return activeDialog != nullptr;
}

const std::string UI_Manager::getEditDialogText() const {
	static std::string empty = "empty text";
	return activeDialog ? activeDialog->getText(0) : empty;
}

void UI_Manager::draw() {
	for (auto& [id, menu] : menu_map)
		menu.draw(renderer.getWindow());

	if (activeDialog && activeDialog->isOpen())
		activeDialog->draw();
}