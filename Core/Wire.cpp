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
    //std::cout << "delta.x = " << delta.x << "\ndelta.y = " << delta.y << std::endl;
    bool primaryDeleted = false;

    if (std::abs(delta.x) > 0) {
        moveNodeSingleAxis(primaryNode, MoveAxis::Horizontal, snappedPosition.x, delta, intent);
        printGraphData();
        primaryDeleted = cleanAllCoincidentNodes(primaryNode);
    }

    if (std::abs(delta.y) > 0) {
        moveNodeSingleAxis(primaryNode, MoveAxis::Vertical, snappedPosition.y, delta, intent);
        primaryDeleted = cleanAllCoincidentNodes(primaryNode);
    }
    
    return primaryDeleted ? WireMoveResult::NodeRemoved : WireMoveResult::None;

}

//this functions job is to move a primary node to a new location and update its neighbors positions and neighbors correctly based on their classification (junction anchor etc.)
void Wire::moveNodeSingleAxis(Node& primaryNode, MoveAxis axis, int newCoordinate, sf::Vector2f delta, WireMoveIntent& intent) {//bugged right now, maybe disconnect the appropriate node, move, then connect appropriate nodes
    sf::Vector2f oldPrimaryPosition = primaryNode.position;
    sf::Vector2f newPrimaryPosition = axis == MoveAxis::Horizontal ? (sf::Vector2f(newCoordinate, primaryNode.position.y)) : (sf::Vector2f(primaryNode.position.x, newCoordinate));
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
        //std::cout << "primary neighbor = " << primaryNeighbor.id << "\n";

        if (primaryNeighbor.isAnchor) {
            //std::cout << "is anchor node\n";
            //do anchor movement
            if (isOrthogonalTo(primaryToNeighborDirection, movingDirection)) {
                //allow node insertion
                //std::cout << "direction from primary to neighbor is ortho to moving direction\n";
                
                sf::Vector2f insertedPosition;

                if (primaryNeighborMovingTowardNeighbor) {
                    Node& primaryNeighbors_NeighborInMovingDirection = graph.at(primaryNeighbor.neighbors[movingDirection]);

                    disconnectNodes(primaryNeighbor, primaryNeighbors_NeighborInMovingDirection);
                    axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                    primaryNode.position = newPrimaryPosition;

                    Node& inserted = graph.at(insertNodeBetween(primaryNode, primaryNeighbor, insertedPosition));
                    primaryNode.position = oldPrimaryPosition;
                    connectNodes(inserted, primaryNeighbors_NeighborInMovingDirection);
                }
                else {
                    axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                    primaryNode.position = newPrimaryPosition;

                    Node& inserted = graph.at(insertNodeBetween(primaryNode, primaryNeighbor, insertedPosition));
                    primaryNode.position = oldPrimaryPosition;
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
                    

                    if (primaryNeighborMovingTowardNeighbor) {
                        Node& primaryNeighbors_NeighborInMovingDirection = graph.at(primaryNeighbor.neighbors[movingDirection]);

                        disconnectNodes(primaryNeighbor, primaryNeighbors_NeighborInMovingDirection);
                        axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                        primaryNode.position = newPrimaryPosition;
                        Node& inserted = graph.at(insertNodeBetween(primaryNode, primaryNeighbor, insertedPosition));
                        primaryNode.position = oldPrimaryPosition;
                        connectNodes(inserted, primaryNeighbors_NeighborInMovingDirection, movingDirection);
                    }
                    else {
                        axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                        primaryNode.position = newPrimaryPosition;

                        Node& inserted = graph.at(insertNodeBetween(primaryNode, primaryNeighbor, insertedPosition));
                        primaryNode.position = oldPrimaryPosition;
                    }
                       
                }

                else {
                    //only one orthogonal segment, insert node if necessary

                    if (primaryNeighborMovingTowardNeighbor) {
                        axis == MoveAxis::Horizontal ? (primaryNeighbor.position.x = newCoordinate) : (primaryNeighbor.position.y = newCoordinate);
                    }

                    else {
                        sf::Vector2f insertedPosition;
                        axis == MoveAxis::Horizontal ? (insertedPosition = sf::Vector2f(newCoordinate, primaryNeighbor.position.y)) : (insertedPosition = sf::Vector2f(primaryNeighbor.position.x, newCoordinate));
                        primaryNode.position = newPrimaryPosition;

                        Node& inserted = graph.at(insertNodeBetween(primaryNode, primaryNeighbor, insertedPosition));
                        primaryNode.position = oldPrimaryPosition;
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

    std::cout << "cleanCoincidentNodes() finished\n" << primaryDeleted << std::endl;
    return primaryDeleted;
}

void Wire::mergeNodes(Node& keep, Node& remove) { //not complete at all
    std::cout << "merge nodes running\nMerging nodes: " << keep.id << " and " << remove.id << std::endl;
    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        int removeNeighborID = remove.neighbors[d];//2
        int keepNeighborID = keep.neighbors[d];//-1
        if (removeNeighborID == -1 || removeNeighborID == keep.id) continue;//THIS IS A BUG LOOK INTO THIS
        Node& removeNeighbor = graph.at(removeNeighborID);
       
        if (removeNeighborID != -1 && keepNeighborID != -1 && keepNeighborID != removeNeighborID) {//if both keep and remove have neighbors in the same direction
        //find which neighbor is further from their position
            Node& keepNeighbor = graph.at(keepNeighborID);
            float distToRemoveNeighbor = distanceBetween(keep.position, removeNeighbor.position);
            float distToKeepNeighbor = distanceBetween(keep.position, keepNeighbor.position);

            if (distToKeepNeighbor < distToRemoveNeighbor) {
                disconnectNodes(remove, keepNeighbor);
                connectNodes(keep, keepNeighbor, d);
                connectNodes(keepNeighbor, removeNeighbor, oppositeDirection(d));
            }

            else {
                disconnectNodes(remove, removeNeighbor);
                connectNodes(keep, removeNeighbor, d);
                connectNodes(keepNeighbor, removeNeighbor, oppositeDirection(d));
            }

        }
        else {
            std::cout << "disconnecting nodes running between \n" << keep.id << "and " << removeNeighborID;


            // Disconnect remove from neighbor
            disconnectNodes(remove, removeNeighbor);
            disconnectNodes(keep, remove);
            std::cout << "\ndisconnecting nodes ran\n";
            // Connect keep to neighbor if not already connected
            printNodeData(keep);
            printNodeData(remove);
            std::cout << "connecting nodes running\n";

            connectNodes(keep, removeNeighbor, d);
            std::cout << "connecting nodes ran\n";
        }
        
        
    }

    // remove node from graph
    disconnectNodes(keep, remove);
    graph.erase(remove.id);
    std::cout << "merge nodes finished\n";
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

void Wire::connectNodes(Node& a, Node& b, Dir dirAB) {
    a.neighbors[dirAB] = b.id;
    b.neighbors[oppositeDirection(dirAB)] = a.id;
}
void Wire::disconnectNodes(int a, int b) {
    disconnectNodes(graph.at(a), graph.at(b));
}

void Wire::disconnectNodes(Node& a, Node& b) {

    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        if (a.neighbors[d] == b.id) {
            std::cout << "\nduring disconnect, node " << a.id << "[" << dirToString(d) << "] = " << b.id;
            a.neighbors[d] = -1;
        }
        if (b.neighbors[d] == a.id) {
            std::cout << "\nduring disconnect, node " << b.id << "[" << dirToString(d) << "] = " << a.id;
            b.neighbors[d] = -1;
        }
        std::cout << std::endl;
    }
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

void Wire::printNodeData(Node& node) {

    std::cout << "\nID: " << node.id;
    std::cout << "\nNeighbors\n";
    for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        std::cout << "[" << dirToString(d) << "] = " << node.neighbors[d] << std::endl;
    }
}

void Wire::printGraphData() {
    std::cout << "Graph nodes:\n";

    for (const auto& [nodeID, node] : graph) {
        std::cout << "  Node " << nodeID
            << " at (" << node.position.x << ", " << node.position.y << ")";

        bool hasNeighbors = false;
        for (Dir d : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
            int neighborID = node.neighbors[d];
            if (neighborID != -1) {
                if (!hasNeighbors) {
                    std::cout << " -> Neighbors: ";
                    hasNeighbors = true;
                }
                std::cout << "[" << dirToString(d) << "] = " << node.neighbors[d] << "    ";
            }

        }

        if (isJunction(nodeID)) {
            std::cout << " Junction Node ";
        }
        if (isAnchor(nodeID)) {
            std::cout << "Anchor Node ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

float Wire::distanceBetween(sf::Vector2f a, sf::Vector2f b) {
    sf::Vector2f delta = a - b;

    return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}
