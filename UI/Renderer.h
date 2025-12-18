#pragma once
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include <vector>
#include <string>
#include "SchematicComponent.h"
#include "AssetManager.h"
#include "UI_Manager.h"

class Renderer {
public:
    Renderer(sf::RenderWindow& Window, AssetManager& Assets, Grid& Grid, UI_Manager& Manager);

    void drawCanvas(std::vector<SchematicComponent>& components);

    void drawUI();

    sf::View& getCanvasView();

    sf::View& getUIView();
private:
    AssetManager& assets;
    sf::RenderWindow& window;
    Grid& grid;
    UI_Manager& uiManager;
    sf::View canvasView;
    sf::View UIView;
};
