#include <iostream>
#include <cmath>
#include <functional>
#include <format>
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

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);
    Grid grid(gridSize);

    AssetManager assets;
    Circuit circuit;
    std::vector<SchematicComponent> schematic_components;

    Renderer renderer(window, assets, grid);

    Controller controller(circuit, schematic_components, window, assets, renderer);

    UI_Manager UI(controller);

    UI.initialize(assets);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();


            if (!UI.handleEvent(event)) {  // only pass to controller if UI ignores it
                controller.handleEvent(event);
            }

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
        

        renderer.drawCanvas(schematic_components);
        renderer.drawUI(UI.menu_map);

        window.display();
    }
    return 0;
}

/*
maybe add a cursor, not totally necessary|| make wires: i already made the selection hitbox, next i need to detect clicking the leads (use node selection radius in config)
                                            make a wire class i think, definitely make a wire handler 
*/