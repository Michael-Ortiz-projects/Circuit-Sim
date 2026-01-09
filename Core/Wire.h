#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include "../Config.h"
#include "ElectricalNode.h"
#include <unordered_set>

struct Node {
	int id;
	sf::Vector2f position;
	std::unordered_set<int> neighbors;
	int belongsTo;
	bool isAnchor = false;
	bool selected = false;
};

enum class PreviewOrientation {
	None,
	HorizontalFirst,
	VerticalFirst
};

enum class WireMoveResult {
	Valid,
	Invalid,
	NodeRemoved
};

struct SegmentHit {
	int nodeA = -1;
	int nodeB = -1;
	sf::Vector2f snappedPosition { };
	bool valid = false;

	bool isValid() const { return valid; }
};

struct WireHit {
	int wireID = -1;
	SegmentHit segment;
	float distance = FLT_MAX;
	bool valid = false;
};

class Wire {
public:
	int ID;
	bool selected;
	
	Wire(sf::Vector2f initialPosition, int id);

	int createNode(sf::Vector2f pos);

	void removeNode(int nodeID);

	void updatePreview(sf::Vector2f pos);

	int appendNodeFromStem(sf::Vector2f pos);

	int commitPreview();

	WireMoveResult moveNode(int movingNodeID, sf::Vector2f worldPosition);

	bool mergeNodes(int primaryID, int merging);
	bool mergeCollinearAtNode(int nodeID);
	bool mergeCoincidentNodes(int nodeID);

	void unselect();

	SegmentHit projectOntoSegment(sf::Vector2f& point);



	std::map<int, Node>& getGraph() { return graph; }	
	Node& getNode(int nodeID) { if (graph.contains(nodeID)) return graph.at(nodeID); }
	int getNextNodeID() const { return nextNodeID; }
	int getStemNode() const { return currentStemNode; }
	sf::Vector2f getFirstPreview() const { return firstPreview; }
	sf::Vector2f getSecondPreview() const { return secondPreview; }
	sf::VertexArray getPreviewLine() const;


	bool isAnchor(int nodeID);
	bool connectNodes(int nodeA, int nodeB);
	bool disconnectNodes(int nodeA, int nodeB);
private:

	

	sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);	
	sf::Vector2f snapToGridBetween(sf::Vector2f A, sf::Vector2f B, sf::Vector2f point);
	float snapCoordinateToGrid(const float coordinate);
	float distanceBetween(sf::Vector2f a, sf::Vector2f b);
	bool areCollinear(sf::Vector2f& A, sf::Vector2f& B, sf::Vector2f& C);

	void printNodeData(Node& node);
	void printGraphData();
	std::map<int, Node> graph;
	int nextNodeID;
	sf::Vector2f firstPreview;
	sf::Vector2f secondPreview;
	int currentStemNode;
	PreviewOrientation previewOrientation;
	float axisTriggerDistance = 6.0f;
};

