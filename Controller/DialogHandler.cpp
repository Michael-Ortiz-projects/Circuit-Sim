#include "DialogHandler.h"

DialogHandler::DialogHandler(SimulationUI_Manager& ui, SimulationType& simType, Config& simulationParameters)
	: ui(ui), simType(simType), simParameters(simulationParameters) { }

void DialogHandler::onKeyPress(const sf::Event::KeyEvent& event) {
}

void DialogHandler::onMousePress(const sf::Vector2f&) {
}

void DialogHandler::onMouseRelease(const sf::Vector2f&) {
}

bool DialogHandler::shouldRelease() const {
	return release;
}

void DialogHandler::update(SimulationUICommand& cmd, Circuit& circuit) {
	EditSimulationDialogResult editSimulationDialogResult;
	GraphDataDialogResult graphDataDialogResult;
	GraphSettingsDialogResult graphSettingsDialogResult;
	std::vector<double> parsedValues;

	std::vector<sf::Vector2f> transientData;
	std::vector<TransientSimulationState>& transientSimResults = circuit.getSimulator().transientResults;
	std::vector<Eigen::VectorXd> results;
	std::vector<double> timeVector;

	auto& graphVars(circuit.getSimulator().graphVariables);
	int graphVarIdx;

	switch (cmd) {
	case SimulationUICommand::NewSimulation:
		ui.openNewSimulationDialog();

		break;
	case SimulationUICommand::CreateSimulation:
		ui.closeDialog();
		break;

	case SimulationUICommand::EditSimulation:
		ui.openEditSimulationDialog(simType, simParameters);
		break;
	case SimulationUICommand::ApplyEditSimulation:
		editSimulationDialogResult = ui.getEditSimulationDialogText(simType);
		switch (simType) {

		case SimulationType::Transient:
			parsedValues.resize(3);

			if (!parseValueWithSuffix(editSimulationDialogResult.timeStep_text, parsedValues[0])) {
				std::cout << "[SimulationController Edit Simulation Dialog] Invalid value: " << editSimulationDialogResult.timeStep_text << "\n";
				break;
			}

			if (!parseValueWithSuffix(editSimulationDialogResult.tStart_text, parsedValues[1])) {
				std::cout << "[SimulationController Edit Simulation Dialog] Invalid value: " << editSimulationDialogResult.tStart_text << "\n";
				break;
			}

			if (!parseValueWithSuffix(editSimulationDialogResult.tEnd_text, parsedValues[2])) {
				std::cout << "[SimulationController Edit Simulation Dialog] Invalid value: " << editSimulationDialogResult.tEnd_text << "\n";
				break;
			}

			simParameters.timeStep = parsedValues[0];
			simParameters.tStart = parsedValues[1];
			simParameters.tEnd = parsedValues[2];
			ui.closeDialog();

			break;
		default:
			break;
		}
		break;

	case SimulationUICommand::GraphData:
		ui.openGraphDataDialog(circuit);
		break;
	case SimulationUICommand::UpdateY_AxisMenu:
		ui.activeDialog->getMenu(MenuID::GraphY_Axis).setLabel(ui.activeDialog->getMenu(MenuID::GraphY_Axis).getOption(ui.activeDialog->getMenu(MenuID::GraphY_Axis).getSelectedValue()).button.getName());
		break;

	case SimulationUICommand::ApplyGraphData:
		graphVarIdx = ui.activeDialog->getMenu(MenuID::GraphY_Axis).getSelectedValue();
		ui.graphDataY_Varidx = graphVarIdx;
		graphDataDialogResult.graphVariableIndex = graphVarIdx;

		std::cout << "graphDataDialog result:\graphVariableIndex = " << graphVarIdx << "\n";

		timeVector.resize(transientSimResults.size());
		results.resize(transientSimResults.size());
		for (size_t t = 1; t < transientSimResults.size(); t++) {
			//printf("Transient Sim Results[%d]:\n", (t));
			//std::cout << "Previous Results Vector: " << transientSimResults[t].previousResultsVector << "\n\n";
			
			timeVector[t] = transientSimResults[t].time;
			results[t] = transientSimResults[t].resultsVector;
			transientSimResults[t].previousResultsVector = transientSimResults[t - 1].previousResultsVector;
			transientData.emplace_back(timeVector[t], graphVars[graphVarIdx].evaluator(transientSimResults[t]));
		}

		ui.simulationGraph.setData(transientData);
		ui.simulationGraph.autoScale();
		
		ui.closeDialog();
		break;

	case SimulationUICommand::GraphSettings:
		ui.openGraphSettingsDialog();
		break;
	case SimulationUICommand::AutoScaleGraph:
		if (!ui.simulationGraph.dataEmpty())
			ui.simulationGraph.autoScale();
		ui.closeDialog();
		break;
	case SimulationUICommand::ApplyGraphSettings:
		parsedValues.resize(4);

		graphSettingsDialogResult = ui.getGraphSettingsDialogText();
		
		if (!parseValueWithSuffix(graphSettingsDialogResult.minX, parsedValues[0]) ||
			!parseValueWithSuffix(graphSettingsDialogResult.maxX, parsedValues[1]) ||
			!parseValueWithSuffix(graphSettingsDialogResult.minY, parsedValues[2]) ||
			!parseValueWithSuffix(graphSettingsDialogResult.maxY, parsedValues[3])) {
			std::cout << "[SimulationController Graph Settings Dialog] Invalid value: " << graphSettingsDialogResult.minX << ", " << graphSettingsDialogResult.maxX <<
					", " << graphSettingsDialogResult.minY << ", " << graphSettingsDialogResult.maxY << "\n";
			break;
		}

		ui.simulationGraph.setBounds(parsedValues[0], parsedValues[1], parsedValues[2], parsedValues[3]);

		ui.closeDialog();
		break;

	case SimulationUICommand::CancelCreateSimulation:
	case SimulationUICommand::CancelEditSimulation:
	case SimulationUICommand::CancelGraphData:
	case SimulationUICommand::CancelGraphSettings:
		ui.closeDialog();
		break;

	case SimulationUICommand::ToggleMenu:
		break;
	case SimulationUICommand::DCOP:
		ui.activeDialog->setMenuLabel(MenuID::SimulationType, "DC Operating Point");
		simType = SimulationType::DC;
		break;
	case SimulationUICommand::TRAN:
		ui.activeDialog->setMenuLabel(MenuID::SimulationType, "Transient");
		simType = SimulationType::Transient;
		break;
	default:
		break;

	}
}

bool DialogHandler::parseValueWithSuffix(const std::string& input, double& outValue) {
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

