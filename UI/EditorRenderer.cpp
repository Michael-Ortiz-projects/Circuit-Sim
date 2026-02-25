#include "EditorRenderer.h"

using Edge = std::pair<int, int>;

EditorRenderer::EditorRenderer(Renderer& r, Grid& Grid)
    : r(r), grid(Grid) { }

void EditorRenderer::drawCanvas(std::vector<SchematicComponent>& components, std::unordered_map<int, Wire>& wires, sf::FloatRect selectionRect) {
    auto& window = r.getWindow();
    window.setView(r.getCanvasView());
    grid.draw(window, r.getCanvasView());
    for (auto& c : components) {
        window.draw(c.getSprite());
        if (c.getType() != ComponentType::Ground) 
            c.getComponentInfoDisplay().draw(window);
        if (c.selected)
            window.draw(c.getHitBox());
    }

    for (auto& wire : wires) {
        std::set<std::pair<int, int>> drawnEdges;
        const auto& graph = wire.second.getGraph();

        if (!graph.empty()) {
            drawWireGraph(graph.begin()->first, graph, drawnEdges);
            for (const auto& [id, node] : wire.second.getGraph()) {
                if (wire.second.getGraph().at(id).neighbors.size() > 2) {
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
    sf::RectangleShape selectionBox(selectionRect.getSize());
    selectionBox.setFillColor(sf::Color::Transparent);
    selectionBox.setPosition(selectionRect.getPosition());
    selectionBox.setOutlineThickness(1);
    selectionBox.setOutlineColor(sf::Color::Yellow);
    window.draw(selectionBox);
}

void EditorRenderer::drawUI(std::unordered_map<MenuID, DropdownMenu<EditorUICommand>> menu_map) {
    r.getWindow().setView(r.getUIView());
    for (auto& m : menu_map) {
        m.second.draw(r.getWindow());
    }
}

void EditorRenderer::drawWireGraph(int nodeID, const std::map<int, Node>& graph, std::set<std::pair<int, int>>& drawnEdges) {
    const Node& node = graph.at(nodeID);

    for (int neighborID : node.neighbors) {
        if (neighborID == -1) continue;

        int a = std::min(nodeID, neighborID);
        int b = std::max(nodeID, neighborID);

        if (drawnEdges.count({ a, b }))
            continue;

        drawnEdges.insert({ a, b });

        const Node& neighbor = graph.at(neighborID);
        sf::Color edgeColor;

        if (node.selected && neighbor.selected) edgeColor = sf::Color(89, 190, 194);
        else edgeColor = wireDefaultColor;

        sf::Vertex line[] = {
            sf::Vertex(node.position, edgeColor),
            sf::Vertex(neighbor.position, edgeColor)
        };

        
        r.getWindow().draw(line, 2, sf::Lines);

        drawWireGraph(neighborID, graph, drawnEdges);
    }
}
