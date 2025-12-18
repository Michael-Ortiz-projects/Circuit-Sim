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
#include "UI/AssetManager.h"
#include "UI/DropdownMenu.h"
#include "Debug.h"
#include "UI/Renderer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circuit Sim", sf::Style::None);
    Grid grid(gridSize);

    AssetManager assets;

    UI_Manager UI;
    UI.initialize(assets);
    Renderer renderer(window, assets, grid, UI);

    std::vector<SchematicComponent> schematic_components;

    Circuit circuit;
	CircuitSolver solver;
    Controller controller(circuit, schematic_components, window, UI, assets, renderer);

   



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
            if (cmd != UICommand::ToggleMenu && cmd != UICommand::None) {
                controller.setHandler(&controller.placeHandler, cmd);
                Debug::setHandler("PlaceHandler");
            }
        }

        renderer.drawCanvas(schematic_components);
        renderer.drawUI();

        window.display();
    }
    return 0;
}

/*
add a cursor and make wires
*/