#include "Circuit.h"
#include <algorithm>
#include <iostream>

int Circuit::addComponent(const Component& comp, const sf::Vector2f& canvasPos) {
    int id = nextComponentID++;
    Component c = comp;
    c.position = canvasPos;
    c.id = id;
    c.value = 0;
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

int Circuit::createWire(sf::Vector2f position) {
    wires.emplace(nextWireID, Wire(snapPositionToGrid(position), nextWireID));
    createElectricalNode();
    return nextWireID++;
}

void Circuit::eraseWire(int wireID, bool updateConnectedComponents) {
    auto wireIt = wires.find(wireID);
    if (wireIt == wires.end()) return;

    auto eNodeIt = electricalNodes.find(wireID);
    if (eNodeIt == electricalNodes.end()) {
        wires.erase(wireIt);
        return;
    }

    ElectricalNode& eNode = eNodeIt->second;

    if (updateConnectedComponents) {
        for (ElectricalConnection& conn : eNode.connections) {
            updateComponentLead(-1, -1, -1, conn);
        }
    }


    electricalNodes.erase(eNodeIt);

    wires.erase(wireIt);
}

int Circuit::createElectricalNode() {
    electricalNodes.emplace(nextNodeID, ElectricalNode{ nextNodeID });
    return nextNodeID++;
}

void Circuit::absorbElectricalNode(int primaryID, int absorbedID) {
    ElectricalNode& primary = electricalNodes.at(primaryID);
    ElectricalNode& absorbed = electricalNodes.at(absorbedID);

    for (const ElectricalConnection& conn : absorbed.connections) {
        Component* c = getComponent(conn.componentID);
        if (!c) continue;

        switch (conn.lead) {
        case Lead::A:
            c->nodeA = primaryID;
            break;
        case Lead::B:
            c->nodeB = primaryID;
            break;
        default:
            break;
        }

        primary.connections.push_back(conn);
    }

    electricalNodes.erase(absorbedID);
}

void Circuit::addConnectionToElectricalNode(int nodeID, ElectricalConnection& connection) {
    electricalNodes[nodeID].connections.push_back({ connection });
}

void Circuit::removeConnectionFromElectricalNode(int nodeID, ElectricalConnection& connection) {
    if (!electricalNodes.contains(nodeID)) return;
    electricalNodes[nodeID].connections.erase(std::remove_if(electricalNodes[nodeID].connections.begin(), electricalNodes[nodeID].connections.end(),
        [connection](const ElectricalConnection& c) {
            return c.componentID == connection.componentID;
        }),
        electricalNodes[nodeID].connections.end()
    );
}

void Circuit::updateComponentLead(int wireID, int wireNodeID, int ElectricalNodeID, const ElectricalConnection& connection) {
    Component* comp = getComponent(connection.componentID);
    if (!comp) return;
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

ElectricalNode* Circuit::getElectricalNode(int electricalNodeID) {
    return &electricalNodes.at(electricalNodeID);
}

std::unordered_map<int, ElectricalNode>& Circuit::getElectricalNodes() { return electricalNodes; }

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

