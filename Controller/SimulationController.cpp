#include "SimulationController.h"


SimulationController::SimulationController(Circuit& C, SimulationUI_Manager& ui)
	: circuit(C), UI(ui), dialogHandler(ui, simType, simParameters) { }

void SimulationController::handleEvent(const sf::Event& event) {
	switch (event.type) {

	case sf::Event::MouseButtonPressed: {

		if (event.mouseButton.button == sf::Mouse::Left) {

			onMousePress(event.mouseButton);
		}
		break;
	}

	case sf::Event::MouseMoved: {

		onMouseMove(event.mouseMove);
		break;
	}

	case sf::Event::MouseWheelScrolled:

		break;

	case sf::Event::MouseButtonReleased: {

		onMouseRelease(event.mouseButton);
		break;
	}

	case sf::Event::KeyPressed:
		onKeyPress(event.key);
		break;

	case sf::Event::KeyReleased:
		onKeyRelease(event.key);
		break;

	default:

		break;
	}

	SimulationUICommand cmd;
	while (UI.pollCommand(cmd)) {
		if (cmd == SimulationUICommand::RunSimulation) {
			runSimulation();
			continue;
		}
		
		dialogHandler.update(cmd, circuit);
	}
}

void SimulationController::onMousePress(const sf::Event::MouseButtonEvent& event) {
	if (event.button != sf::Mouse::Left) return;
	sf::Vector2f pixelPos(event.x, event.y);
	if (currentHandler) currentHandler->onMousePress(pixelPos);
	if (UI.simulationGraph.drawArea.contains(pixelPos)) {
		sf::Vector2f worldPos = UI.getWindow().mapPixelToCoords(sf::Vector2i(pixelPos), UI.simulationGraph.getView());
		Debug::printVector2f(worldPos);
		startSelectionPos = worldPos;
		startSelectionPixelPos = sf::Vector2i(pixelPos);
		dragging = true;
		UI.simulationGraph.dragging = dragging;

	}
	
}

void SimulationController::onMouseMove(const sf::Event::MouseMoveEvent& event) {
	sf::Vector2f pixelPos(event.x, event.y);
	if (currentHandler) currentHandler->onMouseMove(pixelPos);

	if (dragging) {
		sf::Vector2f worldPos = UI.getWindow().mapPixelToCoords(sf::Vector2i(pixelPos), UI.simulationGraph.getView());
		Debug::printVector2f(worldPos);
		endSelectionPos = worldPos;

		selectionRect.left = std::min(startSelectionPos.x, worldPos.x);
		selectionRect.top = std::min(startSelectionPos.y, worldPos.y);
		selectionRect.width = std::abs(worldPos.x - startSelectionPos.x);
		selectionRect.height = std::abs(worldPos.y - startSelectionPos.y);
		UI.simulationGraph.selectionRect = selectionRect;

	}
	
}

void SimulationController::onMouseRelease(const sf::Event::MouseButtonEvent& event) {
	if (event.button != sf::Mouse::Left) return;
	sf::Vector2f pixelPos(event.x, event.y);
	if (currentHandler) currentHandler->onMouseRelease(pixelPos);

	if (dragging) {
		sf::Vector2f worldPos = UI.getWindow().mapPixelToCoords(sf::Vector2i(pixelPos), UI.simulationGraph.getView());
		Debug::printVector2f(worldPos);
		endSelectionPos = worldPos;
		float pixelWidth = std::abs(event.x - startSelectionPixelPos.x);
		float pixelHeight = std::abs(event.y - startSelectionPixelPos.y);
		int minimumPixelSelection = 10;
		if (pixelWidth > minimumPixelSelection || pixelHeight > minimumPixelSelection) {
			selectionRect.left = std::min(startSelectionPos.x, worldPos.x);
			selectionRect.top = std::min(startSelectionPos.y, worldPos.y);
			selectionRect.width = std::abs(worldPos.x - startSelectionPos.x);
			selectionRect.height = std::abs(worldPos.y - startSelectionPos.y);

			UI.simulationGraph.setBounds(selectionRect.left, selectionRect.left + selectionRect.width, selectionRect.top + selectionRect.height, selectionRect.top);
			selectionRect = sf::FloatRect();
			UI.simulationGraph.selectionRect = selectionRect;

			
		}
		dragging = false;
		UI.simulationGraph.dragging = dragging;
	}
	
	
}

void SimulationController::onKeyPress(const sf::Event::KeyEvent& event) {
	if (currentHandler) currentHandler->onKeyPress(event);

}

void SimulationController::onKeyRelease(const sf::Event::KeyEvent& event) {
	//nothing so far
}

