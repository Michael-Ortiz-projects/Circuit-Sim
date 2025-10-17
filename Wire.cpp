#include "Wire.h"

Wire::Wire(sf::Vector2f mousePos) {
    points.push_back(mousePos);
    graph.emplace(0, Node{ mousePos, {} });

    preview_wire_points.setPrimitiveType(sf::LineStrip);
    preview_wire_points.resize(3);
    for (int i = 0; i < 3; i++) {
        preview_wire_points[i].color = sf::Color(255, 0, 0, 75); 
    }
}

void Wire::UpdatePreview(sf::Vector2f mousePos, bool invert) {

    sf::Vector2f last = points.back();
    sf::Vector2f current = mousePos;

    if (std::abs(current.x - last.x) > std::abs(current.y - last.y)) {
        if (invert)
            preview_midpoint = { last.x, current.y };
        else
            preview_midpoint = { current.x, last.y };
    }
    else {
        if (invert)
            preview_midpoint = { current.x, last.y };
        else
            preview_midpoint = { last.x, current.y };
    }

    preview_endpoint = mousePos;
    preview_wire_points[0].position = points.back();
    preview_wire_points[1].position = preview_midpoint;
    preview_wire_points[2].position = preview_endpoint;
}



void Wire::AddHalfPreview() {

    if (points.size() >= 2) {
        sf::Vector2f a = points[points.size() - 2];
        sf::Vector2f b = points[points.size() - 1];
        sf::Vector2f c = preview_midpoint;

        if (isCollinear(a, b, c)) {
            // Replace last point with the new midpoint
            points.back() = c;
            graph[points.size() - 1].position = c;
            // Don't add a new point, just update existing one
            return;
        }
    }

    int newID = points.size();
    int prevID = newID - 1;

    graph.emplace(newID, Node { preview_midpoint, {prevID} });    
    graph[prevID].neighbors.push_back(newID);

    points.push_back(preview_midpoint);
}

void Wire::AddFullPreview() {
    if (preview_midpoint != points.back()) AddHalfPreview();
    
    if (preview_midpoint != preview_endpoint) {
        int newID = points.size();
        int prevID = newID - 1;

        graph.emplace(newID, Node{ preview_endpoint, {prevID} });
        
        graph[prevID].neighbors.push_back(points.size());

        points.push_back(preview_endpoint);
    }
} 

bool Wire::isPointNearWire(sf::Vector2f mouse, float max_distance) {
    std::set<std::pair<int, int>> checked;
    for (const auto& graph_data : graph) {
        
        for (int neighbor : graph_data.second.neighbors) {
            auto edge = std::minmax(graph_data.first, neighbor);
            if (checked.count(edge)) continue;
            checked.insert(edge);

            const sf::Vector2f& a = graph_data.second.position;
            const sf::Vector2f& b = graph.at(neighbor).position;

            if (isPointNearSegment(mouse, a, b, max_distance)) {
                return true;
            }
        }
    }
    return false;
}

std::pair<bool, int> Wire::findClickedNode(sf::Vector2f mousePos) {
    for (const auto& [ID, node] : graph) {
        if (mousePos == node.position) {
            return { true, ID };
        }
    }
    return { false, -1 };
}

std::optional<std::pair<int, int>> Wire::findClickedSegment(sf::Vector2f mousePos, float max_distance) {
    std::set<std::pair<int, int>> checked;

    for (const auto& [id, node] : graph) {
        for (int neighbor : node.neighbors) {
            auto edge = std::minmax(id, neighbor);
            if (checked.count(edge)) continue;
            checked.insert(edge);

            const sf::Vector2f& a = node.position;
            const sf::Vector2f& b = graph.at(neighbor).position;

            if (isPointNearSegment(mousePos, a, b, max_distance)) {
                return std::make_pair(id, neighbor);
            }
        }
    }

    return std::nullopt;
}

