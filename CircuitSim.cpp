#include <iostream>
#include <vector>
#include "SFML/Graphics.hpp"
#include "HelperFunctions.h"
#include "global_variables.h"
#include "Component.h"
#include "TextBox.h"
#include "Button.h"
#include "Circuit.h"
#include "Wire.h"
#include "Entity.h"
#include <Eigen/Dense>
#include "AssetManager.h"
#include "Oscilloscope.h"
#include "CircuitManager.h"

int main()
{
    mainFont.loadFromFile("fonts/NotoSans-Regular.ttf");
    sf::RenderWindow window(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "");
    bool simulating = false;
    float simSpeed = 0.1f;
    float timeDiv = 0.01f;
    int sampleSize = 100;
    Oscilloscope scope(0, 60, timeDiv, sampleSize, mainFont);
    Component* selectedComponent = nullptr;

    CircuitManager circuitManager;

    sf::Clock clock;
    float dt = 0;
    float simTime = 0;
    float realTime = 0;
    Circuit c;
    
    sf::View view(sf::FloatRect(0.f, 0.f, SCR_WIDTH, SCR_HEIGHT));
    view.setCenter(SCR_WIDTH / 2, SCR_HEIGHT / 2);
    view.setSize(SCR_WIDTH, -SCR_HEIGHT);

    window.setView(view);


    std::vector<Button> buttons;
    std::unordered_map<int, Entity>& entities = circuitManager.entities;
    std::vector<Wire>& wires = circuitManager.wires;
    int nextEntityID = 0;

    CursorState cursorState = CursorState::Default;
    Entity*& draggedEntity = circuitManager.draggedEntity;
    bool dragging = false;

    buttons.emplace_back(Button(sf::Vector2f(100, 1200), mainFont, "Resistor", [&]() {
        cursorState = CursorState::CreatingResistor;
        //std::cout << "Resistor Button Pressed\n";
    }));

    buttons.emplace_back(Button(sf::Vector2f(250, 1200), mainFont, "Voltage Source", [&]() {
        cursorState = CursorState::CreatingVoltageSource;
        //std::cout << "Voltage Source Button Pressed\n";
    }));

    buttons.emplace_back(Button(sf::Vector2f(500, 1200), mainFont, "Current Source", [&]() {
        cursorState = CursorState::CreatingCurrentSource;
        //std::cout << "Current Source Button Pressed\n";
    }));
    
    buttons.emplace_back(Button(sf::Vector2f(800, 1200), mainFont, "Capacitor", [&]() {
        cursorState = CursorState::CreatingCapacitor;
        //std::cout << "Capacitor Button Pressed\n";
    }));
    buttons.emplace_back(Button(sf::Vector2f(1100, 1200), mainFont, "Inductor", [&]() {
        cursorState = CursorState::CreatingInductor;
        //std::cout << "Inductor Button Pressed\n";
    }));
    buttons.emplace_back(Button(sf::Vector2f(1500, 1200), mainFont, "Switch", [&]() {
        cursorState = CursorState::CreatingSwitch;
        //std::cout << "Switch Button Pressed\n";
    }));

    TextBox detailBox(sf::Vector2f(2300, 1400), mainFont);
    detailBox.text.setCharacterSize(20);
    detailBox.text.setString("");
    detailBox.setTextBoundingBox();
    detailBox.box.setFillColor(sf::Color(57, 62, 70));

    sf::CircleShape snap_circle(5);
    snap_circle.setFillColor(sf::Color::Red);
    snap_circle.setOrigin(5, 5);

    bool zKeytoggle = false;
    std::pair<int, int> selected_wire_node = { -1, -1 };
    sf::Vector2f lastSnappedPos = { -1, -1 };

    

    while (window.isOpen() || scope.window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sf::Vector2f snappedPos = snapToGrid(mousePosition); //GRID POSITION
            bool CursorMoved = snappedPos != lastSnappedPos;
            snap_circle.setPosition(snapToGrid(mousePosition)); //CURSOR CIRCLE

            
            //HANDLE KEYBOARD INPUTS
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::Closed) {
                sf::Keyboard::Key keyCode = event.key.code;

                //CLOSE WINDOW
                if (keyCode == sf::Keyboard::Escape || event.type == sf::Event::Closed)
                    window.close(); 

                //INVERT WIRE
                if(keyCode == sf::Keyboard::Z)
                    zKeytoggle = !zKeytoggle; 

                //CANCEL WIRE CREATION
                if (keyCode == sf::Keyboard::Space && cursorState == CursorState::PlacingWire && !wires.empty()) {
                    bool toNode = wires.back().connected_entity_ids.back().second;
                    toNode ? entities[wires.back().connected_entity_ids.back().first].component.toNode = -1 :
                             entities[wires.back().connected_entity_ids.back().first].component.fromNode = -1;
                    wires.back().cancel();
                    wires.pop_back();
                    cursorState = CursorState::Default;
                }

                //PRINT SOFTWARE DATA/CONNECTIONS
                if (keyCode == sf::Keyboard::D) {
                    std::cout << "\nNEW DATA\n";
                    for (const auto& [id, entity] : entities) {
                        std::cout << "\n---------------\nEntity#" << id << "\n";
                        entities[id].component.printData(detailBox, false);
                    }
                    for (int i = 0; i < wires.size(); i++) {
                        std::cout << "\n---------------\nWire #" << i << "\n";
                        wires[i].printData(detailBox, false);
                    }
                    //std::cout << "Selected Wire " << selected_wire_node.first << " Node " << selected_wire_node.second << "\n";
                }

                if (keyCode == sf::Keyboard::Delete) {

                    // Mark wires for deletion and disconnect entities
                    circuitManager.markForDeletion();
                    simulating = false;
                    
                }
                    

                //SOLVE CIRCUIT
                if (keyCode == sf::Keyboard::Q && !entities.empty()) {
                    
                    if (!simulating) {
                        for (auto& [id, entity] : entities) {
                            if (entity.component.fromNode != -1 && entity.component.toNode != -1)
                                c.AddComponent(entity.component);

                            simulating = true;
                            realTime = 0;
                        }
                        c.ConfigureCircuit();
                    }
                    else simulating = false;

                }
                
                //ROTATE COMPONENT
                if (keyCode == sf::Keyboard::R ) {
                    c.ResetCircuit();
                    scope.window.clear(sf::Color::Black);
                    scope.drawGrid();
                    scope.drawLabels();
                    scope.window.display();
                    scope.signal.clear();
                    scope.signal.resize(sampleSize, 0.0f);
                    if (simulating) {
                        for (auto& [id, entity] : entities) {
                            if (entity.component.fromNode != -1 && entity.component.toNode != -1)
                                c.AddComponent(entity.component);
                            realTime = 0;
                        }
                        c.ConfigureCircuit();
                    }
                    //std::cout << "\nRESET CIRCUIT\n";

                    for (auto& [id, entity] : entities) {
                        if (entity.selected && (!entity.IDBox.selecting || !entity.ValueBox.selecting)) {
                            entity.setPosition(entity.getPosition(), wires, 90);
                            //std::cout << "\nROTATED COMPONENT\n";
                        }
                    }
                    
                }
                for (auto& [id, entity] : entities) {
                    if (entity.component.type == ComponentType::Switch) {
                        if (keyCode == entity.component.controlKey) {
                            entity.component.isClosed = !entity.component.isClosed;
                            if (simulating) c.ConfigureCircuit();
                            //std::cout << "Control Key Pressed\n";
                        }
                    }
                }
                
                
            }

            
            //HANDLE BUTTON EVENTS
            bool clicked_UI = false;
            for (auto& button : buttons) { 
                if (button.handleEvent(event, window)) {
                    clicked_UI = true;
                }                
            }
            
            if (!wires.empty()) wires.back().UpdatePreview(snappedPos, zKeytoggle);
            // MOUSE CLICK LOGIC, DEALING WITH ENTITIES, WIRES, COMPONENTS
            if (event.type == sf::Event::MouseButtonPressed && (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)) {
                //std::cout << "MousePosition (" << snappedPos.x << ", " << snappedPos.y << ")\n";
                
                //HANDLE ENTITY MOUSE EVENTS

                for (auto& [id, entity] : entities) {
                    entity.handleMouseEvents(cursorState, wires, entities, mousePosition, id);
                    entity.selected = false;
                    //std::cout << entity.contains(mousePosition);
                    sf::FloatRect bounds = entity.selection_box.getGlobalBounds();
                    std::cout << bounds.left << ", " << bounds.top << ", " << bounds.width << ", " << bounds.height;
                    std::cout << ", " << mousePosition.x << ", " << mousePosition.y << std::endl;
                    if (entity.contains(mousePosition) && cursorState == CursorState::Default) {
                        draggedEntity = &entity;
                        entity.selected = true;
                        selectedComponent = &entity.component;
                        scope.setTarget(selectedComponent);
                        entity.component.printData(detailBox, true);
                        cursorState = CursorState::SelectingComponent;
                    }
                }


                selected_wire_node = { -1, -1 };
                for (int i = 0; i < wires.size() && !wires.empty(); i++) {

                    wires[i].selected = false;
                    if (wires[i].isPointNearWire(snappedPos, 15.0f)) {
                        wires[i].selected = true;
                        wires[i].printData(detailBox, true);
                    }

                    std::pair<bool, int> foundNode = wires[i].findClickedNode(snappedPos);
                    
                    if (wires[i].selected && foundNode.first && cursorState != CursorState::PlacingWire) {
                        selected_wire_node = { i, foundNode.second };
                        cursorState = CursorState::SelectingNode;
                        break;
                    }

                }

                //WIRE PLACEMENT
                if (cursorState == CursorState::PlacingWire && !clicked_UI) {             
                    if (snappedPos != wires.back().points.back()) {
                        bool clickedOnSeparateWire = false;
                        

                        for (int i = 0; i < wires.size(); i++) {
                            std::optional<std::pair<int, int>> insertionPoints = wires[i].findClickedSegment(snappedPos, 15.0f);
                            
                            if (insertionPoints.has_value()) {
                                std::cout << "Clicked on another wire\n";
                                auto [a, b] = insertionPoints.value();
                                wires.back().AddFullPreview();
                                wires.back().editing = false;                                
                                wires[i].insertWireBetween(a, b, wires.back());
                                wires.pop_back();
                                cursorState = CursorState::Default;

                                for (const auto& pair : wires[i].connected_entity_ids) {
                                    pair.second ? entities[pair.first].component.toNode = i : entities[pair.first].component.fromNode = i;
                                }
                                clickedOnSeparateWire = true;
                                break;
                            }
                        }

                        if (!clickedOnSeparateWire) {
                            //std::cout << "Clicked on separate wire\n";
                            wires.back().AddFullPreview();
                        }
                    }
                }

                //COMPONENT CREATION

                circuitManager.createEntityFromCursorState(cursorState);
            }
            
            

            //COMPONENT TEXTBOX EDITING
            for (auto& [id, entity] : entities) {
                entity.handleTextInput(event, mousePosition, cursorState);
            }
            
            

            //DRAGGING COMPONENTS
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (cursorState == CursorState::PlacingResistor || cursorState == CursorState::SelectingComponent ||
                                                                cursorState == CursorState::PlacingVoltageSource || cursorState == CursorState::SelectingNode ||
                                                                cursorState == CursorState::PlacingCurrentSource || cursorState == CursorState::PlacingCapacitor ||
                                                                cursorState == CursorState::PlacingInductor      || cursorState == CursorState::PlacingSwitch)) {

                if (draggedEntity && CursorMoved && entities.count(draggedEntity->uniqueID)) {
                    draggedEntity->setPosition(snappedPos, wires, 0);
                    //std::cout << "dragged entity position = (" << draggedEntity->position.x << ", " << draggedEntity->position.y << ")" << std::endl;
                    lastSnappedPos = snappedPos;
                }
                if (cursorState == CursorState::SelectingNode && CursorMoved) {
                    if (selected_wire_node.first != -1) {
                        Wire& wire = wires[selected_wire_node.first];
                        int& node = selected_wire_node.second;
                        if (std::find(wire.anchorNodes.begin(), wire.anchorNodes.end(), node) == wire.anchorNodes.end()) {
                            wire.moveNode(selected_wire_node.second, snappedPos);
                            wire.updateWire(selected_wire_node, cursorState);
                        }
                    }
                    lastSnappedPos = snappedPos;
                }
            }
            

            if (event.type == sf::Event::MouseButtonReleased && cursorState != CursorState::PlacingWire && cursorState != CursorState::CreatingWire) {
                draggedEntity = nullptr;
                cursorState = CursorState::Default;
                if (selected_wire_node != std::pair<int, int> {-1, -1}) {
                    selected_wire_node = { -1, -1 };
                }
            }
        }

        while (scope.window.pollEvent(event)) {
            if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed) {
                scope.window.close();
                std::cout << "\nSCOPE CLOSED\n";
            }
        }

        sf::Time frameTime = clock.restart();
        std::cout << frameTime.asSeconds() << std::endl;
        dt += frameTime.asSeconds();
        realTime += frameTime.asSeconds();

        if (simulating && dt >= timeDiv) {
            dt = 0;
            float timeStep = timeDiv * simSpeed;
            simTime += timeStep;
            c.SolveCircuit(timeStep);
            scope.update();
            

        }
        

        if (draggedEntity && draggedEntity->deleted) {
            draggedEntity = nullptr;
        }

        if (event.key.code == sf::Keyboard::Delete) {
            circuitManager.deleteSelected();
            c.ConfigureCircuit();
        }
        
        // DRAWING SECTION
        window.clear(clearColor);
        drawGrid(window);
        
        for (auto button : buttons) {
            button.draw(window);
        }

        if (!wires.empty()) {
            for (int i = 0; i < wires.size(); i++) {
                wires[i].draw(window);
            }
        }

        for (auto [id, entity] : entities) {
            entity.draw(window);
        }
        window.draw(snap_circle);
        detailBox.draw(window);
        window.display();

        
    }    
    return 0;
}
