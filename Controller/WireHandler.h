#pragma once
#include "SFML/Graphics.hpp"
#include "InputHandler.h"
#include "../Core/Circuit.h"
#include "../UI/SchematicComponent.h"
#include <vector>
#include "../Config.h"

enum class WireState {
	Creating,
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
	bool invalid() const { return !hasLead() && !hasWireNode(); }
};

struct WireHit {
	int wireID = -1;
	SegmentHit segment;
	float distance = FLT_MAX;
	bool valid = false;
};

struct HitResult {
	enum class Type {
		None,
		Lead,
		WireNode,
		WireSegment,
		Component
	} type = Type::None;

	ElectricalConnection lead;
	WireNodeReference wireNode;
	WireHit wireSegment;
	Component* component = nullptr;
	bool shiftHeld;

	bool isNone() const { return type == Type::None; }
};

class WireHandler : public InputHandler {
public:
	WireHandler(Circuit& Circuit, std::vector<SchematicComponent>& components);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override;


	void beginWireFromConnection(ElectricalConnection& connection);

	void handleWireCreationClick(const sf::Vector2f& worldPos);

	void finishWireAtConnection(ElectricalConnection& end);
	void finishWireAtNode(WireNodeReference wire_node);
	void finishWireAtSegment(WireHit wireSegment);


	void mergeActiveWireIntoPrimary(Wire& primaryWire, Wire& activeWire);

	void setHitResult(const HitResult& h);

	WireState getState() { return wireState; }
private:

	sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);
	sf::Vector2f positionOfConnection(ElectricalConnection& connection);
	void beginNodeDrag(WireNodeReference& ref);


	Circuit& circuit;
	std::vector<SchematicComponent>& schematicComponents;

	Wire* activeWire;
	WireState wireState = WireState::Null;

	HitResult hit;

	int activeElectricalNodeID = -1;
};