void Wire::insertWireAt(int joiningID, Wire& new_wire) {
    std::map<int, int> idMap; //transformation from new wire to current wire ID's 

    for (const auto& [oldID, node] : new_wire.graph) {
        if (oldID == new_wire.points.size() - 1) {
            idMap[oldID] = joiningID; // reuse inserted node
        }
        else {
            int newID = points.size(); //set the new id
            points.push_back(node.position); //add this position to points
            graph.emplace(newID, Node{ node.position, {} }); //emplace new node
            idMap[oldID] = newID; //build IDmap
            if (oldID == 0)
                anchorNodes.push_back(newID);
        }
    }

    for (const auto& [oldID, node] : new_wire.graph) {
        int mappedID = idMap[oldID];
        for (int oldNeighbor : node.neighbors) {
            int mappedNeighbor = idMap[oldNeighbor];

            // Add connection if not already connected
            if (std::find(graph[mappedID].neighbors.begin(), graph[mappedID].neighbors.end(), mappedNeighbor) == graph[mappedID].neighbors.end()) { //if the neighbor is not in the node's neighbors, add it
                graph[mappedID].neighbors.push_back(mappedNeighbor);
            }

            if (std::find(graph[mappedNeighbor].neighbors.begin(), graph[mappedNeighbor].neighbors.end(), mappedID) == graph[mappedNeighbor].neighbors.end()) { //if the node is not in the neighbor's neighbors, add it
                graph[mappedNeighbor].neighbors.push_back(mappedID);
            }
        }
    }

    for (const auto& id : new_wire.connected_entity_ids) {
        connected_entity_ids.push_back(id);
    }
}

int Wire::insertWireBetween(int a, int b, Wire& new_wire) { 

    sf::Vector2f joiningPoint = new_wire.points.back();
    int insertedID = -1;

    for (const auto& [id, node] : graph) { //determine if node or segment clicked, -1 for segment > -1 for node
        if (node.position == joiningPoint) {
            insertedID = id;
            break;
        }
    }

    if (insertedID == -1) {//if segment clicked, split segment and insert a new node between
        insertedID = points.size();
        points.push_back(joiningPoint);
        graph.emplace(insertedID, Node{ joiningPoint, {} });

        std::erase(graph[a].neighbors, b);
        std::erase(graph[b].neighbors, a);

        graph[a].neighbors.push_back(insertedID);
        graph[b].neighbors.push_back(insertedID);

        graph[insertedID].neighbors.push_back(a);
        graph[insertedID].neighbors.push_back(b);
    }
    
    insertWireAt(insertedID, new_wire); //because the logic behind creating vs editing a node has been done, this just creates the id transformation map and doesnt worry about creating a connecting node,
                                        //just adding the rest of the wire to the existing wire

    return insertedID;
}



void Wire::moveNode(int moving_node, sf::Vector2f position) {
    auto& currentNode = graph[moving_node];

    //std::cout << "\nBeginning to Iterate Through " << moving_node << "'s Neighbors: ";
    for (auto& neighbor : currentNode.neighbors) std::cout << neighbor << " ";

    // Store neighbors in a copy so we don't mutate while iterating
    std::vector<int> neighborsCopy = currentNode.neighbors;

    for (auto& neighbor : neighborsCopy) {
        //std::cout << "\n------------------\nWorking with Node " << neighbor << "\n";

        if (!graph.count(neighbor)) {
            //std::cout << "Continued loop because graph does not contain node #" << neighbor << std::endl;
            continue;
        }

        if (!isCollinear(currentNode.position, graph[neighbor].position, position)) {
            // Case 1: neighbor is an anchor node — insert new node between
            if (std::find(anchorNodes.begin(), anchorNodes.end(), neighbor) != anchorNodes.end() || graph[neighbor].neighbors.size() > 2) {
                //std::cout << "Requesting Anchor Node " << neighbor << " to move\n";

                // Erase old connections
                currentNode.neighbors.erase(
                    std::remove(currentNode.neighbors.begin(), currentNode.neighbors.end(), neighbor),
                    currentNode.neighbors.end()
                );
                graph[neighbor].neighbors.erase(
                    std::remove(graph[neighbor].neighbors.begin(), graph[neighbor].neighbors.end(), moving_node),
                    graph[neighbor].neighbors.end()
                );

                // Create new node at proper aligned position
                int newID = graph.empty() ? 0 : graph.rbegin()->first + 1;
                sf::Vector2f newPos;
                if (currentNode.position.y == graph[neighbor].position.y) {
                    newPos = sf::Vector2f(graph[neighbor].position.x, position.y);
                }
                else if (currentNode.position.x == graph[neighbor].position.x) {
                    newPos = sf::Vector2f(position.x, graph[neighbor].position.y);
                }
                else {
                    newPos = position; // fallback if not aligned
                }

                // Create and connect new node
                graph[newID] = Node{ newPos, { moving_node, neighbor } };
                currentNode.neighbors.push_back(newID);
                graph[neighbor].neighbors.push_back(newID);
                points.push_back(newPos);

                //std::cout << "Inserted intermediary Node " << newID << " at (" << newPos.x << ", " << newPos.y << ") between " << moving_node << " and " << neighbor << "\n";
            }
            // Case 2: regular neighbor, just update its axis-aligned coordinate
            else if (currentNode.position != graph[neighbor].position) {
                if (currentNode.position.y == graph[neighbor].position.y) {
                    graph[neighbor].position.y = position.y;
                }
                else if (currentNode.position.x == graph[neighbor].position.x) {
                    graph[neighbor].position.x = position.x;
                }
                //std::cout << "Moved Node " << neighbor << " to (" << graph[neighbor].position.x << ", " << graph[neighbor].position.y << ")\n";
            }
        }
        else {
            //std::cout << "Collinear movement -> no action.\n";
        }
    }

    // Finally, move the current node
    currentNode.position = position;
}
void Wire::mergeNodes(int a, int b) { // 
    if (!graph.count(a) || !graph.count(b)) return;

    sf::Vector2f pos = graph[a].position;

    std::vector<int> newNeighborList;

    for (const auto& neighbor : graph[a].neighbors) {
        if (neighbor != a && neighbor != b) {
            newNeighborList.push_back(neighbor);
        }
    }
    for (const auto& neighbor : graph[b].neighbors) {
        if (neighbor != a && neighbor != b) {
            newNeighborList.push_back(neighbor);
        }
    }

    std::sort(newNeighborList.begin(), newNeighborList.end()); //remove duplicates from newNeighbors
    newNeighborList.erase(
        std::unique(newNeighborList.begin(), newNeighborList.end()),
        newNeighborList.end()
    );

    // modify A
    graph[a] = Node(pos, newNeighborList);

    // Redirect all neighbors' edges from b to a
    for (const auto& neighbor : newNeighborList) {
        for (auto& n : graph[neighbor].neighbors) {
            if (n == b)
                n = a;
        }
    }

    // Erase merging node (b)
    graph.erase(b);
}

