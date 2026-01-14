#include "Circuit.h"
#include <algorithm>
#include <iostream>

int Circuit::addComponent(const Component& comp, const sf::Vector2f& canvasPos) {
    int id = nextComponentID++;
    Component c = comp;
    c.position = canvasPos;
    c.id = id;
    componentIDToIndex[id] = static_cast<int>(components.size());
    c.nodeA = comp.nodeA; 
    c.nodeB = comp.nodeB;
    c.A_WireNodeReference = { -1, -1 };
    c.B_WireNodeReference = { -1, -1 };

    components.push_back(c);

    return id;
}

bool Circuit::removeComponent(int compID) {
    auto it = componentIDToIndex.find(compID);
    if (it == componentIDToIndex.end()) return false;

    int index = it->second;

    components.erase(components.begin() + index);
    componentIDToIndex.erase(it);

    componentIDToIndex.clear();
    for (int i = 0; i < components.size(); ++i) {
        componentIDToIndex[components[i].id] = i;
    }

    return true;
}

Component* Circuit::getComponent(int compID) {
    auto it = componentIDToIndex.find(compID);
    if (it == componentIDToIndex.end()) return nullptr;
    return &components[it->second];
}

std::vector<Component>& Circuit::getComponents() {
    return components;
}

Wire* Circuit::getWire(int wireID) {
    auto it = wires.find(wireID);
    if (it == wires.end()) {
        std::cout << "Circuit.getWire() returned nullptr\n";
        return nullptr;
    }

    Wire& wire = it->second;
    return &wire;
}

std::unordered_map<int, Wire>& Circuit::getWires() {
    return wires;
}

ElectricalNode* Circuit::getElectricalNode(int nodeID) {
    return &nodes.at(nodeID);
}

int Circuit::createElectricalNode() {
    nodes.emplace(nextNodeID, ElectricalNode{ nextNodeID });
    return nextNodeID++;
}

void Circuit::removeElectricalNode(int nodeID) {
    nodes.erase(nodeID);
}

void Circuit::mergeElectricalNodes(int primary, int active) {
    ElectricalNode& activeNode = nodes.at(active);
    ElectricalNode& primaryNode = nodes.at(primary);
    for (const auto& connection : activeNode.connections) {
        if (std::find(primaryNode.connections.begin(), primaryNode.connections.end(), connection) == primaryNode.connections.end()) {
            primaryNode.connections.push_back(connection);
        }
    }

}
void Circuit::addConnectionToNode(int nodeID, ElectricalConnection& connection) {
    nodes[nodeID].connections.push_back({ connection });
}

void Circuit::removeConnectionFromNode(int nodeID, ElectricalConnection& connection) {
   

    nodes[nodeID].connections.erase(std::remove_if(nodes[nodeID].connections.begin(), nodes[nodeID].connections.end(),
        [connection](const ElectricalConnection& c) {
            return c.componentID == connection.componentID;
        }),
        nodes[nodeID].connections.end()
    );
}

void Circuit::updateComponentLead(int wireID, int wireNodeID, int ElectricalNodeID, ElectricalConnection& connection) {
    auto comp = getComponent(connection.componentID);
    switch (connection.lead) {
    case Lead::A:
        comp->nodeA = ElectricalNodeID;
        comp->A_WireNodeReference = { wireID, wireNodeID };
        // Only mark as anchor if wire and node exist
        if (wires.find(wireID) != wires.end()) {
            auto& graph = wires.at(wireID).getGraph();
            if (graph.find(wireNodeID) != graph.end()) {
                graph.at(wireNodeID).isAnchor = true;
            }
        }
        return;
    case Lead::B:
        comp->nodeB = ElectricalNodeID;
        comp->B_WireNodeReference = { wireID, wireNodeID };
        if (wires.find(wireID) != wires.end()) {
            auto& graph = wires.at(wireID).getGraph();
            if (graph.find(wireNodeID) != graph.end()) {
                graph.at(wireNodeID).isAnchor = true;
            }
        }
        return;
    case Lead::Null:
        std::cout << "Update component Lead Null connection\n";
        return;
    }
}


int Circuit::createWire(sf::Vector2f position) {
    wires.emplace(nextWireID, Wire(snapPositionToGrid(position), nextWireID));
    return nextWireID++;
}

void Circuit::eraseWire(int wireID) {
    auto wireIt = wires.find(wireID);
    if (wireIt == wires.end()) return;  

    Wire& wire = wireIt->second;

    auto enIt = wireIDToElectricalNode.find(wireID);
    int eNodeID = -1;
    if (enIt != wireIDToElectricalNode.end()) {
        eNodeID = enIt->second;
    }

    // get anchor connections to avoid invalidating graph during traversal
    std::vector<ElectricalConnection> connectionsToRemove;
    for (auto& [id, node] : wire.getGraph()) {
        if (node.isAnchor) {
            for (auto& comp : components) {
                if (comp.A_WireNodeReference.wireID == wireID && comp.A_WireNodeReference.nodeID == id) {
                    connectionsToRemove.push_back({ comp.id, Lead::A });
                }
                if (comp.B_WireNodeReference.wireID == wireID && comp.B_WireNodeReference.nodeID == id) {
                    connectionsToRemove.push_back({ comp.id, Lead::B });
                }
            }
        }
    }


    // remove electrical connections
    for (auto& conn : connectionsToRemove) {
        if (eNodeID != -1 && nodes.find(eNodeID) != nodes.end()) {
            removeConnectionFromNode(eNodeID, conn);
        }
        updateComponentLead(-1, -1, -1, conn);
    }


    // remove electrical node if empty
    if (eNodeID != -1 && nodes.find(eNodeID) != nodes.end() && nodes.at(eNodeID).connections.empty()) {
        removeElectricalNode(eNodeID);
    }


    // erase wire ID mapping
    wireIDToElectricalNode.erase(wireID);

    // erase the wire itself
    wires.erase(wireIt);
}

bool Circuit::leadIsEmpty(ElectricalConnection& connection) { // returns false if connection is not valid
    switch (connection.lead) {
    case Lead::A:
        if (getComponent(connection.componentID)->nodeA == -1) return true;
        else return false;
        break;

    case Lead::B:
        if (getComponent(connection.componentID)->nodeB == -1) return true;
        else return false;
        break;
    case Lead::Null:
        return false;
        break;
    }
}


sf::Vector2f Circuit::snapPositionToGrid(const sf::Vector2f& position) {
    return {
        std::round(position.x / gridSize) * gridSize,
        std::round(position.y / gridSize) * gridSize
    };
}

