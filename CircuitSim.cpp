#include <iostream>
#include <cmath>
#include <functional>
#include "SFML/Graphics.hpp"
#include "Core/NetlistComponent.h"
#include "Core/Circuit.h"
#include "Controller/Controller.h"
#include "UI/Grid.h"
#include "Config.h"
#include "UI/AssetManager.h"
#include "UI/DropdownMenu.h"
#include "Debug.h"
#include "UI/Renderer.h"
#include "UI/UI_Manager.h"
#include <fstream>


// save button doesnt work, save as does though
// additionally: need to fix wiring more because trying to connect a wire to a component Terminal that already has a node doesnt work as it should
int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);

    std::string currentWorkingFilePath;

    Grid grid(gridSize);

    AssetManager assets;

    Circuit circuit(assets);

    Renderer renderer(window, assets, grid);

    UI_Manager UI(renderer);

    Controller controller(circuit, window, assets, renderer, UI, currentWorkingFilePath);
    //initializing circuit data
    char filename[MAX_PATH] = "TestingMNACircuit.ckt";
    CircuitData initializedData = controller.saveCircuitHandler.loadFromFile(filename);
    circuit.setCircuitData(initializedData);


    UI.initialize(assets);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            UI.handleEvent(event);
            controller.handleEvent(event);
            controller.rebuildSchematicComponents(event);
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
            case UICommand::PlaceGround:
                Debug::UICommand(cmd);
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
        

        renderer.drawCanvas(circuit.getSchematicComponents(), circuit.getWires(), controller.selectionBoxHandler.getRect());
        renderer.drawUI(UI.menu_map);
        UI.draw();
        window.display();
    }
    return 0;
}