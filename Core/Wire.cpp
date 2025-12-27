#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ initialPosition, {}, id });
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
    sf::Vector2f position = snapPositionToGrid(pos);
    sf::Vector2f stemPos = graph.at(currentStemNode).position;
    sf::Vector2f delta = position - stemPos;

    bool CrossedX = std::abs(delta.x) > axisTriggerDistance;
    bool CrossedY = std::abs(delta.y) > axisTriggerDistance;

    bool insideBox = !CrossedX && !CrossedY;

    if (insideBox) {
        previewOrientation = PreviewOrientation::None;
        firstPreview = pos;
        secondPreview = pos;
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
        firstPreview = { position.x, stemPos.y };
        secondPreview = position;
    }
    else if (previewOrientation == PreviewOrientation::VerticalFirst) {
        firstPreview = { stemPos.x, position.y };
        secondPreview = position;
    }
}

void Wire::commitPreview() {
    if (firstPreview != graph[currentStemNode].position)
        appendNode(firstPreview);

    if (secondPreview != graph[currentStemNode].position)
        appendNode(secondPreview);
    previewOrientation = PreviewOrientation::None;
}

void Wire::moveNode(int movingNodeID, sf::Vector2f newPosition, WireMoveIntent intent) { // fix this PLEASE node movement moves other nodes incorrectly
    Node& movingNode = graph.at(movingNodeID);

    sf::Vector2f prevPos(movingNode.position);


    if (isAnchor(movingNodeID)) {
        if (intent == WireMoveIntent::Edit) {
            std::cout << "Failed to move Anchor Node " << movingNodeID << "in wire : " << ID << "\n";
            return;
        }
        if (intent == WireMoveIntent::ComponentMove) {
            movingNode.position = snapPositionToGrid(newPosition);
        }
    }
    else if (intent == WireMoveIntent::Edit) {
        movingNode.position = snapPositionToGrid(newPosition);
    }
    std::vector<int> neighborIDs = movingNode.neighbors;

    for (int neighborID : neighborIDs) {
        if (isAnchor(neighborID) && (intent == WireMoveIntent::Edit || intent == WireMoveIntent::ComponentMove)) {
            sf::Vector2f position(getBendNodePosition(movingNodeID, neighborID));
            if (position.x != graph.at(neighborID).position.x || position.y != graph.at(neighborID).position.y) {
                insertBendNodeBetween(movingNodeID, neighborID);
            }
        }

        else {
            updateNeighborPosition(movingNodeID, neighborID, prevPos);
        }
    }
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

sf::Vector2f Wire::snapPositionToGrid(const sf::Vector2f& position) {
    return {
        std::round(position.x / gridSize) * gridSize,
        std::round(position.y / gridSize) * gridSize
    };
}

bool Wire::isAnchor(int nodeID) {
    return anchorNodes.find(nodeID) != anchorNodes.end();
}

sf::Vector2f Wire::getBendNodePosition(int movedID, int anchorID) {
    Node& moved = graph.at(movedID);
    Node& anchor = graph.at(anchorID);

    bool horizontalMove =
        std::abs(moved.position.y - anchor.position.y) <=
        std::abs(moved.position.x - anchor.position.x);

    sf::Vector2f result;
    if (horizontalMove) {
        result = { moved.position.x, anchor.position.y };
    }
    else {
        result = { anchor.position.x, moved.position.y };
    }
    return result;
}

void Wire::insertBendNodeBetween(int nodeA, int nodeB) {
    Node& A = graph.at(nodeA);
    Node& B = graph.at(nodeB);

    int newID = nextNodeID++;

    sf::Vector2f newPosition(getBendNodePosition(nodeA, nodeB));

    graph[newID] = Node{ newPosition, { nodeA, nodeB}, ID };

    removeNeighborFrom(nodeA, nodeB);
    A.neighbors.push_back(newID);

    removeNeighborFrom(nodeB, nodeA);
    B.neighbors.push_back(newID);
    
}

void Wire::removeNeighborFrom(int nodeID, int to_remove) {
    std::vector<int>& neighbors = graph.at(nodeID).neighbors;
    neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), to_remove), neighbors.end());
}

void Wire::updateNeighborPosition(int movedID, int neighborID, sf::Vector2f previousPosition) {
    Node& moved = graph.at(movedID);
    Node& neighbor = graph.at(neighborID);

    if (isAnchor(neighborID))
        return;

    sf::Vector2f movementDelta = moved.position - previousPosition;

    if (movementDelta == sf::Vector2f(0.f, 0.f))
        return;

    if (std::abs(movementDelta.x) > std::abs(movementDelta.y))
    {
        neighbor.position.y = moved.position.y;
    }
    else if (std::abs(movementDelta.y) > std::abs(movementDelta.x))
    {
        neighbor.position.x = moved.position.x;
    }
}