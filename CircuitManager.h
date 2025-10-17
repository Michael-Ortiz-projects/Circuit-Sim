#pragma once
#include "Entity.h"
#include "Wire.h"
#include <vector>
#include <unordered_map>

class CircuitManager {
public:
	std::unordered_map<int, Entity> entities;
	std::vector<Wire> wires;

	int nextEntityID = 0;
	Entity* draggedEntity = nullptr;

	void markForDeletion();
	void deleteSelected();
	void disconnectEntityFromWire(int wireIndex, int entityId, int port);
	void remapWireIndices();
	void eraseDeletedWires();
	void createEntityFromCursorState(CursorState& cursorState);

};

