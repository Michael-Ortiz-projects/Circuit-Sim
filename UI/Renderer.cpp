#include "Renderer.h"

using Edge = std::pair<int, int>;


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
        std::set<std::pair<int, int>> drawnEdges;
        const auto& graph = wire.second.getGraph();

        if (!graph.empty()) {
            drawWireGraph(graph.begin()->first, graph, drawnEdges);
            for (const auto& [id, node] : wire.second.getGraph()) {
                if (wire.second.isJunction(id)) {
                    sf::CircleShape junctionPoint;
                    junctionPoint.setFillColor(sf::Color(63, 182, 168));
                    junctionPoint.setRadius(2.5);
                    junctionPoint.setOrigin({ 2.5, 2.5 });
                    junctionPoint.setPosition(node.position);
                    window.draw(junctionPoint);
                }
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

void Renderer::drawWireGraph(int nodeID, const std::map<int, Node>& graph, std::set<std::pair<int, int>>& drawnEdges) {
    const Node& node = graph.at(nodeID);

    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        int neighborID = node.neighbors[d];
        if (neighborID == -1) continue;

        int a = std::min(nodeID, neighborID);
        int b = std::max(nodeID, neighborID);

        if (drawnEdges.count({ a, b }))
            continue;

        drawnEdges.insert({ a, b });

        const Node& neighbor = graph.at(neighborID);

        sf::Vertex line[] = {
            sf::Vertex(node.position, sf::Color(154, 159, 166, 255)),
            sf::Vertex(neighbor.position, sf::Color(154, 159, 166, 255))
        };
        window.draw(line, 2, sf::Lines);

        drawWireGraph(neighborID, graph, drawnEdges);
    }
}
