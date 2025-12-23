#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ initialPosition, {} });
	nextNodeID = 1;
    currentStemNode = 0;
}

int Wire::addNewNodeToWire(sf::Vector2f worldPos) {
    int newID = nextNodeID++;

    // create new node
    graph[newID].position = worldPos;
    // connect both ways
    graph[currentStemNode].neighbors.push_back(newID);
    graph[newID].neighbors.push_back(currentStemNode);
    currentStemNode = newID;

    return newID;
}