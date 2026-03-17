#pragma once
#include "DropdownMenu.h"
#include "../Debug.h"
#include "Renderer.h"
#include <vector>
#include <optional>
#include "../Config.h"
#include "Dialog.h"
#include <sstream>
#include <iomanip>
#include "../Core/SimComps/SimulationComponent.h"
#include "../core/Circuit.h"
#include "Graph.h"
struct Config;

class SimulationUI_Manager {
public:
	std::unordered_map<MenuID, DropdownMenu<SimulationUICommand>> menu_map;
	std::unordered_map<SimulationUICommand, Button<SimulationUICommand>> button_map;
	ScrollTextBox resultTextBox;
	Graph simulationGraph;
	sf::FloatRect graphRect;
	std::unique_ptr<Dialog<SimulationUICommand>> activeDialog;
	int graphDataY_Varidx = -1;

	SimulationUI_Manager(Renderer& rend);

	void initialize();

	bool handleEvent(const sf::Event& event);

	bool pollCommand(SimulationUICommand& outputCommand);

	bool onMousePress(const sf::Vector2f& pixelPos);

	bool onMouseMove(const sf::Vector2f& pixelPos);

	bool onMouseRelease(const sf::Vector2f& pixelPos);

	void openNewSimulationDialog();

	void openEditSimulationDialog(SimulationType type, Config simParam);

	void openGraphSettingsDialog();

	void openGraphDataDialog(Circuit& circuit);

	void closeDialog() { activeDialog.reset(); }

	EditSimulationDialogResult getEditSimulationDialogText(SimulationType type);

	GraphSettingsDialogResult getGraphSettingsDialogText();

	sf::RenderWindow& getWindow() { return renderer.getWindow(); }

	void draw();

private:
	Renderer& renderer;

	bool parseValueWithSuffix(const std::string& input, double& outValue);

	std::string formatValueWithSuffix(double value);

};