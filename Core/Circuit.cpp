#include "Circuit.h"
#include <algorithm>
#include <iostream>

Circuit::Circuit(AssetManager& AssetManager)
    : assets(AssetManager), simulator(simComponents) { }

int Circuit::addComponent(ComponentType type, sf::Vector2f position) {
    std::cout << "addComponent() started\n";
    int newCompID = addNetlistComponent(type);
    std::cout << "Debug A\n";
    addSchematicComponent(*getNetlistComponent(newCompID), position);
    std::cout << "Debug B\n";
    return newCompID;
    
}
int Circuit::addNetlistComponent(ComponentType type) {
    std::cout << "addNetlistComponent() started\n";
    NetlistComponent c(type, 0);
    std::cout << "Debug X\n";
    c.id = nextComponentID++;
    c.label = Debug::ComponentType_to_String(type);
    std::cout << "Debug Y\n";

    componentIDToIndex[c.id] = static_cast<int>(netlistComponents.size());
    netlistComponents.push_back(c);
    std::cout << "Debug Z\n";

    return c.id;
}

void Circuit::addSchematicComponent(const NetlistComponent& comp, const sf::Vector2f& canvasPos) {
    SchematicComponent c(comp, canvasPos, assets.mainFont);
    schematicComponents.push_back(c);
}

bool Circuit::removeComponent(int compID) {
    return removeNetlistComponent(compID);
}

bool Circuit::removeNetlistComponent(int compID) {
    NetlistComponent* comp = getNetlistComponent(compID);
    if (!comp) return false;

    for (NetlistTerminal& t : comp->terminals) {
        ElectricalConnection connection(compID, t.terminalID);
        removeConnectionFromElectricalNode(t.electricalNode,connection);
    }

    removeSchematicComponent(compID);

    auto it = std::find_if(netlistComponents.begin(), netlistComponents.end(),
        [&](const NetlistComponent& c) { return c.id == compID; });

    netlistComponents.erase(it);

    rebuildComponentIndexMap();

    return true;
}

bool Circuit::removeSchematicComponent(int compID) {
    size_t before = schematicComponents.size();

    schematicComponents.erase(
        std::remove_if(
            schematicComponents.begin(),
            schematicComponents.end(),
            [&](const SchematicComponent& s) {
                return s.componentID == compID;
            }),
        schematicComponents.end()
    );

    return schematicComponents.size() != before;
}


int Circuit::createWire(sf::Vector2f position) {
    wires.emplace(nextWireID, Wire(snapPositionToGrid(position), nextWireID));
    createElectricalNode();
    return nextWireID++;
}

void Circuit::eraseWire(int wireID, bool updateConnectedComponents) {
    auto wireIt = wires.find(wireID);
    if (wireIt == wires.end()) return;
    //std::cout << "Erase Wire Debug Section 1\n";
    auto eNodeIt = electricalNodes.find(wireID);
    if (eNodeIt != electricalNodes.end()) {
        //std::cout << "Erase Wire Debug Section 2\n";

        ElectricalNode& eNode = eNodeIt->second;
        //std::cout << "Erase Wire Debug Section 3\n";

        if (updateConnectedComponents) {
            //std::cout << "Erase Wire Debug Section 4\n";

            auto connections = eNode.connections; // COPY
            //std::cout << "Erase Wire Debug Section 5\n";

            for (const ElectricalConnection& conn : connections) {
                //std::cout << "Erase Wire Debug Section 6\n";

                if (getNetlistComponent(conn.componentID)) {
                    //std::cout << "Erase Wire Debug Section 7\n";

                    NetlistComponent* ncomp = getNetlistComponent(conn.componentID);
                    if (!ncomp) continue;

                    // detach terminal
                    NetlistTerminal& term = ncomp->terminals.at(conn.terminalID);
                    term.electricalNode = -1;

                    // update schematic
                    if (SchematicComponent* s = getSchematicComponent(conn.componentID)) {
                        s->schematicTerminals.at(conn.terminalID).wireNodeReference = { -1, -1 };
                    }
                    //std::cout << "Erase Wire Debug Section 8\n";

                }
            }
        }
        //std::cout << "Erase Wire Debug Section 9\n";


        electricalNodes.erase(eNodeIt);
    }
    //std::cout << "Erase Wire Debug Section 9\n";

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
        NetlistComponent* c = getNetlistComponent(conn.componentID);
        if (!c) continue;
        c->terminals.at(conn.terminalID).electricalNode = primaryID;

        primary.connections.push_back(conn);
    }

    electricalNodes.erase(absorbedID);
}

