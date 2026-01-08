#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include "../Config.h"
#include "ElectricalNode.h"
#include <unordered_set>
#include <array>

enum class Dir : uint8_t {
	Left = 0,
	Right,
	Up,
	Down
};

struct Neighbors {
	std::array<int, 4> id = { -1, -1, -1, -1 };

	int& operator[](Dir d) {
		return id[static_cast<int>(d)];
	}

	int operator[](Dir d) const {
		return id[static_cast<int>(d)];
	}

	bool has(Dir d) const {
		return (*this)[d] != -1;
	}

	void clear(Dir d) {
		(*this)[d] = -1;
	}
};

struct Node {
	int id;
	sf::Vector2f position;
	Neighbors neighbors;
	int belongsTo;
	bool isAnchor = false;
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

	void deleteNode(int nodeID);

	void collapseNodeInto(int keepID, int removeID);

	void updatePreview(sf::Vector2f pos);

	int commitPreview();

	SegmentHit projectOntoSegment(sf::Vector2f& point);

	void connectNodes(int a, int b);
	void connectNodes(Node& a, Node& b);
	void connectNodes(Node& a, Node& b, Dir dirAB);

	void disconnectNodes(int a, int b);
	void disconnectNodes(Node& a, Node& b);

	WireMoveResult moveNode(int nodeID, sf::Vector2f worldPosition, WireMoveIntent intent);

	std::map<int, Node>& getGraph() { return graph; }	
	int getNextNodeID() const { return nextNodeID; }
	int getStemNode() const { return currentStemNode; }
	sf::Vector2f getFirstPreview() const { return firstPreview; }
	sf::Vector2f getSecondPreview() const { return secondPreview; }
	sf::VertexArray getPreviewLine() const;


	bool isAnchor(int nodeID);
	bool isJunction(int nodeID);

private:
	void moveNodeSingleAxis(Node& primaryNode, MoveAxis axis, int newCoordinate, sf::Vector2f delta, WireMoveIntent& intent);

	int insertNodeBetween(Node& a, Node& b, sf::Vector2f position);

	bool cleanAllCoincidentNodes(Node& primaryNode);

	void mergeNodes(Node& keep, Node& remove);

	int appendNodeFromStem(sf::Vector2f pos);

	sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);	

	sf::Vector2f snapToGridBetween(sf::Vector2f A, sf::Vector2f B, sf::Vector2f point);

	float snapCoordinateToGrid(const float coordinate);

	Dir directionFrom(int a, int b);//direction from a to b
	Dir directionFrom(const Node& a, const Node& b);
	Dir directionFrom(sf::Vector2f A, sf::Vector2f B);

	Dir oppositeDirection(Dir d);
	std::string dirToString(Dir d) {
		switch (d) {
		case Dir::Left:
			return "Left";
		case Dir::Right:
			return "Right";
		case Dir::Up:
			return "Up";
		case Dir::Down:
			return "Down";
		}
	}
	void printNodeData(Node& node);
	void printGraphData();
	bool isOrthogonalTo(Dir d, Dir toCheck);
	float distanceBetween(sf::Vector2f a, sf::Vector2f b);

	std::map<int, Node> graph;
	int nextNodeID;
	sf::Vector2f firstPreview;
	sf::Vector2f secondPreview;
	int currentStemNode;
	PreviewOrientation previewOrientation;
	float axisTriggerDistance = 6.0f;
};

