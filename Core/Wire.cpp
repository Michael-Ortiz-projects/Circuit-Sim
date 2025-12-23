#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ initialPosition, {} });
	nextNodeID = 1;
    currentStemNode = 0;
}

int Wire::appendNode(sf::Vector2f pos)
{
    int id = nextNodeID++;

    graph[id].position = pos;

    graph[currentStemNode].neighbors.push_back(id);
    graph[id].neighbors.push_back(currentStemNode);

    currentStemNode = id;
    return id;
}

void Wire::updatePreview(sf::Vector2f pos, bool invert) {
    sf::Vector2f stem = graph[currentStemNode].position;

    if (!invert)
        firstPreview = { pos.x, stem.y };
    else
        firstPreview = { stem.x, pos.y };

    secondPreview = pos;

    previewValid = true;

}

void Wire::commitPreview() {
    if (firstPreview != graph[currentStemNode].position)
        appendNode(firstPreview);

    if (secondPreview != graph[currentStemNode].position)
        appendNode(secondPreview);
    previewValid = false;
}


sf::VertexArray Wire::getPreviewLine() const
{
    sf::VertexArray line(sf::Lines, 4);

    line[0].position = graph.at(currentStemNode).position;
    line[1].position = firstPreview;
    line[2].position = firstPreview;
    line[3].position = secondPreview;

    sf::Color previewColor(76, 141, 255, 140);
    for (int i = 0; i < 4; ++i) line[i].color = previewColor;

    return line;
}