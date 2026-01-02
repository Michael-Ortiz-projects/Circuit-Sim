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
        const auto& graph = wire.second.getGraph();

        if (!graph.empty()) {
            drawWireGraph(graph.begin()->first, graph, visited);
            for (const auto& nodeID : wire.second.junctionNodes) {
                sf::CircleShape junctionPoint;
                junctionPoint.setFillColor(sf::Color(63, 182, 168));
                junctionPoint.setRadius(2.5);
                junctionPoint.setOrigin({ 2.5, 2.5 });
                junctionPoint.setPosition(graph.at(nodeID).position);
                window.draw(junctionPoint);
            }
        }

        if (wire.second.selected) {
            window.draw(wire.second.getPreviewLine());
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

void Renderer::drawWireGraph(int nodeID, const std::map<int, Node>& graph, std::map<int, bool>& visited) {
    visited[nodeID] = true;

    for (int neighborID : graph.at(nodeID).neighbors) {
        if (!visited[neighborID]) {
            sf::Vertex line[] = {
                            sf::Vertex(graph.at(nodeID).position, sf::Color(154, 159, 166, 255)),
                            sf::Vertex(graph.at(neighborID).position, sf::Color(154, 159, 166, 255))
            };
            window.draw(line, 2, sf::Lines);

            drawWireGraph(neighborID, graph, visited);
        }
    }
}