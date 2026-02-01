#include "UI_Manager.h"
#include "../Config.h"

UI_Manager::UI_Manager(Renderer& rend)
	: renderer(rend) { }

void UI_Manager::initialize(AssetManager& assets) {
	menu_map.try_emplace(MenuID::Place, assets.mainFont, "Place", sf::Vector2f(82, 0), sf::Vector2f(80, 25));
	menu_map.try_emplace(MenuID::File, assets.mainFont, "File", sf::Vector2f(0, 0), sf::Vector2f(80, 25));

	menu_map.at(MenuID::Place).addOption("Voltage Source", UICommand::PlaceVoltageSource);
	menu_map.at(MenuID::Place).addOption("Resistor", UICommand::PlaceResistor);
	menu_map.at(MenuID::Place).addOption("Current Source", UICommand::PlaceCurrentSource);
	menu_map.at(MenuID::Place).addOption("Capacitor", UICommand::PlaceCapacitor);
	menu_map.at(MenuID::Place).addOption("Inductor", UICommand::PlaceInductor);
	menu_map.at(MenuID::Place).addOption("Switch", UICommand::PlaceSwitch);
	menu_map.at(MenuID::Place).addOption("Ground", UICommand::PlaceGround);

	menu_map.at(MenuID::File).addOption("New", UICommand::OpenNewFile);
	menu_map.at(MenuID::File).addOption("Open", UICommand::OpenFile);
	menu_map.at(MenuID::File).addOption("Save", UICommand::SaveFile);
	menu_map.at(MenuID::File).addOption("Save As", UICommand::SaveFileAs);
	menu_map.at(MenuID::File).addOption("Exit", UICommand::ExitProgram);

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

void UI_Manager::openEditDialog(NetlistComponent* target) {
	std::cout << "openEditDialog called\n";
	if (activeDialog) return;

	int textSize = 16;

	activeDialog = std::make_unique<Dialog>(sf::Vector2f(800.f, 700.f), sf::Vector2f(600, 200), renderer);

	activeDialog->addLabel("Edit Component Value", { 20.f, 20.f }, 16);
	activeDialog->addLabel("Label:", { 20.f, 70.f });
	activeDialog->addLabel("Value:", { 20.f, 110.f });

	activeDialog->addTextBox(target->label, { 100.f, 65.f }, { 200.f, 30.f }, textSize, true);
	activeDialog->addTextBox(formatValue(target->value), { 100.f, 105.f }, { 200.f, 30.f }, textSize, true);

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

const EditDialogResult UI_Manager::getEditDialogText() const {
	EditDialogResult result;
	if (!activeDialog) return { "empty", "empty" };
	result.labelText = activeDialog->getText(0);
	result.valueText = activeDialog->getText(1);
	std::cout << "getEditDialogText() returns " << result.valueText << " and " << result.labelText << std::endl;
	return result;
}

std::string UI_Manager::formatValue(double value)
{
	std::ostringstream oss;
	oss << std::setprecision(6) << std::noshowpoint << value;
	return oss.str();
}

void UI_Manager::draw() {
	for (auto& [id, menu] : menu_map)
		menu.draw(renderer.getWindow());

	if (activeDialog && activeDialog->isOpen())
		activeDialog->draw();
}