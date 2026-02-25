#pragma once
#include "InputHandler.h"
#include "../Geometry.h"
#include <algorithm>
#include "../Core/Circuit.h"

class SelectionBoxHandler : public InputHandler {

public:
	sf::Vector2f startPos;
	sf::FloatRect selectionRect;
	bool& shiftHeld;
	Selection& selection;
	Circuit& circuit;
	SelectionBoxHandler(std::vector<SchematicComponent>& comps, Circuit& circ, Selection& sel, bool& shift);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override { return released; }

	void updateSelection(sf::Vector2f position);

	sf::FloatRect& getRect() { return selectionRect; }
private:


	bool dragging = false;
	bool released = false;

	sf::FloatRect computeRect() const;
};