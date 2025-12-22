#pragma once
#include "SFML/Graphics.hpp"

struct Node {
	sf::Vector2f position;
	std::vector<int> neighbors;
};

class Wire {
public:

	Wire(sf::Vector2f initialPosition, int id);

	int addNodeToWire(sf::Vector2f& worldPosition);

	int ID;
	std::map<int, Node> graph;
	std::vector<int> anchorNodes;
	std::vector<int> junctionNodes;
	int nextNodeID;
};

