#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "Component.h"
#include "Wire.h"
#include "ElectricalNode.h"
#include "../Config.h"

class Circuit {
public:
    Circuit() = default;

    int addComponent(const Component& comp, const sf::Vector2f& canvasPos);
    bool removeComponent(int compID);

    Component* getComponent(int compID);
    std::vector<Component>& getComponents();

    Wire* getWire(int wireID);
    std::unordered_map<int, Wire>& getWires();

    int createElectricalNode();
    void removeElectricalNode(int nodeID);

    void addConnectionToNode(int nodeID, ElectricalConnection& connection);
    void removeConnectionFromNode(int nodeID, ElectricalConnection& connection);

    void updateComponentLead(int nodeID, ElectricalConnection& connection);

    int createWire(sf::Vector2f position);

    bool leadIsEmpty(ElectricalConnection& connection);


private:

    sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);

    int nextComponentID = 0;
    int nextNodeID = 0;
    int nextWireID = 0;

    bool isSimulating = false;

    std::vector<Component> components;
    std::unordered_map<int, ElectricalNode> nodes;
    std::unordered_map<int, Wire> wires;

    std::unordered_map<int, int> componentIDToIndex; // maps ID's to Component Vector
    
   
};