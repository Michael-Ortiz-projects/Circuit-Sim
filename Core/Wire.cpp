#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ 0, initialPosition, {}, id, false });
	nextNodeID = 1;
    currentStemNode = 0;
    previewOrientation = PreviewOrientation::None;
}

int Wire::createNode(sf::Vector2f pos) {
    int id = nextNodeID++;
    Node n;
    n.id = id;
    n.position = pos;
    n.belongsTo = ID;
    graph[id] = n;
    return id;
}

void Wire::removeNode(int nodeID) {
    if (!graph.contains(nodeID)) return;

    std::vector<int> neighborsToDisconnect(
        graph.at(nodeID).neighbors.begin(),
        graph.at(nodeID).neighbors.end()
    );

    for (int neighborID : neighborsToDisconnect) {
        disconnectNodes(neighborID, nodeID);
    }

    graph.erase(nodeID);
}

bool Wire::connectNodes(int nodeA, int nodeB) { //returns true if successful
    if (!graph.contains(nodeA) || !graph.contains(nodeB)) return false;
    Node& A = graph.at(nodeA);
    Node& B = graph.at(nodeB);
    A.neighbors.insert(nodeB);
    B.neighbors.insert(nodeA);
    return true;
}

bool Wire::disconnectNodes(int nodeA, int nodeB) { // meant to disconnect before erasing, returns true if successful
    if (!graph.contains(nodeA) || !graph.contains(nodeB)) return false;
    Node& A = graph.at(nodeA);
    Node& B = graph.at(nodeB);

    bool B_ErasedFromA = A.neighbors.erase(nodeB) > 0;
    bool A_ErasedFromB = B.neighbors.erase(nodeA) > 0;
    return B_ErasedFromA && A_ErasedFromB;
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

int Wire::appendNodeFromStem(sf::Vector2f pos) {//fix this pls
    int newID = createNode(pos);
    connectNodes(newID, currentStemNode);
    currentStemNode = newID;
    return currentStemNode;
}

int Wire::commitPreview() {
    int committedNode = -1;
    if (firstPreview != graph[currentStemNode].position) {
        committedNode = appendNodeFromStem(firstPreview);
    }

    if (secondPreview != graph[currentStemNode].position && firstPreview != secondPreview) {
        committedNode = appendNodeFromStem(secondPreview);
    }

    previewOrientation = PreviewOrientation::None;
    return committedNode;
}


WireMoveResult Wire::moveNode(int movingNodeID, sf::Vector2f worldPosition) {
    if (!graph.contains(movingNodeID)) return WireMoveResult::Invalid;

    Node& movingNode = graph.at(movingNodeID);
    movingNode.position = snapPositionToGrid(worldPosition);
    return WireMoveResult::Valid;
}

bool Wire::mergeCollinearAtNode(int nodeID) {
    //maybe put stuff here
    return false;
}

bool Wire::mergeCoincidentNodes(int nodeID) { // merges coincident nodes into primary node
    if (!graph.contains(nodeID))
        return false;

    Node& node = graph.at(nodeID);

    for (auto& [otherID, other] : graph) {
        if (otherID == nodeID || node.position != other.position)
            continue;

        

        // Merge other into node
        mergeNodes(nodeID, otherID);
        return true;
    }

    return false;
}

bool Wire::mergeNodes(int primaryID, int merging) {
    if (!graph.contains(primaryID) || !graph.contains(merging)) return false;
    Node& primaryNode = graph.at(primaryID);
    Node& mergingNode = graph.at(merging);

    for (int mergingNeighborID : mergingNode.neighbors) {
        if (mergingNeighborID == primaryID) continue;
        connectNodes(primaryID, mergingNeighborID);
    }
    removeNode(merging);
}

sf::Vector2f Wire::snapPositionToGrid(const sf::Vector2f& position) {
    return {
        std::round(position.x / gridSize) * gridSize,
        std::round(position.y / gridSize) * gridSize
    };
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


SegmentHit Wire::projectOntoSegment(sf::Vector2f& worldPoint) {
    SegmentHit best;

    for (const auto& [id, node] : graph) {
        for (int neighborID : node.neighbors) {
            if (neighborID < 0 || neighborID <= id)
                continue;

            const sf::Vector2f& A = node.position;
            const sf::Vector2f& B = graph.at(neighborID).position;

            // Vector projection of P onto segment AB
            sf::Vector2f AB = B - A;
            sf::Vector2f AP = worldPoint - A;

            float abLenSq = AB.x * AB.x + AB.y * AB.y;
            if (abLenSq == 0.f)
                continue; // Degenerate segment

            float t = (AP.x * AB.x + AP.y * AB.y) / abLenSq;
            t = std::clamp(t, 0.f, 1.f);

            sf::Vector2f projected = A + AB * t;

            float dist = std::hypot(
                worldPoint.x - projected.x,
                worldPoint.y - projected.y
            );

            if (!best.valid || dist < std::hypot(
                worldPoint.x - best.snappedPosition.x,
                worldPoint.y - best.snappedPosition.y)) {

                best.nodeA = id;
                best.nodeB = neighborID;
                best.snappedPosition = projected;
                best.valid = true;
            }
        }
    }

    return best;
}



bool Wire::isAnchor(int nodeID) {
    return graph.at(nodeID).isAnchor;
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

float Wire::distanceBetween(sf::Vector2f a, sf::Vector2f b) {
    sf::Vector2f delta = a - b;

    return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}

bool Wire::areCollinear(sf::Vector2f& A, sf::Vector2f& B, sf::Vector2f& C) {
    sf::Vector2f AB = B - A;
    sf::Vector2f BC = C - B;

    float cross = AB.x * BC.y - AB.y * BC.x;
    return std::abs(cross) < 0;
}

void Wire::printNodeData(Node& node) {

    std::cout << "\nID: " << node.id;
    std::cout << "\nNeighbors\n";
    for (int neighborID : node.neighbors) {
        std::cout << neighborID;
    }
    std::cout << std::endl;
}

void Wire::printGraphData() {
    std::cout << "Graph nodes:\n";

    for (const auto& [nodeID, node] : graph) {
        std::cout << "  Node " << nodeID
            << " at (" << node.position.x << ", " << node.position.y << ")";

        bool hasNeighbors = false;
        for (int neighborID : node.neighbors) {
            if (neighborID != -1) {
                if (!hasNeighbors) {
                    std::cout << " -> Neighbors: ";
                    hasNeighbors = true;
                }
                std::cout << neighborID << "    ";
            }
        }

        if (isAnchor(nodeID)) {
            std::cout << "Anchor Node ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}


