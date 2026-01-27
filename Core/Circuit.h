#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "Component.h"
#include "Wire.h"
#include "ElectricalNode.h"
#include "../Config.h"
struct CircuitData {
    int nextComponentID;
    int nextNodeID;
    int nextWireID;
    bool isSimulating = false;

    std::vector<Component> components;
    std::unordered_map<int, Wire> wires;
    std::unordered_map<int, ElectricalNode> electricalNodes;
    std::unordered_map<int, int> componentIDToIndex;
};

class Circuit {
public:
    Circuit() = default;

    int addComponent(const Component& comp, const sf::Vector2f& canvasPos);
    bool removeComponent(int compID);

    int createWire(sf::Vector2f position);
    void eraseWire(int wireID, bool updateConnectedComponents);

    int createElectricalNode();
    void absorbElectricalNode(int primaryID, int absorbedID);

    void addConnectionToElectricalNode(int nodeID, ElectricalConnection& connection);
    void removeConnectionFromElectricalNode(int nodeID, ElectricalConnection& connection);

    void updateComponentLead(int wireID, int wireNodeID, int ElectricalNodeID, const ElectricalConnection& connection);

    void setCircuitData(const CircuitData& data);


    Component* getComponent(int compID);
    std::vector<Component>& getComponents();

    Wire* getWire(int wireID);
    std::unordered_map<int, Wire>& getWires();

    ElectricalNode* getElectricalNode(int electricalNodeID);
    std::unordered_map<int, ElectricalNode>& getElectricalNodes();

    std::unordered_map<int, int> getComponentIDToIndex();

    int getNextWireID() const;
    int getNextNodeID() const;
    int getNextComponentID() const;

    bool leadIsEmpty(ElectricalConnection& connection);


    
private:

    sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);


    int nextComponentID = 0;
    int nextNodeID = 0;
    int nextWireID = 0;

    bool isSimulating = false;

    std::vector<Component> components;
    std::unordered_map<int, Wire> wires;
    std::unordered_map<int, ElectricalNode> electricalNodes;
    std::unordered_map<int, int> componentIDToIndex;
};