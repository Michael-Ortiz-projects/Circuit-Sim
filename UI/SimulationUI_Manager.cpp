#include "SimulationUI_Manager.h"
#include "../Core/Simulator.h"

SimulationUI_Manager::SimulationUI_Manager(Renderer& rend) 
	: renderer(rend), graphRect(sf::FloatRect(50, 75, 1600, 900)), simulationGraph(rend.getAssets(), sf::FloatRect(50, 75, 1600, 900)) { }

void SimulationUI_Manager::initialize() {
	button_map.try_emplace(SimulationUICommand::NewSimulation, renderer.getAssets().mainFont, "New Simulation", sf::Vector2f(0, 0), sf::Vector2f(120, 25), SimulationUICommand::NewSimulation);
	button_map.try_emplace(SimulationUICommand::EditSimulation, renderer.getAssets().mainFont, "Edit Simulation", sf::Vector2f(122, 0), sf::Vector2f(120, 25), SimulationUICommand::EditSimulation);
	button_map.try_emplace(SimulationUICommand::RunSimulation, renderer.getAssets().mainFont, "Run Simulation", sf::Vector2f(244, 0), sf::Vector2f(120, 25), SimulationUICommand::RunSimulation);

	menu_map.try_emplace(MenuID::Graph, renderer.getAssets().mainFont, "Graph", sf::Vector2f(366, 0), sf::Vector2f(120, 25), SimulationUICommand::ToggleMenu);
	menu_map.at(MenuID::Graph).addOption("Data", SimulationUICommand::GraphData);
	menu_map.at(MenuID::Graph).addOption("Settings", SimulationUICommand::GraphSettings);

	resultTextBox.setFont(renderer.getAssets().mainFont);
	resultTextBox.setCharSize(16);
	resultTextBox.setPosition({ 1, 1089 });
	resultTextBox.setSize({ 500, 350 });
	resultTextBox.clear();

	simulationGraph.setArea(graphRect, renderer.getWindow().getSize());
}

bool SimulationUI_Manager::handleEvent(const sf::Event& event) {
	sf::Vector2f pixelPos;

	if (activeDialog != nullptr && activeDialog->isOpen()) {
		activeDialog->handleEvent(event);
		//std::cout << "activeDialog.handleEvent() running\n";
		return true;
	}

	resultTextBox.handleEvent(event, renderer.getWindow());
	simulationGraph.handleEvent(event, renderer.getWindow());
	
	bool consumed = false;

	switch (event.type) {
	case sf::Event::MouseButtonPressed: {
		pixelPos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
		consumed = onMousePress(pixelPos);
		break;
	}

	case sf::Event::MouseMoved: {
		pixelPos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
		consumed = onMouseMove(pixelPos);
		break;
	}

	case sf::Event::MouseButtonReleased: {

		pixelPos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
		consumed = onMouseRelease(pixelPos);
		break;
	}
	default:
		break;
	}

	return consumed;
}

bool SimulationUI_Manager::pollCommand(SimulationUICommand& outputCommand) {
	if (activeDialog && activeDialog->poll(outputCommand)) {
		return true;
	}

	for (auto& [id, menu] : menu_map) {
		if (menu.poll(outputCommand))
			return true;
	}

	for (auto& [id, button] : button_map) {
		if (button.consumed(outputCommand))
			return true;
	}

	return false;

}

bool SimulationUI_Manager::onMousePress(const sf::Vector2f& pixelPos) {
	std::cout << "SimulationUI_Manager.onMousePress() running\n";
	for (auto& [id, menu] : menu_map) {
		if (menu.contains(pixelPos)) {
			std::cout << "menu contains point\n";
			menu.onMousePress(pixelPos);
			return true;
		}
	}

	for (auto& [id, button] : button_map) {
		if (button.contains(pixelPos)) {
			button.onMousePress(pixelPos);
			return true;
		}
	}

	return false;
}

bool SimulationUI_Manager::onMouseMove(const sf::Vector2f& pixelPos) {
	for (auto& [id, menu] : menu_map) {
		menu.onMouseMove(pixelPos);
	}
	for (auto& [id, button] : button_map) {
		button.onMouseMove(pixelPos);
	}

	return true;
}

bool SimulationUI_Manager::onMouseRelease(const sf::Vector2f& pixelPos) {
	for (auto& [id, menu] : menu_map) {
		menu.onMouseRelease(pixelPos);
	}

	for (auto& [id, button] : button_map) {
		button.onMouseRelease(pixelPos);
	}
	return true;
}

