#include "SelectionBoxHandler.h"


SelectionBoxHandler::SelectionBoxHandler(std::vector<SchematicComponent>& comps, Circuit& circ, Selection& sel, bool& shift) 
	: components(comps), circuit(circ), selection(sel), shiftHeld(shift) { }

void SelectionBoxHandler::onMousePress(const sf::Vector2f& worldPos) {
    startPos = worldPos;
    dragging = true;
    released = false;
    selectionRect = { worldPos.x - 2.5f, worldPos.y - 2.5f, 5.f, 5.f };
    updateSelection();
}

void SelectionBoxHandler::onMouseMove(const sf::Vector2f& worldPos) {
    if (!dragging) return;

    selectionRect.left = std::min(startPos.x, worldPos.x);
    selectionRect.top = std::min(startPos.y, worldPos.y);
    selectionRect.width = std::abs(worldPos.x - startPos.x);
    selectionRect.height = std::abs(worldPos.y - startPos.y);
    updateSelection();
}

void SelectionBoxHandler::onMouseRelease(const sf::Vector2f& worldPos) {
    dragging = false;
    released = true;
    updateSelection();

    selectionRect = sf::FloatRect();
}

sf::FloatRect SelectionBoxHandler::computeRect() const {
    return {
        std::min(startPos.x, startPos.x + selectionRect.width),
        std::min(startPos.y, startPos.y + selectionRect.height),
        std::abs(selectionRect.width),
        std::abs(selectionRect.height)
    };
}

void SelectionBoxHandler::updateSelection() {
    if (!shiftHeld) {
        selection.clear();

        for (auto& comp : components) {
            auto* circuitComp = circuit.getComponent(comp.componentID);
            if (circuitComp) {
                circuitComp->selected = false;
            }
        }

        for (auto& [wireID, wire] : circuit.getWires()) {
            wire.unselect();
            for (auto& [nodeID, node] : wire.getGraph()) {
                node.selected = false;
            }
        }
    }
   
    for (auto& comp : components) {
        if (selectionRect.intersects(comp.getHitBox().getGlobalBounds())) {
            selection.componentIDs.insert(comp.componentID);
            circuit.getComponent(comp.componentID)->selected = true;
        }
    }

    for (auto& [wireID, wire] : circuit.getWires()) {
        for (const auto& [nodeID, node] : wire.getGraph()) {
            if (selectionRect.contains(node.position)) {
                WireNodeReference ref{ wireID, nodeID };
                selection.nodes.insert(ref);
                wire.selectNode(nodeID);
            }
        }
    }

    for (auto& [wireID, wire] : circuit.getWires()) {
        for (const auto& segment : wire.getSegments()) {
            if (segmentIntersectsRect(segment.start, segment.end, selectionRect)) {
                WireSegmentReference ref{ wireID, segment.nodeA, segment.nodeB };
                selection.segments.insert(ref);
                wire.getNode(segment.nodeA).selected = true;
                wire.getNode(segment.nodeB).selected = true;
            }
        }
    }
}