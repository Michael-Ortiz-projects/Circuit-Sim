#pragma once
#include <unordered_map>
#include "NetlistComponent.h"
#include "../UI/SchematicComponent.h"
#include "SimComps/SimulationComponent.h"
#include "Wire.h"
#include "../Config.h"
#include "Simulator.h"


struct CircuitData {
    int nextComponentID = 0;
    int nextNodeID = 0;
    int nextWireID = 0;

    bool isSimulating = false;

    std::vector<NetlistComponent> netlistComponents;
    std::vector<SchematicComponent> schematicComponents;

    std::unordered_map<int, Wire> wires;
    std::unordered_map<int, ElectricalNode> electricalNodes;

    std::unordered_map<int, int> componentIDToIndex;

    void clear() {
        netlistComponents.clear();
        schematicComponents.clear();
        wires.clear();
        electricalNodes.clear();
        componentIDToIndex.clear();

        nextComponentID = 0;
        nextNodeID = 0;
        nextWireID = 0;
        isSimulating = false;
    }
};

class Circuit {
public:
    std::vector<Eigen::VectorXd> simulationResult;

    Circuit(AssetManager& AssetManager);

    int addComponent(ComponentType type, sf::Vector2f position);

    int addNetlistComponent(ComponentType type);
    void addSchematicComponent(const NetlistComponent& comp, const sf::Vector2f& canvasPos);

    bool removeComponent(int compID);

    bool removeNetlistComponent(int compID);
    bool removeSchematicComponent(int compID);

    int createWire(sf::Vector2f position);
    void eraseWire(int wireID, bool updateConnectedComponents);

    int createElectricalNode();
    void absorbElectricalNode(int primaryID, int absorbedID);

    void addConnectionToElectricalNode(int nodeID, ElectricalConnection& connection);
    void removeConnectionFromElectricalNode(int nodeID, ElectricalConnection& connection);

    void updateComponentTerminal(WireNodeReference wireNode, int ElectricalNodeID, const ElectricalConnection& connection);

    void setCircuitData(const CircuitData& data);


    NetlistComponent* getNetlistComponent(int compID);
    NetlistComponent* getNetlistComponent(SchematicComponent comp);
    std::vector<NetlistComponent>& getNetlistComponents();

    SchematicComponent* getSchematicComponent(int compID);
    SchematicComponent* getSchematicComponent(NetlistComponent comp);
    std::vector<SchematicComponent>& getSchematicComponents();

    Wire* getWire(int wireID);
    std::unordered_map<int, Wire>& getWires();

    ElectricalNode* getElectricalNode(int electricalNodeID);
    std::unordered_map<int, ElectricalNode>& getElectricalNodes();

    Simulator& getSimulator();

    std::unordered_map<int, int> getComponentIDToIndex();

    int getNextWireID() const;
    int getNextNodeID() const;
    int getNextComponentID() const;

    bool terminalIsEmpty(ElectricalConnection& connection);
private:

    sf::Vector2f snapPositionToGrid(const sf::Vector2f& position);
    void rebuildComponentIndexMap();
    Simulator simulator;
    AssetManager& assets;

    int nextComponentID = 0;
    int nextNodeID = 0;
    int nextWireID = 0;

    bool isSimulating = false;

    std::vector<NetlistComponent> netlistComponents;
    std::vector<SchematicComponent> schematicComponents;
    std::vector<std::unique_ptr<SimulationComponent>> simComponents;

    

    std::unordered_map<int, Wire> wires;
    std::unordered_map<int, ElectricalNode> electricalNodes;
    std::unordered_map<int, int> componentIDToIndex;
};