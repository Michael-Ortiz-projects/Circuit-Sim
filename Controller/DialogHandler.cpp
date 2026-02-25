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
	TransientSimResults& transientSimResults = circuit.getSimulator().transientSimResults;

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
		graphDataDialogResult.MNAIndex = ui.activeDialog->getMenu(MenuID::GraphY_Axis).getSelectedValue();
		ui.graphDataY_VarMNAidx = graphDataDialogResult.MNAIndex;
		std::cout << "graphDataDialog result:\nMNAIndex = " << graphDataDialogResult.MNAIndex << "\n";

		

		for (size_t i = 0; i < transientSimResults.timeVector.size(); i++) {
			transientData.emplace_back(transientSimResults.timeVector[i], transientSimResults.resultsVector[i](ui.graphDataY_VarMNAidx));
			//std::cout << timeVector[i] << ", " << results[i](UI.graphDataY_VarMNAidx) << "\n";
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

