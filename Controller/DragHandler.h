#pragma once
#include "SFML/Graphics.hpp"
#include "../UI/SchematicComponent.h"
#include "InputHandler.h"
#include <vector>

class DragHandler : public InputHandler {
public:
	DragHandler(std::vector<SchematicComponent>& comps, std::unordered_map<int, Wire>& Wires);

	void setDraggedComponent(SchematicComponent& component);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override;

private:
	std::vector<SchematicComponent>& components;
	bool dragging;
	SchematicComponent* active;
	std::unordered_map<int, Wire>& wires;
	
};