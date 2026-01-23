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


int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);
    Grid grid(gridSize);

    AssetManager assets;
    Circuit circuit;
    Wire wire(sf::Vector2f(520, 520), 0);
    
    std::vector<SchematicComponent> schematic_components;

    Renderer renderer(window, assets, grid);

    UI_Manager UI(renderer);
    
    Controller controller(circuit, schematic_components, window, assets, renderer, UI);

   

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
            
            switch (cmd) {
            case UICommand::PlaceVoltageSource:
                Debug::setHandler("PlaceHandler");
                controller.setHandler(&controller.placeHandler, cmd);
                break;
            case UICommand::PlaceResistor:
                Debug::setHandler("PlaceHandler");
                controller.setHandler(&controller.placeHandler, cmd);
                break;
            case UICommand::PlaceCurrentSource:
                Debug::setHandler("PlaceHandler");
                controller.setHandler(&controller.placeHandler, cmd);
                break;
            case UICommand::PlaceCapacitor:
                Debug::setHandler("PlaceHandler");
                controller.setHandler(&controller.placeHandler, cmd);
                break;
            case UICommand::PlaceInductor:
                Debug::setHandler("PlaceHandler");
                controller.setHandler(&controller.placeHandler, cmd);
                break;
            case UICommand::PlaceSwitch:
                Debug::setHandler("PlaceHandler");
                controller.setHandler(&controller.placeHandler, cmd);
                break;
            case UICommand::ToggleMenu:
                break;
            case UICommand::None:
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