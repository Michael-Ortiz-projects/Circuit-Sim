#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ initialPosition, {} });
	nextNodeID = 1;
    currentStemNode = 0;
    previewOrientation = PreviewOrientation::None;
}

int Wire::appendNode(sf::Vector2f pos) {
    int id = nextNodeID++;

    graph[id].position = pos;

    graph[currentStemNode].neighbors.push_back(id);
    graph[id].neighbors.push_back(currentStemNode);

    currentStemNode = id;
    return id;
}

void Wire::updatePreview(sf::Vector2f pos) {
    sf::Vector2f stemPos = graph.at(currentStemNode).position;
    sf::Vector2f delta = pos - stemPos;

    bool CrossedX = std::abs(delta.x) > axisTriggerDistance;
    bool CrossedY = std::abs(delta.y) > axisTriggerDistance;

    bool insideBox = !CrossedX && !CrossedY;

    if (insideBox) {
        previewOrientation = PreviewOrientation::None;
        return;
    }

    if (previewOrientation == PreviewOrientation::None) {
        if (CrossedX && !CrossedY) {
            previewOrientation = PreviewOrientation::HorizontalFirst;
        }
        else if (CrossedY && !CrossedX) {
            previewOrientation = PreviewOrientation::VerticalFirst;
        }
        else {
            return;
        }
    }

    if (previewOrientation == PreviewOrientation::HorizontalFirst){
        firstPreview = { pos.x, stemPos.y };
        secondPreview = pos;
    }
    else if (previewOrientation == PreviewOrientation::VerticalFirst) {
        firstPreview = { stemPos.x, pos.y };
        secondPreview = pos;
    }
}

void Wire::commitPreview() {
    if (firstPreview != graph[currentStemNode].position)
        appendNode(firstPreview);

    if (secondPreview != graph[currentStemNode].position)
        appendNode(secondPreview);
    previewOrientation = PreviewOrientation::None;
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