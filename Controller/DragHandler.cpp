#include "DragHandler.h"

DragHandler::DragHandler(std::vector<SchematicComponent>& comps, std::unordered_map<int, Wire>& Wires)
	: components(comps), wires(Wires), dragging(false), active(nullptr) {}

void DragHandler::setDraggedComponent(SchematicComponent& component) {
	active = &component;
}

void DragHandler::onMousePress(const sf::Vector2f& worldPos)  {
	if (active) {
		active->startDrag(worldPos);
		dragging = true;		
	}
}

void DragHandler::onMouseMove(const sf::Vector2f& worldPos) {
	if (active && dragging) {
		active->dragTo(worldPos);
		for (SchematicTerminal& T : active->schematicTerminals) {
			if (T.wireNodeReference.isValid()) {
				wires.at(T.wireNodeReference.wireID).moveNode(T.wireNodeReference.nodeID, worldPos + T.offset);
			}
		}		
	}
}

void DragHandler::onMouseRelease(const sf::Vector2f& worldPos) {
	if (active && dragging) {
		active->stopDrag();
		dragging = false;
	}
}

bool DragHandler::shouldRelease() const {
	return true;
}