void SimulationUI_Manager::openNewSimulationDialog() {
	std::cout << "openEditDialog called\n";
	if (activeDialog) return;

	int textSize = 16;

	activeDialog = std::make_unique<Dialog<SimulationUICommand>>(sf::Vector2f(1000.f, 200.f), sf::Vector2f(600, 200), renderer);

	activeDialog->addLabel("Simulation Type:", { 30.f, 40.f }, textSize);
	activeDialog->addButton("OK", SimulationUICommand::CreateSimulation, { 400.f, 150.f }, { 80.f, 30.f });
	activeDialog->addButton("Cancel", SimulationUICommand::CancelCreateSimulation, { 500.f, 150.f }, { 80.f, 30.f });
	activeDialog->addDropdown(MenuID::SimulationType, renderer.getAssets().mainFont, "Simulation Type", { 200.f, 40.f }, { 200.f, 30.f }, SimulationUICommand::ToggleMenu);
	activeDialog->addOptionToDropdown(MenuID::SimulationType, "DC Operating Point", SimulationUICommand::DCOP);
	activeDialog->addOptionToDropdown(MenuID::SimulationType, "Transient", SimulationUICommand::TRAN);

	activeDialog->open();
}

void SimulationUI_Manager::openEditSimulationDialog(SimulationType type, Config simParam) {
	std::cout << "openEditSimulationDialog called\n";
	if (activeDialog) return;

	int textSize = 16;

	activeDialog = std::make_unique<Dialog<SimulationUICommand>>(sf::Vector2f(1000.f, 200.f), sf::Vector2f(800, 500), renderer);
	activeDialog->addButton("OK", SimulationUICommand::ApplyEditSimulation, { 600.f, 450.f }, { 80.f, 30.f });
	activeDialog->addButton("Cancel", SimulationUICommand::CancelEditSimulation, { 700.f, 450.f }, { 80.f, 30.f });

	switch (type) {
	case SimulationType::DC:
		activeDialog->addLabel("Edit DC Operating Point Simulation", { 30.f, 40.f }, textSize + 6);
		//no parameters for a DC solution
		break;

	case SimulationType::Transient:
		activeDialog->addLabel("Edit Transient Simulation", { 30.f, 40.f }, textSize + 6);
		activeDialog->addLabel("Time Step:", { 40.f, 100.f }, textSize);
		activeDialog->addTextBox(formatValueWithSuffix(simParam.timeStep), { 145.f, 100.f }, { 200.f, 30.f }, textSize, true);

		activeDialog->addLabel("Initial Time:", { 40.f, 200.f }, textSize);
		activeDialog->addTextBox(formatValueWithSuffix(simParam.tStart), {145.f, 200.f}, {200.f, 30.f}, textSize, true);

		activeDialog->addLabel("Final Time:", { 40.f, 300.f }, textSize);
		activeDialog->addTextBox(formatValueWithSuffix(simParam.tEnd), { 145.f, 300.f }, { 200.f, 30.f }, textSize, true);
		break;
	}

	activeDialog->open();
}

void SimulationUI_Manager::openGraphSettingsDialog() {
	std::cout << "openGraphDataDialog called\n";
	if (activeDialog) return;

	int textSize = 16;
	activeDialog = std::make_unique<Dialog<SimulationUICommand>>(sf::Vector2f(1000.f, 200.f), sf::Vector2f(800, 500), renderer);
	
	activeDialog->addLabel("Min X:", sf::Vector2f(20, 100), textSize);
	activeDialog->addLabel("Max X:", sf::Vector2f(20, 200), textSize);
	activeDialog->addLabel("Min Y:", sf::Vector2f(20, 300), textSize);
	activeDialog->addLabel("Max Y:", sf::Vector2f(20, 400), textSize);

	activeDialog->addTextBox(formatValueWithSuffix(simulationGraph.getBounds().minX), sf::Vector2f(80, 95), sf::Vector2f(80, 25), textSize, true);
	activeDialog->addTextBox(formatValueWithSuffix(simulationGraph.getBounds().maxX), sf::Vector2f(80, 195), sf::Vector2f(80, 25), textSize, true);
	activeDialog->addTextBox(formatValueWithSuffix(simulationGraph.getBounds().minY), sf::Vector2f(80, 295), sf::Vector2f(80, 25), textSize, true);
	activeDialog->addTextBox(formatValueWithSuffix(simulationGraph.getBounds().maxY), sf::Vector2f(80, 395), sf::Vector2f(80, 25), textSize, true);

	activeDialog->addButton("Autoscale", SimulationUICommand::AutoScaleGraph, { 500.f, 450.f }, { 80.f, 30.f });
	activeDialog->addButton("OK", SimulationUICommand::ApplyGraphSettings, { 600.f, 450.f }, { 80.f, 30.f });
	activeDialog->addButton("Cancel", SimulationUICommand::CancelGraphSettings, { 700.f, 450.f }, { 80.f, 30.f });

	activeDialog->open();

	// i want to add more y_axis data options for current and voltage of every circuit element, currently I am only able to graph the node voltages as well as any extra variables produced by the elements
	// i need to fix graph tick markers, sometimes when autoscaling the numbers dont go all they way up
}

