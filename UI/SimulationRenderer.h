#pragma once
#include "Grid.h"
#include <unordered_map>
#include "DropdownMenu.h"
#include "../Core/Circuit.h"
#include <set>
#include "Renderer.h"

class SimulationRenderer {
public:
    SimulationRenderer(Renderer& r);

    void drawUI(std::unordered_map<MenuID, DropdownMenu<SimulationUICommand>> menu_map);

    sf::View& getCanvasView() { return r.getCanvasView(); }

    sf::View& getUIView() { return r.getUIView(); }

    sf::RenderWindow& getWindow() { return r.getWindow(); }

    AssetManager& getAssets() { return r.getAssets(); }
private:

    Renderer& r;
};
