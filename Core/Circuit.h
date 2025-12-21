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


    int createElectricalNode();
    void removeElectricalNode(int nodeID);

    void connectComponentToNodes(int nodeID, int componentID, Lead lead);
    void disconnectComponentFromNodes(int nodeID, int componentID, Lead lead);

    int addWireSegment(int nodeID, std::vector<sf::Vector2f> points);
    void removeWireSegment(int wireID);

    ElectricalNode* getElectricalNode(int nodeID);
    WireSegment* getWireSegment(int wireID);
private:
    int nextComponentID = 0;
    int nextNodeID = 0;
    int nextWireID = 0;

    bool isSimulating = false;

    std::vector<Component> components;
    std::vector<ElectricalNode> nodes;
    std::vector<std::vector<WireSegment>> wires;

    std::unordered_map<int, int> componentIDToIndex;
    /*
    each wire segment contains an electrical ID that matches with a corresponding electrical node
    and a normal ID to reference the segment
    segments have a set of continous points such that there is a start and an end.
    instead of worrying about which wire nodes need to split off to make a joint, merge wire segments into the same electrical node
    so that each "Wire" is made of several wire segments. 

    when merging segments im going to need to figure out a system to clean the wire and trim collinear nodes and 
    Dont store the wire segments in the electrical node, they should be separate.

    SCRAP THE ABOVE 

    reuse old wire system in a sense because working with a graph of nodes is the superior way to think about a wire.
    A map of nodes each with a posiiton and a vector of the indexes to neighboring nodes
    Keep electrical nodes though, thats going to keep it clean so that you dont mix electrical connections with wire merging logic
    

    when 
    */
};