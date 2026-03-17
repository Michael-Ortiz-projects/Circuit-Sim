#include "SaveManager.h"

SaveManager::SaveManager() { }
//make this implementation neater

bool SaveManager::saveCircuitToFile(Circuit& circuit, const std::string& filePath) {
    std::ofstream out(filePath);
    if (!out.is_open())
        return false;

    // ================= HEADER =================
    out << "CIRCUIT\n";
    out << "NEXT_IDS "
        << circuit.getNextComponentID() << " "
        << circuit.getNextNodeID() << " "
        << circuit.getNextWireID() << "\n";

    // ================= NETLIST COMPONENTS =================
    auto& netlist = circuit.getNetlistComponents();
    out << "\nNETLIST_COMPONENTS " << netlist.size() << "\n";

    for (const auto& comp : netlist) {
        out << "COMPONENT "
            << comp.id << " "
            << static_cast<int>(comp.type) << " "
            << comp.value << " "
            << std::quoted(comp.label) << " "
            << std::quoted(comp.expressionString) << "\n";

        out << "TERMINALS " << comp.terminals.size() << "\n";
        for (const auto& t : comp.terminals) {
            out << "TERMINAL "
                << t.terminalID << " "
                << t.electricalNode << " "
                << static_cast<int>(t.role) << "\n";
        }
    }

    // ================= SCHEMATIC COMPONENTS =================
    auto& schematic = circuit.getSchematicComponents();
    out << "\nSCHEMATIC_COMPONENTS " << schematic.size() << "\n";

    for (auto& comp : schematic) {
        const sf::Vector2f& pos = comp.getPosition();

        out << "SCHEMATIC "
            << comp.componentID << " "
            << pos.x << " "
            << pos.y << " "
            << comp.getRotation() << " "
            << std::quoted(comp.getLabel()) << " "
            << comp.getValue() << "\n";

        out << "TERMINALS " << comp.schematicTerminals.size() << "\n";
        for (const auto& t : comp.schematicTerminals) {
            out << "TERMINAL "
                << t.terminalID << " "
                << t.offset.x << " "
                << t.offset.y << " "
                << t.wireNodeReference.wireID << " "
                << t.wireNodeReference.nodeID << "\n";
        }
    }

    // ================= WIRES =================
    auto& wires = circuit.getWires();
    out << "\nWIRES " << wires.size() << "\n";

    for (auto& [wireID, wire] : wires) {
        out << "WIRE " << wireID << " " << wire.getNextNodeID() << "\n";

        const auto& graph = wire.getGraph();
        out << "NODES " << graph.size() << "\n";

        for (const auto& [nodeID, node] : graph) {
            out << "NODE "
                << node.id << " "
                << node.position.x << " "
                << node.position.y << " "
                << node.belongsTo << " "
                << node.isAnchor << "\n";

            out << "NEIGHBORS " << node.neighbors.size();
            for (int n : node.neighbors)
                out << " " << n;
            out << "\n";
        }
    }

    // ================= ELECTRICAL NODES =================
    auto& eNodes = circuit.getElectricalNodes();
    out << "\nELECTRICAL_NODES " << eNodes.size() << "\n";

    for (const auto& [id, node] : eNodes) {
        out << "ENODE " << node.id << "\n";
        out << "CONNECTIONS " << node.connections.size() << "\n";

        for (const auto& c : node.connections) {
            out << "CONN " << c.componentID << " " << c.terminalID << "\n";
        }
    }

    // ================= COMPONENT INDEX MAP =================
    auto indexMap = circuit.getComponentIDToIndex();
    out << "\nCOMPONENT_INDEX_MAP " << indexMap.size() << "\n";

    for (const auto& [compID, index] : indexMap) {
        out << "MAP " << compID << " " << index << "\n";
    }

    out.close();
    return true;
}

