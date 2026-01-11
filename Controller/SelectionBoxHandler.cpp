#include "SelectionBoxHandler.h"


SelectionBoxHandler::SelectionBoxHandler(std::vector<SchematicComponent>& comps, Circuit& circ, Selection& sel, bool& shift) 
	: components(comps), circuit(circ), selection(sel), shiftHeld(shift) { }

void SelectionBoxHandler::onMousePress(const sf::Vector2f& worldPos) {
    startPos = worldPos;
    dragging = true;
    released = false;
    selectionRect = { worldPos.x, worldPos.y, 0.f, 0.f };
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
    selectionRect = computeRect();
    updateSelection();
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
        // Clear Selection struct
        selection.clear();

        // Unselect all components
        for (auto& comp : components) {
            auto* circuitComp = circuit.getComponent(comp.componentID);
            if (circuitComp) circuitComp->selected = false;
        }

        // Unselect all wires and their nodes
        for (auto& [wireID, wire] : circuit.getWires()) {
            wire.unselect(); // this should reset wire.selected
            for (auto& [nodeID, node] : wire.getGraph()) {
                node.selected = false;
            }
        }
    }

    // --- Components ---
    for (auto& comp : components) {
        if (selectionRect.intersects(comp.getHitBox().getGlobalBounds())) {
            selection.componentIDs.insert(comp.componentID);
            circuit.getComponent(comp.componentID)->selected = true;
        }
    }

    // --- Wire nodes ---
    for (auto& [wireID, wire] : circuit.getWires()) {
        for (const auto& [nodeID, node] : wire.getGraph()) {
            if (selectionRect.contains(node.position)) {
                WireNodeReference ref{ wireID, nodeID };
                selection.nodes.insert(ref);
                wire.selectNode(nodeID);
            }
        }
    }

    // --- Wire segments ---
    for (auto& [wireID, wire] : circuit.getWires()) {
        for (const auto& segment : wire.getSegments()) {
            if (segmentIntersectsRect(segment.start, segment.end, selectionRect)) {
                WireSegmentReference ref{ wireID, segment.nodeA, segment.nodeB };
                selection.segments.insert(ref);
            }
        }
    }
}