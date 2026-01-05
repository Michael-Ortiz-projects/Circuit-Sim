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
    //when placing a wire collinear nodes need to be erased and neighbors adjusted because it is annoying to always have to put an L wire when placing
    //essentially make it possible to build a straight wire one grid square at a time without making a bunch of nodes
}

int Wire::appendNodeFromStem(sf::Vector2f pos) {
    int newID = createNode(pos);
    connectNodes(newID, currentStemNode);

    currentStemNode = newID;
    return currentStemNode;
}

WireMoveResult Wire::moveNode(int nodeID, sf::Vector2f worldPosition, WireMoveIntent intent) {
    Node& primaryNode = graph.at(nodeID);
    sf::Vector2f snappedPosition = snapPositionToGrid(worldPosition);
    sf::Vector2f delta = snappedPosition - primaryNode.position;
    bool primaryDeleted = false;

    if (std::abs(delta.x) > 0) {
        moveNodeSingleAxis(primaryNode, MoveAxis::Horizontal, snappedPosition.x, delta, intent);
        //primaryDeleted = cleanAllCoincidentNodes(primaryNode);
    }

    if (std::abs(delta.y) > 0) {
        moveNodeSingleAxis(primaryNode, MoveAxis::Vertical, snappedPosition.y, delta, intent);
        //primaryDeleted = cleanAllCoincidentNodes(primaryNode);
    }
    

    return primaryDeleted ? WireMoveResult::NodeRemoved : WireMoveResult::None;

}

//this functions job is to move a primary node to a new location and update its neighbors positions and neighbors correctly based on their classification (junction anchor etc.)
void Wire::moveNodeSingleAxis(Node& primaryNode, MoveAxis axis, int newCoordinate, sf::Vector2f delta, WireMoveIntent& intent) {//bugged right now, maybe disconnect the appropriate node, move, then connect appropriate nodes
    sf::Vector2f oldPrimaryPosition = primaryNode.position;
    sf::Vector2f newPrimaryPosition = axis == MoveAxis::Horizontal ? (sf::Vector2f(newCoordinate, primaryNode.position.y)) : (sf::Vector2f(primaryNode.position.x, newCoordinate));
    //axis == MoveAxis::Horizontal ? (primaryNode.position.x = newCoordinate) : (primaryNode.position.y = newCoordinate);
    Dir movingDirection = axis == MoveAxis::Horizontal ? (delta.x < 0 ? Dir::Left : Dir::Right) : (delta.y < 0 ? Dir::Up : Dir::Down);
    std::vector<int> originalNeighbors;

    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        int nid = primaryNode.neighbors[d];
        if (nid != -1)
            originalNeighbors.push_back(nid);
    }
    for (int neighborID : originalNeighbors) {
        primaryNode.position = oldPrimaryPosition;
        Node& primaryNeighbor = graph.at(neighborID);
        Dir primaryToNeighborDirection = directionFrom(primaryNode, primaryNeighbor);
        bool primaryNeighborMovingTowardNeighbor = primaryNeighbor.neighbors[movingDirection] != -1;
        std::cout << "primary neighbor = " << primaryNeighbor.id << "\n";

        if (primaryNeighbor.isAnchor) {
            std::cout << "is anchor node\n";
            //do anchor movement
            if (isOrthogonalTo(primaryToNeighborDirection, movingDirection)) {
                //allow node insertion
                std::cout << "direction from primary to neighbor is ortho to moving direction\n";
                
                sf::Vector2f insertedPosition;
                axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                Node& inserted = graph.at(createNode(insertedPosition));
                disconnectNodes(primaryNode, primaryNeighbor);

                primaryNode.position = newPrimaryPosition;
                connectNodes(primaryNode, inserted);
                connectNodes(primaryNeighbor, inserted);//FIND A WAY TO DO THIS MORE, DISCONNECT MOVE RECONNECT


                if (primaryNeighborMovingTowardNeighbor) {
                    Node& primaryNeighbors_NeighborInMovingDirection = graph.at(primaryNeighbor.neighbors[movingDirection]);

                    disconnectNodes(primaryNeighbor, primaryNeighbors_NeighborInMovingDirection);
                    connectNodes(inserted, primaryNeighbors_NeighborInMovingDirection);
                }
            }
        }
        
        else if (isJunction(primaryNeighbor.id)) {
            //do junction movement
            if (isOrthogonalTo(primaryToNeighborDirection, movingDirection)) {
                //allow junction movement

                if (primaryNeighbor.neighbors[primaryToNeighborDirection] != -1) {
                    //two orthogonal segments on junction, insert a new node
                    sf::Vector2f insertedPosition;
                    axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                    Node& inserted = graph.at(insertNodeBetween(primaryNode, primaryNeighbor, insertedPosition));
                    Node& primaryNeighbors_NeighborInMovingDirection = graph.at(primaryNeighbor.neighbors[movingDirection]);

                    if (primaryNeighborMovingTowardNeighbor) {
                        disconnectNodes(primaryNeighbor, primaryNeighbors_NeighborInMovingDirection);
                        connectNodes(inserted, primaryNeighbors_NeighborInMovingDirection);
                    }
                       
                }

                else {
                    //only one orthogonal segment, insert node if necessary
                    if (!primaryNeighborMovingTowardNeighbor) {
                        sf::Vector2f insertedPosition;
                        axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                        Node& inserted = graph.at(insertNodeBetween(primaryNode, primaryNeighbor, insertedPosition));
                    }
                    else {
                        axis == MoveAxis::Horizontal ? (primaryNeighbor.position.x = newCoordinate) : (primaryNeighbor.position.y = newCoordinate);
                    }
                }
            }
        }

        else {
            //do regular node movement            
            if (isOrthogonalTo(primaryToNeighborDirection, movingDirection)) {
                axis == MoveAxis::Horizontal ? (primaryNeighbor.position.x = newCoordinate) : (primaryNeighbor.position.y = newCoordinate);
            }
        }
    }

    axis == MoveAxis::Horizontal ? (primaryNode.position.x = newCoordinate) : (primaryNode.position.y = newCoordinate);

}

