#pragma once
#include "SFML/Graphics.hpp"
#include "../UI/SchematicComponent.h"
#include "InputHandler.h"
#include <vector>

class DragHandler : public InputHandler {
public:
	DragHandler(std::vector<SchematicComponent>& comps);

	void setDraggedComponent(Component& component);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override;

private:
	std::vector<SchematicComponent>& components;
	bool dragging;
	Component* active;
};