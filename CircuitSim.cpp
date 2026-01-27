#include <iostream>
#include <cmath>
#include <functional>
#include "SFML/Graphics.hpp"
#include "Core/Component.h"
#include "Core/Circuit.h"
#include "Core/CircuitSolver.h"
#include "Controller/Controller.h"
#include "UI/Grid.h"
#include "Config.h"
#include "UI/AssetManager.h"
#include "UI/DropdownMenu.h"
#include "Debug.h"
#include "UI/Renderer.h"
#include "UI/UI_Manager.h"
#include <fstream>

//WORK ON CIRCUIT SIMULATION

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);
    Grid grid(gridSize);

    AssetManager assets;
    Circuit circuit;
    
    
    std::string currentWorkingFilePath;
    std::vector<SchematicComponent> schematic_components;

    Renderer renderer(window, assets, grid);

    UI_Manager UI(renderer);
    
    Controller controller(circuit, schematic_components, window, assets, renderer, UI, currentWorkingFilePath);

    UI.initialize(assets);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            UI.handleEvent(event);
            controller.handleEvent(event);
            controller.rebuildSchematicComponents();
        }

        UICommand cmd;

        while (UI.pollCommand(cmd)) {
            CircuitData data;
            switch (cmd) {
            case UICommand::PlaceVoltageSource:
            case UICommand::PlaceResistor:
            case UICommand::PlaceCurrentSource:
            case UICommand::PlaceCapacitor:
            case UICommand::PlaceInductor:
            case UICommand::PlaceSwitch:
                Debug::setHandler("PlaceHandler");
                controller.setHandler(&controller.placeHandler, cmd);
                break;

            case UICommand::OpenNewFile:
                circuit.setCircuitData(data);
                currentWorkingFilePath.clear();
                break;
            case UICommand::OpenFile:
                controller.saveCircuitHandler.loadDialog();
                break;
            case UICommand::SaveFile:
                if (currentWorkingFilePath.empty())
                    controller.saveCircuitHandler.saveDialog();
                else {
                    controller.saveCircuitHandler.saveCurrentWorkingFile();
                }
                break;
            case UICommand::SaveFileAs:
                controller.saveCircuitHandler.saveDialog();
                break;
            case UICommand::ExitProgram:
                window.close();
                break;
            default:
                break;
            }
        }
        

        renderer.drawCanvas(schematic_components, circuit.getWires(), controller.selectionBoxHandler.getRect());
        renderer.drawUI(UI.menu_map);
        UI.draw();
        window.display();
    }
    return 0;
}