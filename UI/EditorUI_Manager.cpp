#include "EditorUI_Manager.h"
#include "../Config.h"

EditorUI_Manager::EditorUI_Manager(Renderer& rend)
	: renderer(rend) { }

void EditorUI_Manager::initialize() {
	menu_map.try_emplace(MenuID::Place, renderer.getAssets().mainFont, "Place", sf::Vector2f(82, 0), sf::Vector2f(80, 25), EditorUICommand::ToggleMenu);
	menu_map.try_emplace(MenuID::File, renderer.getAssets().mainFont, "File", sf::Vector2f(0, 0), sf::Vector2f(80, 25), EditorUICommand::ToggleMenu);
	menu_map.try_emplace(MenuID::Simulation, renderer.getAssets().mainFont, "Simulation", sf::Vector2f(164, 0), sf::Vector2f(80, 25), EditorUICommand::ToggleMenu);

	menu_map.at(MenuID::Place).addOption("Voltage Source", EditorUICommand::PlaceVoltageSource);
	menu_map.at(MenuID::Place).addOption("Resistor", EditorUICommand::PlaceResistor);
	menu_map.at(MenuID::Place).addOption("Current Source", EditorUICommand::PlaceCurrentSource);
	menu_map.at(MenuID::Place).addOption("Capacitor", EditorUICommand::PlaceCapacitor);
	menu_map.at(MenuID::Place).addOption("Inductor", EditorUICommand::PlaceInductor);
	menu_map.at(MenuID::Place).addOption("VCVS", EditorUICommand::PlaceVCVS);
	menu_map.at(MenuID::Place).addOption("VCCS", EditorUICommand::PlaceVCCS);
	menu_map.at(MenuID::Place).addOption("CCVS", EditorUICommand::PlaceCCVS);
	menu_map.at(MenuID::Place).addOption("CCCS", EditorUICommand::PlaceCCCS);
	menu_map.at(MenuID::Place).addOption("AC Voltage Source", EditorUICommand::PlaceACVoltageSource);
	menu_map.at(MenuID::Place).addOption("AC Current Source", EditorUICommand::PlaceACCurrentSource);


	//menu_map.at(MenuID::Place).addOption("Switch", EditorUICommand::PlaceSwitch);
	menu_map.at(MenuID::Place).addOption("Ground", EditorUICommand::PlaceGround);

	menu_map.at(MenuID::File).addOption("New", EditorUICommand::OpenNewFile);
	menu_map.at(MenuID::File).addOption("Open", EditorUICommand::OpenFile);
	menu_map.at(MenuID::File).addOption("Save", EditorUICommand::SaveFile);
	menu_map.at(MenuID::File).addOption("Save As", EditorUICommand::SaveFileAs);
	menu_map.at(MenuID::File).addOption("Exit", EditorUICommand::ExitProgram);

	menu_map.at(MenuID::Simulation).addOption("Run Simulation", EditorUICommand::OpenSimulationWindow);

}

bool EditorUI_Manager::handleEvent(const sf::Event& event) { //true if event consumed by UI
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

bool EditorUI_Manager::pollCommand(EditorUICommand& outputCommand) {
	if (activeDialog && activeDialog->buttonPressed(outputCommand)) {
		return true;
	}

	for (auto& [id, menu] : menu_map) {
		if (menu.poll(outputCommand))
			return true;
	}

	return false;
}

bool EditorUI_Manager::onMousePress(const sf::Vector2f& pixelPos) {
	std::cout << "EditorUI_Manager.onMousePress() running\n";
	for (auto& [id, menu] : menu_map) {
		if (menu.contains(pixelPos)) {
			std::cout << "menu contains point\n";
			menu.onMousePress(pixelPos);
			return true; 
		}
	}
	return false;
}

bool EditorUI_Manager::onMouseMove(const sf::Vector2f& pixelPos) {
	for (auto& [id, menu] : menu_map) {
		menu.onMouseMove(pixelPos);
	}
	return true;
}

bool EditorUI_Manager::onMouseRelease(const sf::Vector2f& pixelPos) {
	for (auto& [id, menu] : menu_map) {
		menu.onMouseRelease(pixelPos);
	}
	return true;
}

void EditorUI_Manager::openEditDialog(NetlistComponent* target) {
	std::cout << "openEditDialog called\n";
	if (activeDialog) return;

	int textSize = 16;

	activeDialog = std::make_unique<Dialog<EditorUICommand>>(sf::Vector2f(800.f, 700.f), sf::Vector2f(600, 200), renderer);

	activeDialog->addLabel("Edit Component Value", { 20.f, 20.f }, textSize);
	activeDialog->addLabel("Label:", { 20.f, 70.f }, textSize);
	activeDialog->addLabel("Value:", { 20.f, 110.f }, textSize);

	activeDialog->addTextBox(target->label, { 100.f, 65.f }, { 200.f, 30.f }, textSize, true);
	activeDialog->addTextBox(formatValueWithSuffix(target->value), { 100.f, 105.f }, { 200.f, 30.f }, textSize, true);

	activeDialog->addButton("OK", EditorUICommand::ApplyEdit, { 350.f, 130.f }, { 80.f, 30.f });
	activeDialog->addButton("Cancel", EditorUICommand::CancelEdit, { 450.f, 130.f }, { 80.f, 30.f });

	activeDialog->open();
}

void EditorUI_Manager::closeEditDialog() {
	activeDialog.reset();
}

bool EditorUI_Manager::hasActiveDialog() const {
	return activeDialog != nullptr;
}

const EditDialogResult EditorUI_Manager::getEditDialogText() const {
	EditDialogResult result;
	if (!activeDialog) return { "empty", "empty" };
	result.labelText = activeDialog->getText(0);
	result.valueText = activeDialog->getText(1);
	std::cout << "getEditDialogText() returns " << result.valueText << " and " << result.labelText << std::endl;
	return result;
}

std::string EditorUI_Manager::formatValue(double value)
{
	std::ostringstream oss;
	oss << std::setprecision(6) << std::noshowpoint << value;
	return oss.str();
}

std::string EditorUI_Manager::formatValueWithSuffix(double value) {
	if (value == 0.0)
		return "0";

	struct Suffix {
		double multiplier;
		char symbol;
	};

	static const std::vector<Suffix> suffixes = {
		{1e9,  'G'},
		{1e6,  'M'},
		{1e3,  'k'},
		{1.0,  '\0'}, // no suffix
		{1e-3, 'm'},
		{1e-6, 'u'},
		{1e-9, 'n'},
		{1e-12,'p'}
	};

	double absVal = std::abs(value);

	for (const auto& s : suffixes) {
		if (absVal >= s.multiplier) {
			double scaled = value / s.multiplier;

			std::ostringstream oss;
			oss << scaled;

			if (s.symbol != '\0')
				oss << s.symbol;

			return oss.str();
		}
	}

	// smaller than 1e-12
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

void EditorUI_Manager::draw() {
	auto& window = renderer.getWindow();
	window.setView(renderer.getUIView());
	for (auto& [id, menu] : menu_map)
		menu.draw(window);

	if (activeDialog && activeDialog->isOpen())
		activeDialog->draw();
}