bool Wire::cleanAllCoincidentNodes(Node& primaryNode) {
    bool primaryDeleted = false;
    bool mergedSomething;

    do {
        mergedSomething = false;

        for (auto it1 = graph.begin(); it1 != graph.end(); ++it1) {
            Node& nodeA = it1->second;

            auto it2 = it1;
            ++it2; // only compare to nodes after nodeA
            for (; it2 != graph.end(); ++it2) {
                Node& nodeB = it2->second;

                if (nodeA.position == nodeB.position) {
                    // Decide which node to keep
                    Node* keep = &nodeA;
                    Node* remove = &nodeB;

                    // If primaryNode is involved, make sure we keep track of deletion
                    if (nodeA.id == primaryNode.id) {
                        keep = &nodeA;
                        remove = &nodeB;
                    }
                    else if (nodeB.id == primaryNode.id) {
                        keep = &nodeA;   // remove primary
                        remove = &nodeB;
                        primaryDeleted = true;
                    }
                    else {
                        // Neither is primary; arbitrarily keep nodeA
                        keep = &nodeA;
                        remove = &nodeB;
                    }

                    mergeNodes(*keep, *remove);
                    mergedSomething = true;
                    goto restart_outer; // iterators invalidated, restart scan
                }
            }
        }

    restart_outer:;
    } while (mergedSomething);

    return primaryDeleted;
}

void Wire::mergeNodes(Node& keep, Node& remove) {
    std::cout << "merge nodes ran\n";
    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        int neighborID = remove.neighbors[d];
        if (neighborID == -1) continue;

        Node& neighbor = graph.at(neighborID);

        // Disconnect remove from neighbor
        disconnectNodes(remove, neighbor);

        // Connect keep to neighbor if not already connected
        if (!keep.neighbors.has(directionFrom(keep, neighbor))) {
            connectNodes(keep, neighbor);
        }
    }

    // remove node from graph
    graph.erase(remove.id);
}

int Wire::insertNodeBetween(Node& a, Node& b, sf::Vector2f position) {
    Node& newNode = graph.at(createNode(position));
    std::cout << "disconnecting nodes " << a.id << " and " << b.id << std::endl;
    disconnectNodes(a, b);
    std::cout << "connecting nodes " << a.id << " and " << newNode.id << std::endl;
    connectNodes(a, newNode);
    std::cout << "connecting nodes " << b.id << " and " << newNode.id << std::endl;
    connectNodes(b, newNode);

    return newNode.id;
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
        for (Dir d : {Dir::Right, Dir::Down}) {
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

void Wire::connectNodes(int a, int b) {
    connectNodes(graph.at(a), graph.at(b));
}

void Wire::connectNodes(Node& a, Node& b) {
    std::cout << a.id << ".neighbors[" << dirToString(directionFrom(a, b)) << "] = " << b.id << std::endl;
    a.neighbors[directionFrom(a, b)] = b.id;
    std::cout << b.id << ".neighbors[" << dirToString(directionFrom(b, a)) << "] = " << a.id << std::endl;
    b.neighbors[directionFrom(b, a)] = a.id;
}

void Wire::disconnectNodes(int a, int b) {
    disconnectNodes(graph.at(a), graph.at(b));
}

void Wire::disconnectNodes(Node& a, Node& b) {
    a.neighbors[directionFrom(a, b)] = -1;
    b.neighbors[directionFrom(b, a)] = -1;
}

Dir Wire::directionFrom(int a, int b) {
    const Node& A = graph.at(a);
    const Node& B = graph.at(b);

    return directionFrom(A, B);
}

Dir Wire::directionFrom(const Node& a, const Node& b) {
    sf::Vector2f delta = b.position - a.position;

    if (delta.x != 0 && delta.y == 0)
        return delta.x < 0 ? Dir::Left : Dir::Right;
    if (delta.y != 0 && delta.x == 0)
        return delta.y < 0 ? Dir::Up : Dir::Down;

    std::cout << "throwing\n";
    std::cout << delta.x << ", " << delta.y << std::endl;
    throw std::logic_error("directionFrom called on diagonal or coincident nodes");
}

Dir Wire::directionFrom(sf::Vector2f A, sf::Vector2f B) {
    sf::Vector2f delta = B - A;
    if (delta.x != 0 && delta.y == 0)
        return delta.x < 0 ? Dir::Left : Dir::Right;
    if (delta.y != 0 && delta.x == 0)
        return delta.y < 0 ? Dir::Up : Dir::Down;

    std::cout << "throwing\n";
    std::cout << delta.x << ", " << delta.y << std::endl;
    throw std::logic_error("directionFrom called on diagonal or coincident nodes");
}
Dir Wire::oppositeDirection(Dir d) {
    switch (d) {
    case Dir::Left:  return Dir::Right;
    case Dir::Right: return Dir::Left;
    case Dir::Up:    return Dir::Down;
    case Dir::Down:  return Dir::Up;
    }
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

bool Wire::isOrthogonalTo(Dir d, Dir toCheck) {
    if (d == Dir::Left || d == Dir::Right)
        return toCheck == Dir::Up || toCheck == Dir::Down;
    else if (d == Dir::Up || d == Dir::Down)
        return toCheck == Dir::Left || toCheck == Dir::Right;
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