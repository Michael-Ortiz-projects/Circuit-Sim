#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/circuit.h"
#include "DragHandler.h"
#include "CameraHandler.h"
#include "../UI/UI_Manager.h"
class InputHandler;

class Controller {
public:
    Controller(Circuit& circ, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, UI_Manager& UI);

    void onMousePress(const sf::Event::MouseButtonEvent& event);

    void onMouseMove(const sf::Event::MouseMoveEvent& event);

    void onScroll(const sf::Event::MouseWheelScrollEvent& event);

    void onMouseRelease(const sf::Event::MouseButtonEvent& event);

    void onKeyPress(const sf::Event::KeyEvent& event);

    void setHandler(InputHandler* handler);

    SchematicComponent* findComponentAt(const sf::Vector2f point);

    sf::View getView();

private:
    InputHandler* currentHandler;
    DragHandler dragHandler;
    CameraHandler cameraHandler;

    Circuit& circuit;
    std::vector<SchematicComponent>& components;
    sf::RenderWindow& window;
    UI_Manager& ui;

};

