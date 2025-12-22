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
    auto it = std::find_if(wires.begin(), wires.end(),
        [wireID](const Wire& w) {
            return w.ID == wireID;
        }
    );

    /* I want to do 
    auto it = wireIDToIndex.find(wireID);
    if (it == wireIDToIndex.end()) return nullptr;
    return &wires[it->second];
    but I need to implement the wireIDToIndex map
    */

    //PICKUP HERE:
    /* NEED to continue on wirehandler, work with wire ID maps to finish getWire function so that
       WireHandler can set the active wire along with the future when syncing ElectricalNode vector to the Wire vector*/
}

std::vector<Wire>& Circuit::getWires() {
    return wires;
}

int Circuit::createElectricalNode() {
    nodes.push_back(ElectricalNode(nextNodeID++));
    return nodes.back().id;
}

void Circuit::removeElectricalNode(int nodeID) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
        [nodeID](const ElectricalNode& node) {
            return node.id == nodeID;
        });
    nodes.erase(it);
}

void Circuit::addConnectionToNode(int nodeID, int componentID, Lead lead) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
        [nodeID](const ElectricalNode& n) { return n.id == nodeID; });

    if (it != nodes.end()) {
        it->connections.push_back({ componentID, lead });
    }
}

void Circuit::removeConnectionFromNode(int nodeID, int componentID, Lead lead) {
    for (auto& node : nodes) {
        node.connections.erase(std::remove_if(node.connections.begin(), node.connections.end(),
                [componentID](const ElectricalConnection& c) {
                    return c.componentID == componentID;
                }
            ),
            node.connections.end()
        );
    }
}

int Circuit::createWire(sf::Vector2f& position) {
    wires.push_back(Wire(position, nextWireID));
    wireIDToIndex[nextWireID] = wires.size(); // check if this is right. It technically is but i need to see where im going to ultimately use this and if i need to change it
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
