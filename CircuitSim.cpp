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
#include "UI/Camera.h"
#include "UI/ComponentRenderer.h"
#include "UI/AssetManager.h"
#include "UI/DropdownMenu.h"
#include "Debug.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);

    AssetManager assets;

    UI_Manager UI;
    UI.initialize(assets);
    ComponentRenderer renderer(assets);

    Component VoltageSource(1, 0, ComponentType::VoltageSource, 9);
    std::vector<SchematicComponent> schematic_components;

    Circuit circuit;
	CircuitSolver solver;
    Controller controller(circuit, schematic_components, window, UI, assets);

    Grid grid(30.0f);
    
    

    

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
            Debug::UICommand(cmd);
            if (cmd == UICommand::PlaceVoltageSource || cmd == UICommand::PlaceResistor){
                controller.setHandler(&controller.placeHandler, cmd);
                Debug::setHandler("PlaceHandler");
            }

        }
        grid.draw(window, controller.getView());

        renderer.drawComponents(window, schematic_components);
        
        UI.draw(window);

        window.display();
    }
    return 0;
}


/*
    To do (not in any particular order)

    Make debugging for circuit topography, just details of everything
    Maybe clean up component placement, right now when you place a component
     -  The origin isnt set when you put it down and it isnt grid snapped until you move it

    Start working on wires, I want a separate wire class that I can put a lot of the merging/creation logic in
        decide whether the wires are UI or controller logic, I feel like its both the visual parts, and the core components connected to it



*/