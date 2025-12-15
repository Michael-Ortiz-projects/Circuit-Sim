#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/circuit.h"
#include "DragHandler.h"
#include "CameraHandler.h"
#include "PlaceHandler.h"
#include "../UI/UI_Manager.h"
#include "../Debug.h"
class InputHandler;

class Controller {
public:
    DragHandler dragHandler;
    CameraHandler cameraHandler;
    PlaceHandler placeHandler;

    Controller(Circuit& circ, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, UI_Manager& UI, AssetManager& Assets);

    void onMousePress(const sf::Event::MouseButtonEvent& event);

    void onMouseMove(const sf::Event::MouseMoveEvent& event);

    void onScroll(const sf::Event::MouseWheelScrollEvent& event);

    void onMouseRelease(const sf::Event::MouseButtonEvent& event);

    void onKeyPress(const sf::Event::KeyEvent& event);

    void setHandler(InputHandler* handler, UICommand cmd);

    InputHandler* getHandler();

    SchematicComponent* findComponentAt(const sf::Vector2f point);

    sf::View getView();

private:
    InputHandler* currentHandler;
    
    UICommand command;

    Circuit& circuit;
    std::vector<SchematicComponent>& components;
    sf::RenderWindow& window;
    UI_Manager& ui;

};

