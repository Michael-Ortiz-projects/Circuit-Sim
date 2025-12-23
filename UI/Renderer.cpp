#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow& Window, AssetManager& Assets, Grid& Grid)
    : assets(Assets), window(Window), grid(Grid) {
    sf::Vector2f windowSize(window.getSize());

    canvasView.setSize(windowSize);
    canvasView.setCenter(windowSize * 0.5f);
    canvasView.zoom(defaultZoom);

    UIView.setSize(windowSize);
    UIView.setCenter(windowSize * 0.5f);
}

void Renderer::drawCanvas(std::vector<SchematicComponent>& components, std::unordered_map<int, Wire>& wires) {
    window.setView(canvasView);
    grid.draw(window, canvasView);
    for (auto& c : components) {
        window.draw(c.getSprite());
        if (c.selected)
            window.draw(c.getHitBox());
    }

    for (auto& wire : wires) {
        std::map<int, bool> visited;
        if (!wire.second.graph.empty()) {
            drawWireGraph(wire.second.graph.begin()->first, wire.second.graph, visited);
        }
    }
}

void Renderer::drawUI(std::unordered_map<MenuID, DropdownMenu> menu_map) {
    window.setView(UIView);
    for (auto& m : menu_map) {
        m.second.draw(window);
    }
}

sf::View& Renderer::getCanvasView() {
    return canvasView;
}

sf::View& Renderer::getUIView() {
    return UIView;
}

void Renderer::drawWireGraph(int nodeID, std::map<int, Node>& graph, std::map<int, bool>& visited) {
    visited[nodeID] = true;

    for (int neighborID : graph[nodeID].neighbors) {
        if (!visited[neighborID]) {
            sf::Vertex line[] = {
                            sf::Vertex(graph[nodeID].position, sf::Color::White),
                            sf::Vertex(graph[neighborID].position, sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);

            drawWireGraph(neighborID, graph, visited);
        }
    }
}