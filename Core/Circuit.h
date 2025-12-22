#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "Component.h"
#include "Wire.h"
#include "ElectricalNode.h"

class Circuit {
public:
    Circuit() = default;

    int addComponent(const Component& comp, const sf::Vector2f& canvasPos);
    bool removeComponent(int compID);

    Component* getComponent(int compID);
    std::vector<Component>& getComponents();

    Wire* getWire(int wireID);
    std::vector<Wire>& getWires();

    int createElectricalNode();
    void removeElectricalNode(int nodeID);

    void addConnectionToNode(int nodeID, int componentID, Lead lead);
    void removeConnectionFromNode(int nodeID, int componentID, Lead lead);


    int createWire(sf::Vector2f& position);

    bool leadIsEmpty(ElectricalConnection& connection);


private:
    int nextComponentID = 0;
    int nextNodeID = 0;
    int nextWireID = 0;

    bool isSimulating = false;

    std::vector<Component> components;
    std::vector<ElectricalNode> nodes;
    std::vector<Wire> wires;

    std::unordered_map<int, int> componentIDToIndex; // maps ID's to Component Vector
    std::unordered_map<int, int> wireIDToIndex; // maps wireID to Wire Vector (not started)
    std::unordered_map<int, int> electricalNodeIDToIndex; // maps ID to ElectricalNode Vector (not started)
    /*
  
    reuse old wire system in a sense because working with a graph of nodes is the superior way to think about a wire.
    A map of nodes each with a posiiton and a vector of the indexes to neighboring nodes
    Keep electrical nodes though, thats going to keep it clean so that you dont mix electrical connections with wire merging logic
    

    when 
    */
};