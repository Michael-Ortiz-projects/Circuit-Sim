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

enum class MoveAxis {
	Horizontal,
	Vertical
};

enum class WireMoveResult {
	None,
	NodeMerged,
	NodeRemoved
};

class Wire {
public:

	Wire(sf::Vector2f initialPosition, int id);

	int appendNode(sf::Vector2f pos);

	void updatePreview(sf::Vector2f pos);

	int commitPreview();

	sf::VertexArray getPreviewLine() const;

	WireMoveResult moveNode(int movingNodeID, sf::Vector2f newPosition, WireMoveIntent intent);

	void moveNodeSingleAxis(int movingNodeID, const sf::Vector2f& newGridPosition, MoveAxis axis, WireMoveIntent intent);

	bool collapseCoincidentNodes(int nodeID);

	void cleanupCollinearNodes();

	void removeCollinearNode(int nodeID);


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
	bool isCollinear(int nodeID);

	sf::Vector2f getBendNodePosition(sf::Vector2f newMovingNodePosition, int anchorID, bool horizontalMove);
	void insertBendNodeBetween(int nodeA, int nodeB, bool horizontalMove, sf::Vector2f newMovingNodePosition);

	void removeNeighborFrom(int nodeID, int to_remove);

	void updateNeighborPosition(int movingID, int neighborID, bool horizontalMove, sf::Vector2f newMovingNodePosition);
	std::map<int, Node> graph;
	int nextNodeID;
	sf::Vector2f firstPreview;
	sf::Vector2f secondPreview;
	int currentStemNode;
	PreviewOrientation previewOrientation;
	float axisTriggerDistance = 6.0f;
};

