#pragma once
#include "SFML/Graphics.hpp"
#include "InputHandler.h"
#include "../Core/Circuit.h"
#include "../UI/SchematicComponent.h"
#include <vector>
#include "../Config.h"

enum class WireState {
	Creating,
	Editing,
	DraggingNode,
	Null
};

struct Vector2fCompare {
	bool operator()(const sf::Vector2f& a, const sf::Vector2f& b) const {
		if (a.x != b.x) return a.x < b.x;
		return a.y < b.y;
	}
};


struct WireInteraction {
	ElectricalConnection connection{ -1, Lead::Null };
	WireNodeReference wire_node{ -1, -1 };

	bool hasLead() const { return connection.lead != Lead::Null; }
	bool hasWireNode() const { return wire_node.isValid(); }
};

class WireHandler : public InputHandler {
public:
	WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override;


	void beginWireFromConnection(ElectricalConnection& connection);

	void finishWireAtConnection(ElectricalConnection& end);

	void beginNodeDrag(WireNodeReference& ref);

	void finishWireAtSegment(WireHit& wireSegment);

	void setInteractionContext(WireInteraction context);
	void setSegmentContext(WireHit context);
private:

	sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);
	sf::Vector2f positionOfConnection(ElectricalConnection& connection);

	Circuit& circuit;
	std::vector<SchematicComponent>& schematicComponents;

	Wire* activeWire;
	WireState wireState = WireState::Null;

	WireInteraction interaction;
	WireHit wireSegment;

	int activeElectricalNodeID = -1;
};