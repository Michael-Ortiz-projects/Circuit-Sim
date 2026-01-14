#pragma once
#include "../Core/Circuit.h"
#include "InputHandler.h"

class DeleteHandler : public InputHandler {
public:
	DeleteHandler(Circuit& c, Selection& s);

	void onKeyPress(const sf::Event::KeyEvent& event) override;
	bool shouldRelease() const override;

	void deleteSelection();
	void deleteComponents(std::unordered_set<int> component_IDs);
	void deleteSegments(std::unordered_set<WireSegmentReference> segments);
	void deleteNodes(std::unordered_set<WireNodeReference> nodes);
	void deleteWires(std::unordered_set<int> wire_IDs);


	Selection simplifySelection();

private:
	Circuit& circuit;
	Selection& selection;
	bool release = false;
};