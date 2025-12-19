#pragma once
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include <vector>
#include <string>
#include "SchematicComponent.h"
#include "AssetManager.h"
#include <unordered_map>
#include "DropdownMenu.h"

class Renderer {
public:
    Renderer(sf::RenderWindow& Window, AssetManager& Assets, Grid& Grid);

    void drawCanvas(std::vector<SchematicComponent>& components);

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
