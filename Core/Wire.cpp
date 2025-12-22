#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ initialPosition, {} });
	nextNodeID = 0;
}

int Wire::addNodeToWire(sf::Vector2f& worldPos) {
    graph[nextNodeID++].position = worldPos;
	return nextNodeID;
}