void SimulationUI_Manager::openGraphDataDialog(Circuit& circuit) {
	std::cout << "openGraphDataDialog called\n";
	if (activeDialog) return;

	int textSize = 16;
	activeDialog = std::make_unique<Dialog<SimulationUICommand>>(sf::Vector2f(1000.f, 200.f), sf::Vector2f(800, 500), renderer);


	activeDialog->addLabel("Plot Y-axis Variable: ", sf::Vector2f(20, 100), textSize);
	activeDialog->addDropdown(MenuID::GraphY_Axis, renderer.getAssets().mainFont, "", sf::Vector2f(180, 100), sf::Vector2f(120, 25), SimulationUICommand::ToggleMenu);
	auto& graphVariables(circuit.getSimulator().graphVariables);
	std::string menuLabel;
	menuLabel = "None";
	activeDialog->getMenu(MenuID::GraphY_Axis).setLabel(menuLabel);

	


	for (int i = 0; i < graphVariables.size(); i++) {
		activeDialog->addOptionToDropdown(MenuID::GraphY_Axis, graphVariables[i].label, SimulationUICommand::UpdateY_AxisMenu, i);
	}
	
	activeDialog->addLabel("Plot X-Axis Variable: ", sf::Vector2f(380, 100), textSize);
	activeDialog->addDropdown(MenuID::GraphX_Axis, renderer.getAssets().mainFont, "", sf::Vector2f(540, 100), sf::Vector2f(120, 25), SimulationUICommand::ToggleMenu);
	

	activeDialog->addButton("OK", SimulationUICommand::ApplyGraphData, { 600.f, 450.f }, { 80.f, 30.f });
	activeDialog->addButton("Cancel", SimulationUICommand::CancelGraphData, { 700.f, 450.f }, { 80.f, 30.f });

	if (graphDataY_Varidx == -1) menuLabel = "None";

	else {
		menuLabel = activeDialog->getMenu(MenuID::GraphY_Axis).getOption(graphDataY_Varidx).button.getName();
	}
	activeDialog->getMenu(MenuID::GraphY_Axis).setLabel(menuLabel);

	

	activeDialog->open();
}


EditSimulationDialogResult SimulationUI_Manager::getEditSimulationDialogText(SimulationType type) {
	EditSimulationDialogResult simulationParameters;
	switch (type) {
	case SimulationType::DC:
		return simulationParameters;
	case SimulationType::Transient:
		simulationParameters.timeStep_text = activeDialog->getText(0);
		simulationParameters.tStart_text = activeDialog->getText(1);
		simulationParameters.tEnd_text = activeDialog->getText(2);
		return simulationParameters;
	default:
		return simulationParameters;
	}
}

GraphSettingsDialogResult SimulationUI_Manager::getGraphSettingsDialogText() {
	GraphSettingsDialogResult graphBounds_text;
	return { activeDialog->getText(0), activeDialog->getText(1), activeDialog->getText(2), activeDialog->getText(3) };
}


std::string SimulationUI_Manager::formatValueWithSuffix(double value) {
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

void SimulationUI_Manager::draw() {
	renderer.getWindow().draw(resultTextBox);

	simulationGraph.draw(renderer.getWindow());

	for (auto& [id, menu] : menu_map)
		menu.draw(renderer.getWindow());
	for (auto& [id, button] : button_map)
		button.draw(renderer.getWindow());

	if (activeDialog && activeDialog->isOpen())
		activeDialog->draw();


}