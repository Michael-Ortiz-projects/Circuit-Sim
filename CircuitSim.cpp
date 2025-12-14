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
#include "UI/Camera.h"
#include "UI/ComponentRenderer.h"
#include "UI/AssetManager.h"
#include "UI/DropdownMenu.h"
int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);

    AssetManager assets;

    UI_Manager UI;
    UI.initialize(assets);
    ComponentRenderer renderer(assets);

    Component VoltageSource(1, 0, ComponentType::VoltageSource, 9);
    std::vector<SchematicComponent> schematic_components = { SchematicComponent(&VoltageSource, sf::Vector2f(20, 200), 0), SchematicComponent(&VoltageSource, sf::Vector2f(30, 600), 0) };

    Circuit circuit;
	CircuitSolver solver;
    Controller controller(circuit, schematic_components, window, UI);

    Grid grid(30.0f);

    
   

    
    //put this into a UI manager that the controller queries for events.

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                controller.onKeyPress(event.key);
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));


            if (event.type == sf::Event::MouseButtonPressed) {
                if (UI.onMousePress(mousePos)) continue;
                controller.onMousePress(event.mouseButton);

            }
            
            if (event.type == sf::Event::MouseMoved) {
                if (UI.onMouseMove(mousePos)) continue;
                controller.onMouseMove(event.mouseMove);
            }

            if (event.type == sf::Event::MouseButtonReleased) {

                if (UI.onMouseRelease(mousePos)) continue;
                controller.onMouseRelease(event.mouseButton);
                
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                controller.onScroll(event.mouseWheelScroll);
            }
        }

        UICommand cmd;
        if (UI.pollCommand(cmd)) {
            if (cmd == UICommand::PlaceVoltageSource || cmd == UICommand::PlaceResistor)
                std::cout << "place handler set\n";
                //controller.setHandler() make handler for placing components
        }
        grid.draw(window, controller.getView());

        renderer.drawComponents(window, schematic_components);
        
        UI.draw(window);

        window.display();
    }
    return 0;
}