void Wire::pruneDeadEnds() {
    
    std::unordered_set<int> anchorSet(anchorNodes.begin(), anchorNodes.end());

    bool changed = true;
    while (changed) {
        if (graph.size() <= 3) return;
        changed = false;
        std::vector<int> toDelete;

        for (const auto& [id, node] : graph) {
            if (node.neighbors.size() == 1 && anchorSet.count(id) == 0) {
                toDelete.push_back(id);
                changed = true;
            }
        }
        if (toDelete.size() > 0) std::cout << "Pruned Dead Ends\n";

        for (int id : toDelete) {
            int neighbor = graph[id].neighbors[0];
            auto& neighborList = graph[neighbor].neighbors;
            neighborList.erase(std::remove(neighborList.begin(), neighborList.end(), id), neighborList.end());
            graph.erase(id);
        }
    }
}


void Wire::pruneCollinearNodes() {
    std::unordered_set<int> anchorSet(anchorNodes.begin(), anchorNodes.end());
    bool changed = true;
    while (changed) {
        if (graph.size() <= 3) return;

        changed = false;
        std::vector<int> deletionVector;

        for (auto& [id, node] : graph) {
            if (node.neighbors.size() != 2 || anchorSet.count(id)) continue;
            
            int neighbor1 = node.neighbors[0];
            int neighbor2 = node.neighbors[1];

            if (!graph.count(neighbor1) || !graph.count(neighbor2)) continue;

            if (isCollinear(graph[neighbor1].position, node.position, graph[neighbor2].position)) {
                deletionVector.push_back(id);
                changed = true;
            }
        }

        if(deletionVector.size()>0) std::cout << "Pruned Collinear Nodes\n";
        for (int id : deletionVector) {
            //std::cout << id << ", ";
            if (!graph.count(id)) continue;

            int neighbor1 = graph[id].neighbors[0];
            int neighbor2 = graph[id].neighbors[1];

            std::vector<int>& neighborList1 = graph[neighbor1].neighbors;
            std::vector<int>& neighborList2 = graph[neighbor2].neighbors;

            neighborList1.erase(std::remove(neighborList1.begin(), neighborList1.end(), id), neighborList1.end());
            neighborList2.erase(std::remove(neighborList2.begin(), neighborList2.end(), id), neighborList2.end());

            if (std::find(neighborList1.begin(), neighborList1.end(), neighbor2) == neighborList1.end()) {
                neighborList1.push_back(neighbor2);
            }

            if (std::find(neighborList2.begin(), neighborList2.end(), neighbor1) == neighborList2.end()) {
               neighborList2.push_back(neighbor1);
            }
            graph.erase(id);
        }

        
    }
    

    
}

