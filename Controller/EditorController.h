#pragma once
#include "../Core/circuit.h"
#include "DragHandler.h"
#include "PlaceHandler.h"
#include "WireHandler.h"
#include "SelectionBoxHandler.h"
#include "DeleteHandler.h"
#include "EditComponentHandler.h"
#include "SaveCircuitHandler.h"

#include "../Debug.h"
#include "../UI/Renderer.h"
#include "CameraController.h"
#include "../Geometry.h"
#include "../UI/ComponentInfoDisplay.h"

class InputHandler;




class EditorController {
public:
    DragHandler dragHandler;
    PlaceHandler placeHandler;
    WireHandler wireHandler;
    SelectionBoxHandler selectionBoxHandler;
    DeleteHandler deleteHandler;
    EditComponentHandler editComponentHandler;
    SaveCircuitHandler saveCircuitHandler;

    Selection selection;

    EditorController(Circuit& circ, AssetManager& Assets, Renderer& Renderer, EditorUI_Manager& UI, std::string& workingFilePath);

    void handleEvent(const sf::Event& event);

    void onMousePress(const sf::Event::MouseButtonEvent& event);

    void onMouseMove(const sf::Event::MouseMoveEvent& event);

    void onMouseRelease(const sf::Event::MouseButtonEvent& event);

    void onKeyPress(const sf::Event::KeyEvent& event);

    void onKeyRelease(const sf::Event::KeyEvent& event);

    void onScroll(const sf::Event::MouseWheelScrollEvent& event);

    void setHandler(InputHandler* handler, EditorUICommand cmd);

    void rebuildSchematicComponents();

    InputHandler* getHandler();

    HitResult hitTest(const sf::Vector2f& mousePixel);
    
    SchematicComponent* findComponentAt(const sf::Vector2f mousePixel);

    ElectricalConnection findClickedLead(const sf::Vector2f mousePixel);

    WireNodeReference findClickedNode(const sf::Vector2f mousePixel);

    WireHit findClickedSegment(const sf::Vector2f mousePixel);

private:
    InputHandler* currentHandler;

    CameraController cameraController;

    EditorUICommand command;
    Circuit& circuit;
    SaveManager saveManager;
    
    std::string& workingFilePath;
    sf::RenderWindow& window;
    Renderer& renderer;
    AssetManager& assets;
    bool shiftHeld = false;
};

