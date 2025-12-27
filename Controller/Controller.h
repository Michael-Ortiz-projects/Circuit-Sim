#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/circuit.h"
#include "DragHandler.h"
#include "PlaceHandler.h"
#include "WireHandler.h"
#include "../Debug.h"
#include "../UI/Renderer.h"
#include "CameraController.h"
class InputHandler;

class Controller {
public:
    DragHandler dragHandler;
    PlaceHandler placeHandler;
    WireHandler wireHandler;

    Controller(Circuit& circ, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, AssetManager& Assets, Renderer& Renderer);

    void handleEvent(const sf::Event& event);

    void onMousePress(const sf::Event::MouseButtonEvent& event);

    void onMouseMove(const sf::Event::MouseMoveEvent& event);

    void onMouseRelease(const sf::Event::MouseButtonEvent& event);

    void onKeyPress(const sf::Event::KeyEvent& event);

    void onScroll(const sf::Event::MouseWheelScrollEvent& event);

    void setHandler(InputHandler* handler, UICommand cmd);

    void rebuildSchematicComponents();

    InputHandler* getHandler();

    Component* findComponentAt(const sf::Vector2f point);

    ElectricalConnection findClickedLead(const sf::Vector2f point);

    WireNodeReference findClickedNode(const sf::Vector2f point);

private:
    InputHandler* currentHandler;

    CameraController cameraController;

    UICommand command;
    Circuit& circuit;
    std::vector<SchematicComponent>& components;
    
    sf::RenderWindow& window;
    Renderer& renderer;
    AssetManager& assets;
};

