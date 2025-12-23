#pragma once
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include <vector>
#include <string>
#include "SchematicComponent.h"
#include "AssetManager.h"
#include <unordered_map>
#include "DropdownMenu.h"
#include "../Core/Wire.h"

class Renderer {
public:
    Renderer(sf::RenderWindow& Window, AssetManager& Assets, Grid& Grid);

    void drawCanvas(std::vector<SchematicComponent>& components, std::unordered_map<int, Wire>& wires);

    void drawWireGraph(int nodeID, const std::map<int, Node>& graph, std::map<int, bool>& visited);


    void drawUI(std::unordered_map<MenuID, DropdownMenu> menu_map);

    sf::View& getCanvasView();

    sf::View& getUIView();
private:


    AssetManager& assets;
    sf::RenderWindow& window;
    Grid& grid;
    sf::View canvasView;
    sf::View UIView;
};
