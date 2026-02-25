#pragma once
#include "Grid.h"
#include <unordered_map>
#include "DropdownMenu.h"
#include "../Core/Circuit.h"
#include <set>
#include "Renderer.h"

class EditorRenderer {
public:
    EditorRenderer(Renderer& r, Grid& Grid);

    void drawCanvas(std::vector<SchematicComponent>& components, std::unordered_map<int, Wire>& wires, sf::FloatRect selectionRect);

    void drawWireGraph(int nodeID, const std::map<int, Node>& graph, std::set<std::pair<int, int>>& drawnEdges);

    void drawUI(std::unordered_map<MenuID, DropdownMenu<EditorUICommand>> menu_map);

    sf::View& getCanvasView() { return r.getCanvasView(); }

    sf::View& getUIView() { return r.getUIView(); }

    sf::RenderWindow& getWindow() { return r.getWindow(); }

    AssetManager& getAssets() { return r.getAssets(); }

private:
    Renderer& r;
    Grid& grid;
};
