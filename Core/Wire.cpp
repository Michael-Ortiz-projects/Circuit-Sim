#include "Wire.h"

Wire::Wire(sf::Vector2f initialPosition, int id) {
	ID = id;
	graph.emplace(0, Node{ 0, initialPosition, {}, id, false });
	nextNodeID = 1;
    currentStemNode = 0;
    previewOrientation = PreviewOrientation::None;
}

int Wire::appendNodeFromStem(sf::Vector2f pos) {
    int newID = createNode(pos);
    connectNodes(newID, currentStemNode);

    currentStemNode = newID;
    return currentStemNode;
}

int Wire::createNode(sf::Vector2f pos) {
    int id = nextNodeID++;
    graph[id] = Node();
    graph.at(id).position = pos;
    graph.at(id).belongsTo = ID;
    return id;
}

void Wire::deleteNode(int nodeID) {
    auto it = graph.find(nodeID);
    if (it == graph.end()) return;

    Node& node = it->second;

    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        int neighborID = node.neighbors[d];
        if (neighborID == -1) continue;

        auto neighborIt = graph.find(neighborID);
        if (neighborIt != graph.end()) {
            Node& neighbor = neighborIt->second;

            // Remove reference back to this node
            neighbor.neighbors[oppositeDirection(d)] = -1;
        }
    }

    graph.erase(it);
}

void Wire::collapseNodeInto(int keepID, int removeID) {
    Node& keep = graph.at(keepID);
    Node& remove = graph.at(removeID);

    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        int n = remove.neighbors[d];
        if (n == -1) continue;

        Dir opposite = oppositeDirection(d);
        graph.at(n).neighbors[opposite] = keepID;
        keep.neighbors[d] = n;
    }

    keep.isAnchor |= remove.isAnchor;
    graph.erase(removeID);
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
    //when placing a wire collinear nodes need to be erased because it is annoying to always have to put an L wire when placing
    //essentially make it possible to build a straight wire one grid square at a time without making a bunch of nodes
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
    return graph.at(nodeID).isAnchor;
}

bool Wire::isJunction(int nodeID) {
    int neighborcount = 0;
    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        if (graph.at(nodeID).neighbors.has(d)) neighborcount++;
    }
    return neighborcount > 2;
}



SegmentHit Wire::projectOntoSegment(sf::Vector2f& worldPoint) {
    SegmentHit best;

    for (const auto& [id, node] : graph) {
        for (Dir d : {Dir::Right, Dir::Down}) { // only check each segment once
            int neighborID = node.neighbors[d];
            if (neighborID == -1) continue;

            const sf::Vector2f& a = node.position;
            const sf::Vector2f& b = graph.at(neighborID).position;

            // Skip non-axis-aligned segments
            if (a.x != b.x && a.y != b.y) continue;

            // Project worldPoint onto the segment and snap to grid
            sf::Vector2f snapped = snapToGridBetween(a, b, worldPoint);

            float dist = std::hypot(worldPoint.x - snapped.x, worldPoint.y - snapped.y);

            if (!best.valid || dist < std::hypot(worldPoint.x - best.snappedPosition.x, worldPoint.y - best.snappedPosition.y)) {
                best.nodeA = id;
                best.nodeB = neighborID;
                best.snappedPosition = snapped;
                best.valid = true;
            }
        }
    }

    return best;
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

void Wire::connectNodes(int a, int b) {
    graph.at(a).neighbors[directionFrom(a, b)] = b;
    graph.at(b).neighbors[directionFrom(b, a)] = a;
}

void Wire::disconnectNodes(int a, int b) {
    graph.at(a).neighbors[directionFrom(a, b)] = -1;
    graph.at(b).neighbors[directionFrom(b, a)] = -1;
}

Dir Wire::directionFrom(int a, int b) {
    const Node& A = graph.at(a);
    const Node& B = graph.at(b);

    sf::Vector2f delta = B.position - A.position;

    if (delta.x < 0) return Dir::Left;
    if (delta.x > 0) return Dir::Right;
    if (delta.y < 0) return Dir::Up;
    if (delta.y > 0) return Dir::Down;

    throw std::logic_error("directionFrom called on coincident nodes");
}

Dir Wire::oppositeDirection(Dir d) {
    switch (d) {
    case Dir::Left:  return Dir::Right;
    case Dir::Right: return Dir::Left;
    case Dir::Up:    return Dir::Down;
    case Dir::Down:  return Dir::Up;
    }
}