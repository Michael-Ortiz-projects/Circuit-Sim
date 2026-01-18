#include "DragHandler.h"

DragHandler::DragHandler(std::vector<SchematicComponent>& comps, std::unordered_map<int, Wire>& Wires)
	: components(comps), wires(Wires), dragging(false), active(nullptr) {}

void DragHandler::setDraggedComponent(Component& component) {
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
		if (active->A_WireNodeReference.isValid()) {
			wires.at(active->A_WireNodeReference.wireID).moveNode(active->A_WireNodeReference.nodeID, worldPos + active->leadOffsetA);
		}

		if (active->B_WireNodeReference.isValid()) {
			wires.at(active->B_WireNodeReference.wireID).moveNode(active->B_WireNodeReference.nodeID, worldPos + active->leadOffsetB);
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