void Wire::cleanGraph() {
    for (auto& [id, node] : graph) {
        auto& neighbors = node.neighbors;

        //remove self-reference
        neighbors.erase(
            std::remove(neighbors.begin(), neighbors.end(), id),
            neighbors.end()
        );

        //remove duplicate neighbors
        std::sort(neighbors.begin(), neighbors.end()); //sorts list
        neighbors.erase(
            std::unique(neighbors.begin(), neighbors.end()),
            neighbors.end()
        );
    }
}

void Wire::updateWire(std::pair<int, int>& selectedPair, CursorState& cursorState) {
    bool changed = true;
    while (changed) {
        changed = false;

        std::vector<int> ids;
        for (const auto& [id, node] : graph) {
            ids.push_back(id); // get a list of keys
        }

        for (int i = 0; i < ids.size(); ++i) {
            for (int j = i + 1; j < ids.size(); ++j) {
                int id1 = ids[i];
                int id2 = ids[j];

                // Ensure both nodes still exist 
                if (!graph.count(id1) || !graph.count(id2)) continue;

                if (graph[id1].position == graph[id2].position && graph.size() > 3) {
                    mergeNodes(id1, id2); 
                    std::cout << "Merged Nodes " << id1 << ", " << id2 << "\n";
                    cleanGraph();
                    if (selectedPair.second == id1 || selectedPair.second == id2) {
                        selectedPair = { -1, -1 };
                        cursorState = CursorState::Default;
                    }
                    changed = true;
                    goto restart; // restart with new graph state
                }
            }
        }

        break;

    restart:;
    }
    if (graph.size() > 3) {
        pruneDeadEnds();
        pruneCollinearNodes();
    }
}
void Wire::cancel() {
    preview_midpoint = sf::Vector2f(0, 0);
    preview_endpoint = sf::Vector2f(0, 0);
    points.clear();
    preview_wire_points.clear();
    preview_wire_points.resize(3);
    for (int i = 0; i < 3; i++) {
        preview_wire_points[i].color = sf::Color(255, 0, 0, 75);
    }
    graph.clear();
}

void Wire::draw(sf::RenderWindow& window) {
    if (editing) window.draw(preview_wire_points);
    std::set<std::pair<int, int>> drawnEdges;
    for (const auto& [id, node] : graph) {
        for (int neighborID : node.neighbors) {
            sf::Vertex line[] = {
                sf::Vertex(node.position),
                sf::Vertex(graph[neighborID].position)
            };
            line[0].color = selected ? sf::Color(255, 200, 0) : sf::Color(160, 160, 160);
            line[1].color = selected ? sf::Color(255, 200, 0) : sf::Color(160, 160, 160);
            
            window.draw(line, 2, sf::Lines);
        }
        sf::CircleShape point(4.0f);
        point.setFillColor(sf::Color::Cyan);
        point.setOrigin(4.0f, 4.0f);
        point.setPosition(node.position);
        window.draw(point);
    }
}

void Wire::sortConnectedEntities() {
    std::sort(connected_entity_ids.begin(), connected_entity_ids.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b)
        {return a.first < b.first;});
}

void Wire::printData(TextBox& detailBox, bool updateBox) {
    std::string detailString;

    detailString += "Graph Data:\n";
    for (const auto& data : graph) {
        detailString += "\nNode " + std::to_string(data.first) + ": (" + floatToString(data.second.position.x) + ", " + floatToString(data.second.position.y) + ") ";
    }

    detailString += "\nConnected Entity ID's:\n";

    for (int i = 0; i < connected_entity_ids.size(); i++) {
        detailString += "Entity " + std::to_string(connected_entity_ids[i].first) + ": ";
        if (connected_entity_ids[i].second == 0)
            detailString += "Left\n";
        if (connected_entity_ids[i].second == 1)
            detailString += "Right\n";
    }

    detailString += "Anchor Nodes\n";

    for (int i = 0; i < anchorNodes.size(); i++) {
        detailString += std::to_string(anchorNodes[i]) + " ";
    }

    detailString += "\nNode Voltage: " + doubleToString(node_voltage) + "\n";


    std::cout << detailString;

    if (updateBox) {
        detailBox.text.setString(detailString);
        detailBox.setTextBoundingBox();
    }
}