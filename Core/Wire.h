#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include "../Config.h"
#include "ElectricalNode.h"
enum class NodeType {
	Anchor,
	Junction,
	Intermediate
};

struct Node {
	sf::Vector2f position;
	std::vector<int> neighbors;
	int belongsTo;
};

enum class PreviewOrientation {
	None,
	HorizontalFirst,
	VerticalFirst
};


enum class WireMoveIntent {
	Edit,
	ComponentMove
};

class Wire {
public:

	Wire(sf::Vector2f initialPosition, int id);

	int appendNode(sf::Vector2f pos);

	void updatePreview(sf::Vector2f pos);

	void commitPreview();

	sf::VertexArray getPreviewLine() const;

	void moveNode(int movingNodeID, sf::Vector2f newPosition, WireMoveIntent intent);


	const std::map<int, Node>& getGraph() const { return graph; }	
	int getNextNodeID() const { return nextNodeID; }
	int getStemNode() const { return currentStemNode; }
	sf::Vector2f getFirstPreview() const { return firstPreview; }
	sf::Vector2f getSecondPreview() const { return secondPreview; }

	int ID;
	std::map<int, ElectricalConnection> anchorNodes;
	std::vector<int> junctionNodes;

	

	bool selected;

private:

	sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);
	bool isAnchor(int nodeID);

	sf::Vector2f getBendNodePosition(int movedID, int anchorID);
	void insertBendNodeBetween(int nodeA, int nodeB);

	void removeNeighborFrom(int nodeID, int to_remove);

	void updateNeighborPosition(int movedID, int neighborID, sf::Vector2f previousPosition);
	std::map<int, Node> graph;
	int nextNodeID;
	sf::Vector2f firstPreview;
	sf::Vector2f secondPreview;
	int currentStemNode;
	PreviewOrientation previewOrientation;
	float axisTriggerDistance = 6.0f;
};