bool SaveManager::loadCircuitFromFile(CircuitData& circuitData, const std::string& filePath, AssetManager& assets) {
    std::cout << "loadCircuitFromFile() is running\n";
    std::ifstream in(filePath);
    if (!in.is_open()) {
        std::cout << "failed to open filepath " << filePath << "\n";
        return false;

    }

    circuitData.clear();

    std::string token;
    std::cout << "debug1\n";

    // ================= HEADER =================
    if (!expectToken(in, "CIRCUIT")) return false;

    if (!expectToken(in, "NEXT_IDS")) return false;
    in >> circuitData.nextComponentID >> circuitData.nextNodeID >> circuitData.nextWireID;

    // ================= NETLIST COMPONENTS =================
    if (!expectToken(in, "NETLIST_COMPONENTS")) return false;

    int netlistCount;
    in >> netlistCount;
    std::cout << "debug2\n";

    for (int i = 0; i < netlistCount; ++i) {

        if (!expectToken(in, "COMPONENT")) return false;
        NetlistComponent comp;
        int typeInt;


        in >> comp.id >> typeInt >> comp.value >> std::quoted(comp.label) >> std::quoted(comp.expressionString);
        comp.type = static_cast<ComponentType>(typeInt);

        if (!expectToken(in, "TERMINALS")) return false;

        int terminalCount;
        in >> terminalCount;

        for (int t = 0; t < terminalCount; ++t) {
            if (!expectToken(in, "TERMINAL")) return false;

            NetlistTerminal term;
            int roleInt;

            in >> term.terminalID >> term.electricalNode >> roleInt;
            term.role = static_cast<PinRole>(roleInt);

            comp.terminals.push_back(term);
        }

        circuitData.netlistComponents.push_back(comp);
    }

    std::cout << "debug3\n";

    // ================= SCHEMATIC COMPONENTS =================
    if (!expectToken(in, "SCHEMATIC_COMPONENTS")) return false;

    int schematicCount;
    in >> schematicCount;

    for (int i = 0; i < schematicCount; ++i) {

        if (!expectToken(in, "SCHEMATIC")) return false;

        int compID;
        sf::Vector2f pos;
        float rotation;
        std::string label;
        double value;

        in >> compID >> pos.x >> pos.y >> rotation >> std::quoted(label) >> value;

        auto it = std::find_if(
            circuitData.netlistComponents.begin(),
            circuitData.netlistComponents.end(),
            [&](const NetlistComponent& c) { return c.id == compID; }
        );

        if (it == circuitData.netlistComponents.end()) {
            std::cout << "schematic without netlist is invalid\n";
            return false;
        }

        SchematicComponent sc(*it, pos, assets.mainFont);

        sc.setRotation(rotation);
        sc.setLabel(label);
        sc.setValue(value);

        if (!expectToken(in, "TERMINALS")) return false;

        int termCount;
        in >> termCount;
        sc.schematicTerminals.clear();
        for (int t = 0; t < termCount; ++t) {
            if (!expectToken(in, "TERMINAL")) return false;

            SchematicTerminal st;
            in >> st.terminalID
                >> st.offset.x
                >> st.offset.y
                >> st.wireNodeReference.wireID
                >> st.wireNodeReference.nodeID;
            
            sc.schematicTerminals.push_back(st);
        }
        sc.setTexture(assets.getTexture(sc.getType()));
        circuitData.schematicComponents.push_back(sc);
    }
    std::cout << "debug4\n";


    // ================= WIRES =================
    if (!expectToken(in, "WIRES")) return false;

    int wireCount;
    in >> wireCount;

    for (int i = 0; i < wireCount; ++i) {
        if (!expectToken(in, "WIRE")) return false;

        int wireID, nextNodeID;
        in >> wireID >> nextNodeID;

        if (!expectToken(in, "NODES")) return false;

        int nodeCount;
        in >> nodeCount;

        std::map<int, Node> graph;

        for (int n = 0; n < nodeCount; ++n) {
            if (!expectToken(in, "NODE")) return false;

            Node node;
            in >> node.id
                >> node.position.x
                >> node.position.y
                >> node.belongsTo
                >> node.isAnchor;

            if (!expectToken(in, "NEIGHBORS")) return false;

            int neighborCount;
            in >> neighborCount;

            for (int k = 0; k < neighborCount; ++k) {
                int nb;
                in >> nb;
                node.neighbors.insert(nb);
            }

            graph[node.id] = node;
        }

        Wire wire(wireID, graph, nextNodeID);
        circuitData.wires.emplace(wireID, std::move(wire));
    }

    std::cout << "debug5\n";

    // ================= ELECTRICAL NODES =================
    if (!expectToken(in, "ELECTRICAL_NODES")) return false;

    int eNodeCount;
    in >> eNodeCount;

    for (int i = 0; i < eNodeCount; ++i) {
        if (!expectToken(in, "ENODE")) return false;

        int id;
        in >> id;

        ElectricalNode node(id);

        if (!expectToken(in, "CONNECTIONS")) return false;

        int connCount;
        in >> connCount;

        for (int c = 0; c < connCount; ++c) {
            if (!expectToken(in, "CONN")) return false;

            ElectricalConnection ec;
            in >> ec.componentID >> ec.terminalID;
            node.connections.push_back(ec);
        }

        circuitData.electricalNodes[id] = node;
    }

    std::cout << "debug6\n";


    // ================= COMPONENT INDEX MAP =================
    if (!expectToken(in, "COMPONENT_INDEX_MAP")) return false;

    int mapCount;
    in >> mapCount;

    for (int i = 0; i < mapCount; ++i) {
        if (!expectToken(in, "MAP")) return false;

        int compID, index;
        in >> compID >> index;
        circuitData.componentIDToIndex[compID] = index;
    }
    std::cout << "loadCircuitFromFile() finished\n";
    return true;
}

bool SaveManager::expectToken(std::istream& in, const std::string& expected) {
    std::string token;
    if (!(in >> token)) return false;
    return token == expected;
}