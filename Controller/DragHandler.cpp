#include "DragHandler.h"

DragHandler::DragHandler(std::vector<SchematicComponent>& comps) 
	: components(comps), dragging(false), active(nullptr) {}

void DragHandler::setDraggedComponent(Component& component) {
	active = &component;
	std::cout << "Component Set\n";
}

void DragHandler::onMousePress(const sf::Vector2f& worldPos)  {
	if (active) {
		active->startDrag(worldPos);
		dragging = true;
		std::cout << "Starting drag\n";
	}
}

void DragHandler::onMouseMove(const sf::Vector2f& worldPos) {
	if (active && dragging) {
		active->dragTo(worldPos);
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