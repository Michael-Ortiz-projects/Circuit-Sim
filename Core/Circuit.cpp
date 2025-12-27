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

int Circuit::createElectricalNode() {
    nodes.emplace(nextNodeID, ElectricalNode{ nextNodeID });
    return nextNodeID++;
}

void Circuit::removeElectricalNode(int nodeID) {
    nodes.erase(nodeID);
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

void Circuit::updateComponentLead(int nodeID, ElectricalConnection& connection) {
    switch (connection.lead) {
    case Lead::A:
        getComponent(connection.componentID)->nodeA = nodeID;
        return;
    case Lead::B:
        getComponent(connection.componentID)->nodeB = nodeID;
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

