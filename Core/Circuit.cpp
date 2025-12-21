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

int Circuit::createElectricalNode() {
    nodes.push_back({ nextNodeID++, {} });
    return nodes.back().id;
}

void Circuit::removeElectricalNode(int nodeID) {
    auto it = std::find(nodes.begin(), nodes.end(), nodeID);
    nodes.erase(it);
}

void Circuit::connectComponentToNodes(int nodeID, int componentID, Lead lead) {
    ElectricalNode* node = getElectricalNode(nodeID);
    if (!node) return;

    node->connections.push_back({ componentID, lead });
}

void Circuit::disconnectComponentFromNodes(int nodeID, int componentID, Lead lead) {
    for (auto& node : nodes) {
        node.connections.erase(std::remove_if(node.connections.begin(), node.connections.end(),
                [&](const ElectricalConnection& c) {
                    return c.componentID == componentID;
                }
            ),
            node.connections.end()
        );
    }
}

int Circuit::addWireSegment(int nodeID, std::vector<sf::Vector2f> points) {
    wires.push_back({ nextWireID++, nodeID, std::move(points) });
    return wires.back().id;
}

void Circuit::removeWireSegment(int wireID) {
    auto it = std::find_if(wires.begin(), wires.end(),
        [wireID](const WireSegment& w) { return w.id == wireID; });
    if (it != wires.end()) {
        wires.erase(it);
    }
}

ElectricalNode* Circuit::getElectricalNode(int nodeID) {
    for (auto& node : nodes) {
        if (node.id == nodeID) return &node;
    }
    return nullptr;
}

WireSegment* Circuit::getWireSegment(int wireID) {
    for (auto& wire : wires) {
        if (wire.id == wireID) return &wire;
    }
    return nullptr;
}