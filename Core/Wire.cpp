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

int Wire::commitPreview() {
    int committedNode = -1;
    if (firstPreview != graph[currentStemNode].position) {
        committedNode = appendNode(firstPreview);
    }

    if (secondPreview != graph[currentStemNode].position) {
        committedNode = appendNode(secondPreview);
    }
    previewOrientation = PreviewOrientation::None;
    return committedNode;
}

void Wire::moveNode(int movingNodeID, sf::Vector2f newPosition, WireMoveIntent intent) { // I need to determine node movement direction and insert nodes based on that
                                                                                         // when i move a node diagonally fast it messes up, probably because the movement delta
    Node& movingNode = graph.at(movingNodeID);

    sf::Vector2f newGridPosition = snapPositionToGrid(newPosition);
    if (newGridPosition == movingNode.position) return;

    
    std::vector<int> neighborIDs = movingNode.neighbors;

    sf::Vector2f movementDelta = newPosition - movingNode.position;
    bool horizontalMovement = std::abs(movementDelta.x) > std::abs(movementDelta.y);
    std::cout << "Horizontal Movement: " << horizontalMovement << std::endl;
    for (int neighborID : neighborIDs) {
        if (isAnchor(neighborID) && (intent == WireMoveIntent::Edit || intent == WireMoveIntent::ComponentMove)) {
            

            sf::Vector2f bendPosition(getBendNodePosition(newGridPosition, neighborID, horizontalMovement));
            if (horizontalMovement && graph.at(neighborID).position.x == movingNode.position.x) {
                insertBendNodeBetween(movingNodeID, neighborID, horizontalMovement, newGridPosition);
                std::cout << "(" + std::to_string(bendPosition.x) + ", " + std::to_string(bendPosition.y) + ")\n";
            }
            else if (!horizontalMovement && graph.at(neighborID).position.y == movingNode.position.y) {
                insertBendNodeBetween(movingNodeID, neighborID, horizontalMovement, newGridPosition);
                std::cout << "(" + std::to_string(bendPosition.x) + ", " + std::to_string(bendPosition.y) + ")\n";
            }
        }

        else {
            updateNeighborPosition(movingNodeID, neighborID, horizontalMovement, newGridPosition);
            std::cout << "Updated Neighbor position\n";
        }
    }







    if (isAnchor(movingNodeID)) {
        if (intent == WireMoveIntent::Edit) {
            std::cout << "Failed to move Anchor Node " << movingNodeID << "in wire : " << ID << "\n";
            return;
        }
        if (intent == WireMoveIntent::ComponentMove) {
            movingNode.position = newGridPosition;
        }
    }
    else if (intent == WireMoveIntent::Edit) {
        movingNode.position = newGridPosition;
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

sf::Vector2f Wire::getBendNodePosition(sf::Vector2f newMovingNodePosition, int anchorID, bool horizontalMove) {
    Node& anchor = graph.at(anchorID);

    sf::Vector2f result;
    if (horizontalMove) {
        result = { newMovingNodePosition.x, anchor.position.y };
    }
    else {
        result = { anchor.position.x, newMovingNodePosition.y };
    }
    return result;
}

void Wire::insertBendNodeBetween(int nodeA, int nodeB, bool horizontalMove, sf::Vector2f newMovingNodePosition) {
    Node& A = graph.at(nodeA);
    Node& B = graph.at(nodeB);

    int newID = nextNodeID++;

    graph[newID] = Node{ getBendNodePosition(newMovingNodePosition, nodeB, horizontalMove), {nodeA, nodeB}, ID};

    removeNeighborFrom(nodeA, nodeB);
    A.neighbors.push_back(newID);

    removeNeighborFrom(nodeB, nodeA);
    B.neighbors.push_back(newID);
    
}

void Wire::removeNeighborFrom(int nodeID, int to_remove) {
    std::vector<int>& neighbors = graph.at(nodeID).neighbors;
    neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), to_remove), neighbors.end());
}

void Wire::updateNeighborPosition(int movingID, int neighborID, bool horizontalMovement, sf::Vector2f newMovingNodePosition) {
    Node& moving = graph.at(movingID);
    Node& neighbor = graph.at(neighborID);

    if (isAnchor(neighborID))
        return;

    if (horizontalMovement && neighbor.position.x == moving.position.x)
    {
        neighbor.position.x = newMovingNodePosition.x;
        std::cout << "Set neighbor position\n";
    }
    else if (!horizontalMovement && neighbor.position.y == moving.position.y)
    {
        neighbor.position.y = newMovingNodePosition.y;
    }
}