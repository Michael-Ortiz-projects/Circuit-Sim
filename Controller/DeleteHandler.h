#pragma once
#include "../Core/Circuit.h"
#include "InputHandler.h"
#include <stack>
#include "../debug.h"
using WireSection = std::vector<int>; // old node IDs

struct ComponentAttachment {
	int componentID;
	Lead lead;
	int oldNodeID;
};

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

	void updateWires();
	void dfsSection(int start, const std::map<int, Node>& graph, std::unordered_set<int>& visited, WireSection& out);
	std::vector<WireSection> findWireSections(Wire& wire);
	void rebuildWireFromSection(WireSection& section, std::vector<ComponentAttachment>& attachments, std::map<int, Node> graph);

private:
	Circuit& circuit;
	Selection& selection;
	std::unordered_set<int> modifiedWires;
	bool release = false;
};