void Circuit::addConnectionToElectricalNode(int nodeID, ElectricalConnection& connection) {
    electricalNodes.at(nodeID).connections.push_back({connection});
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

void Circuit::updateComponentTerminal(WireNodeReference wireNode, int ElectricalNodeID, const ElectricalConnection& connection) {
    int componentID = connection.componentID;
    int terminalID = connection.terminalID;
    int wireID = wireNode.wireID;
    int nodeID = wireNode.nodeID;
    NetlistComponent* ncomp = getNetlistComponent(componentID);
    if (!ncomp) return;

    SchematicComponent* scomp = getSchematicComponent(*ncomp);

    if (ncomp->terminalValid(terminalID)) {
        ncomp->terminals.at(terminalID).electricalNode = ElectricalNodeID;
        scomp->schematicTerminals.at(terminalID).wireNodeReference = wireNode;
        if (wires.find(wireID) != wires.end()) {
            auto& graph = wires.at(wireID).getGraph();
            if (graph.find(nodeID) != graph.end()) {
                graph.at(nodeID).isAnchor = true;
            }
        }
    }
}

void Circuit::setCircuitData(const CircuitData& data) {
    netlistComponents.clear();
    schematicComponents.clear();
    wires.clear();
    electricalNodes.clear();
    componentIDToIndex.clear();

    nextComponentID = data.nextComponentID;
    nextNodeID = data.nextNodeID;
    nextWireID = data.nextWireID;
    isSimulating = data.isSimulating;

    netlistComponents = data.netlistComponents;
    schematicComponents = data.schematicComponents;

    wires = data.wires;
    electricalNodes = data.electricalNodes;

    componentIDToIndex = data.componentIDToIndex;


    if (componentIDToIndex.empty() && !netlistComponents.empty()) {
        rebuildComponentIndexMap();
    }
}


NetlistComponent* Circuit::getNetlistComponent(int compID) {
    auto it = componentIDToIndex.find(compID);
    if (it == componentIDToIndex.end()) return nullptr;
    return &netlistComponents[it->second];
}
NetlistComponent* Circuit::getNetlistComponent(SchematicComponent comp) {
    auto it = componentIDToIndex.find(comp.componentID);
    if (it == componentIDToIndex.end()) return nullptr;
    return &netlistComponents[it->second];
}

std::vector<NetlistComponent>& Circuit::getNetlistComponents() {
    return netlistComponents;
}

SchematicComponent* Circuit::getSchematicComponent(int compID) {
    auto it = componentIDToIndex.find(compID);
    if (it == componentIDToIndex.end()) return nullptr;
    return &schematicComponents[it->second];
}

SchematicComponent* Circuit::getSchematicComponent(NetlistComponent comp) {
    auto it = componentIDToIndex.find(comp.id);
    if (it == componentIDToIndex.end()) return nullptr;
    return &schematicComponents[it->second];
}

std::vector<SchematicComponent>& Circuit::getSchematicComponents() {
    return schematicComponents;
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

Simulator& Circuit::getSimulator() { return simulator; }


std::unordered_map<int, int> Circuit::getComponentIDToIndex() {
    return componentIDToIndex;
}
int Circuit::getNextWireID() const {
    return nextWireID;
}

int Circuit::getNextNodeID() const {
    return nextNodeID;
}

int Circuit::getNextComponentID() const {
    return nextComponentID;
}

bool Circuit::terminalIsEmpty(ElectricalConnection& connection) { // returns false if connection is not valid
    NetlistComponent* c = getNetlistComponent(connection.componentID);
    std::cout << "terminalIsEmpty(), connection.terminalID = " << connection.terminalID << "  Terminals size = " << c->terminals.size() << "\n";
    return c && c->terminals.at(connection.terminalID).electricalNode == -1;

}


sf::Vector2f Circuit::snapPositionToGrid(const sf::Vector2f& position) {
    return {
        std::round(position.x / gridSize) * gridSize,
        std::round(position.y / gridSize) * gridSize
    };
}

void Circuit::rebuildComponentIndexMap() {
    componentIDToIndex.clear();
    componentIDToIndex.reserve(netlistComponents.size());

    for (size_t i = 0; i < netlistComponents.size(); ++i) {
        componentIDToIndex[netlistComponents[i].id] = i;
    }
}