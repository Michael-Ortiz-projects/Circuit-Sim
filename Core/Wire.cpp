#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ initialPosition, {}, id });
	nextNodeID = 1;
    currentStemNode = 0;
    previewOrientation = PreviewOrientation::None;
}

int Wire::appendNodeFromStem(sf::Vector2f pos) {
    int id = nextNodeID++;

    graph[id].position = pos;

    graph[currentStemNode].neighbors.push_back(id);
    graph[id].neighbors.push_back(currentStemNode);

    currentStemNode = id;
    return id;
}

int Wire::insertNode(sf::Vector2f pos) {
    int id = nextNodeID++;
    graph[id].position = pos;
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
        committedNode = appendNodeFromStem(firstPreview);
    }

    if (secondPreview != graph[currentStemNode].position) {
        committedNode = appendNodeFromStem(secondPreview);
    }

    previewOrientation = PreviewOrientation::None;
    return committedNode;
}

WireMoveResult Wire::moveNode(int movingNodeID, sf::Vector2f newPosition, WireMoveIntent intent) {
    Node& movingNode = graph.at(movingNodeID);
    WireMoveResult result = WireMoveResult::None;


    sf::Vector2f newPos = snapPositionToGrid(newPosition);
    sf::Vector2f currentPosition = movingNode.position;
    sf::Vector2f delta = newPos - currentPosition;

    if (delta == sf::Vector2f{ 0.f, 0.f })
        return WireMoveResult::None;

    sf::Vector2f intermediatePosition = currentPosition;

    if (delta.x != 0.f) {
        intermediatePosition.x += delta.x;
        moveNodeSingleAxis(movingNodeID, intermediatePosition, MoveAxis::Horizontal, intent);
        bool merged = collapseCoincidentNodes(movingNodeID);
        if (merged)
            result = WireMoveResult::NodeMerged;
    }

    if (delta.y != 0.f) {
        intermediatePosition.y += delta.y;
        moveNodeSingleAxis(movingNodeID, intermediatePosition, MoveAxis::Vertical, intent);
        bool merged = collapseCoincidentNodes(movingNodeID);
        if (merged)
            result = WireMoveResult::NodeMerged;
    }

    //cleanupCollinearNodes();
    return result;
}

void Wire::moveNodeSingleAxis(int movingNodeID, const sf::Vector2f& newGridPosition, MoveAxis axis, WireMoveIntent intent) {
    Node& movingNode = graph.at(movingNodeID);
    bool horizontal = (axis == MoveAxis::Horizontal);

    std::vector<int> neighborIDs = movingNode.neighbors;

    for (int neighborID : neighborIDs) {

        if (isAnchor(neighborID) && (intent == WireMoveIntent::Edit || intent == WireMoveIntent::ComponentMove)) {
            Node& neighbor = graph.at(neighborID);

            bool orthogonalBreak = horizontal ? (neighbor.position.x == movingNode.position.x) : (neighbor.position.y == movingNode.position.y);

            if (orthogonalBreak) {
                insertBendNodeBetween(movingNodeID, neighborID, horizontal, newGridPosition);
            }
        }


        else {
            updateNeighborPosition(movingNodeID, neighborID, horizontal, newGridPosition);
        }
    }

    if (isAnchor(movingNodeID)) {
        if (intent == WireMoveIntent::ComponentMove)
            movingNode.position = newGridPosition;
        return;
    }

    if (intent == WireMoveIntent::Edit) {
        movingNode.position = newGridPosition;
    }

    if (isJunction(movingNodeID) && intent == WireMoveIntent::Edit) {
        movingNode.position = newGridPosition;
        return;
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

bool Wire::isJunction(int nodeID) {
    return graph.at(nodeID).neighbors.size() >= 3;
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

sf::Vector2f Wire::computeJunctionPosition(int junctionID) {
    return { -1, -1 }; //for now, make this comput the junciton position, The behavior im looking for is a clamp to a segment
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
        std::cout << "Set Neighbor position\n";
    }
}

bool Wire::collapseCoincidentNodes(int nodeID)
{
    Node& node = graph.at(nodeID);
    std::vector<int> neighbors = node.neighbors;

    for (int neighborID : neighbors) {

        if (graph.at(neighborID).position != node.position)
            continue;

        Node& neighbor = graph.at(neighborID);

        for (int n : neighbor.neighbors) {
            if (n == nodeID) continue;
            graph.at(n).neighbors.push_back(nodeID);
            removeNeighborFrom(n, neighborID);
            node.neighbors.push_back(n);
        }

        removeNeighborFrom(nodeID, neighborID);
        graph.erase(neighborID);

        return true;
    }
    return false;
}

bool Wire::isCollinear(int nodeID)
{
    Node& n = graph.at(nodeID);
    if (n.neighbors.size() != 2)
        return false;

    Node& a = graph.at(n.neighbors[0]);
    Node& b = graph.at(n.neighbors[1]);

    return
        (a.position.x == n.position.x && n.position.x == b.position.x) ||
        (a.position.y == n.position.y && n.position.y == b.position.y);
}

void Wire::removeCollinearNode(int nodeID) {
    Node& n = graph.at(nodeID);

    int a = n.neighbors[0];
    int b = n.neighbors[1];

    removeNeighborFrom(a, nodeID);
    removeNeighborFrom(b, nodeID);

    graph.at(a).neighbors.push_back(b);
    graph.at(b).neighbors.push_back(a);

    graph.erase(nodeID);
}

SegmentHit Wire::projectOntoSegment(sf::Vector2f& worldPoint) {
    SegmentHit best;

    for (const auto& [id, node] : graph) {
        for (int n : node.neighbors) {
            if (id >= n) continue;

            const sf::Vector2f& a = node.position;
            const sf::Vector2f& b = graph.at(n).position;

            if (a.x != b.x && a.y != b.y)
                continue;

            sf::Vector2f snapped = snapToGridBetween(a, b, worldPoint);

            float d = std::hypot(
                worldPoint.x - snapped.x,
                worldPoint.y - snapped.y
            );

            if (!best.valid || d < std::hypot(
                worldPoint.x - best.snappedPosition.x,
                worldPoint.y - best.snappedPosition.y)) {

                best.nodeA = id;
                best.nodeB = n;
                best.snappedPosition = snapped;
                best.valid = true;
            }
        }
    }

    return best;
}

void Wire::cleanupCollinearNodes() {
    std::vector<int> toCheck;

    for (auto& [id, node] : graph)
        toCheck.push_back(id);

    for (int id : toCheck) {
        if (!isAnchor(id) && isCollinear(id))
            removeCollinearNode(id);
    }
}


float Wire::snapCoordinateToGrid(const float coordinate) {
    return std::round(coordinate / gridSize) * gridSize;
}

sf::Vector2f Wire::snapToGridBetween(sf::Vector2f A, sf::Vector2f B, sf::Vector2f point) {
    if (A.y == B.y) {
        float x = snapCoordinateToGrid(point.x);
        float minX = std::min(A.x, B.x);
        float maxX = std::max(A.x, B.x);
        x = std::clamp(x, minX, maxX);
        return { x, A.y };
    }

    if (A.x == B.x) {
        float y = snapCoordinateToGrid(point.y);
        float minY = std::min(A.y, B.y);
        float maxY = std::max(A.y, B.y);
        y = std::clamp(y, minY, maxY);
        return { A.x, y };
    }

    return { -1, -1 };
}

