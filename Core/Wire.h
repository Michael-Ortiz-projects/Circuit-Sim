#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>

struct Node {
	sf::Vector2f position;
	std::vector<int> neighbors;
};

enum class PreviewOrientation {
	None,
	HorizontalFirst,
	VerticalFirst
};

class Wire {
public:

	Wire(sf::Vector2f initialPosition, int id);

	int appendNode(sf::Vector2f pos);

	void updatePreview(sf::Vector2f pos);

	void commitPreview();

	sf::VertexArray getPreviewLine() const;


	const std::map<int, Node>& getGraph() const { return graph; }	
	int getNextNodeID() const { return nextNodeID; }
	int getStemNode() const { return currentStemNode; }
	sf::Vector2f getFirstPreview() const { return firstPreview; }
	sf::Vector2f getSecondPreview() const { return secondPreview; }

	int ID;
	std::vector<int> anchorNodes;
	std::vector<int> junctionNodes;

	bool selected;

private:
	std::map<int, Node> graph;
	int nextNodeID;
	sf::Vector2f firstPreview;
	sf::Vector2f secondPreview;
	int currentStemNode;
	PreviewOrientation previewOrientation;
	float axisTriggerDistance = 6.0f;
};