void SimulationController::onScroll(const sf::Event::MouseWheelScrollEvent& event) {
	//nothing so far, probably will add a scroll menu or something similar
	
}


void SimulationController::setHandler(InputHandler* handler, SimulationUICommand cmd) {
	currentHandler = handler;
	
}

void SimulationController::runSimulation() {
	std::string resultString = "";
	std::vector<Eigen::VectorXd> results;
	std::vector<double> timeVector;
	Simulator& sim = circuit.getSimulator();
	if (!sim.setSystem(circuit.getNetlistComponents(), circuit.getElectricalNodes())) return;
	
	TransientSimResults transientResults;
	std::vector<sf::Vector2f> data;

	switch (simType) {
	case SimulationType::DC:
		sim.runDC(true);
		for (auto [eNode, MNA] : sim.eNodeToMNA) {
			resultString += "eNode " + std::to_string(eNode) + " Voltage = " + std::to_string(sim.system.getx()(MNA)) + "\n";
		}
		for (auto extraVarInfo : sim.system.extraVars) {
			extraVarInfo.label += circuit.getNetlistComponent(extraVarInfo.componentID)->label + ")";
			resultString += extraVarInfo.label + " = " + std::to_string(sim.system.getx()(extraVarInfo.index)) + "\n";
		}

		UI.resultTextBox.setString(resultString);
		break;

	case SimulationType::Transient:
		if (!simParameters.transientValid()) {
			std::string invalidTransientString = "Simulation failed due to invalid parameters\n";
			std::cout << invalidTransientString;
			UI.resultTextBox.clear();
			UI.resultTextBox.appendString(invalidTransientString);
			return;
		}
		transientResults = sim.runTransient(simParameters);
		timeVector = transientResults.timeVector;
		results = transientResults.resultsVector;
		
		exportToCSV(results, timeVector, "simulation_results.csv");
		circuit.simulationResult = results;
		std::cout << "Time and Result Vectors \n\n";
		if (UI.graphDataY_VarMNAidx >= 0) {
			for (size_t i = 0; i < timeVector.size(); i++) {

				data.emplace_back(timeVector[i], results[i](UI.graphDataY_VarMNAidx));
				//std::cout << timeVector[i] << ", " << results[i](UI.graphDataY_VarMNAidx) << "\n";
			}
			std::cout << timeVector.size();

			UI.simulationGraph.setData(data);
			UI.simulationGraph.autoScale();
		}
		
		
		for (auto [eNode, MNA] : sim.eNodeToMNA) {
			resultString += "eNode " + std::to_string(eNode) + " Voltage = " + std::to_string(sim.system.getx()(MNA)) + "\n";
		}
		for (auto extraVarInfo : sim.system.extraVars) {
			extraVarInfo.label += circuit.getNetlistComponent(extraVarInfo.componentID)->label + ")";
			resultString += extraVarInfo.label + " = " + std::to_string(sim.system.getx()(extraVarInfo.index)) + "\n";
		}
		
		break;
	default:
		std::cout << "No sim type selected\n";
		UI.resultTextBox.clear();
		UI.resultTextBox.appendString("No sim type selected\n");
		break;
	}
}

bool SimulationController::parseValueWithSuffix(const std::string& input, double& outValue) {
	if (input.empty())
		return false;

	static const std::unordered_map<char, double> suffixMap = {
		{'p', 1e-12},
		{'n', 1e-9},
		{'u', 1e-6},
		{'m', 1e-3},
		{'k', 1e3},
		{'M', 1e6},
		{'G', 1e9}
	};

	char lastChar = input.back();
	double multiplier = 1.0;
	std::string numberString = input;

	// check for suffix
	if (std::isalpha(lastChar)) {
		auto it = suffixMap.find(lastChar);
		if (it == suffixMap.end())
			return false;

		multiplier = it->second;
		numberString.pop_back();
	}

	try {
		size_t idx;
		double baseValue = std::stod(numberString, &idx);

		// reject invalid strings
		if (idx != numberString.size())
			return false;

		outValue = baseValue * multiplier;
		return true;
	}
	catch (...) {
		return false;
	}
}

void SimulationController::exportToCSV(const std::vector<Eigen::VectorXd>& results, const std::vector<double> timeVector, const std::string& filename) {
	std::ofstream file(filename);

	if (!file.is_open()) {
		throw std::runtime_error("Cannot open file for writing");
	}

	for (size_t step = 0; step < results.size(); ++step) {
		const Eigen::VectorXd& v = results[step];
		file << timeVector[step]; //time column?
		file << ",";
		for (int i = 0; i < v.size(); ++i) {
			file << v[i];
			if (i < v.size() - 1) file << ",";
		}
		file << "\n";
	}

	file.close();
}