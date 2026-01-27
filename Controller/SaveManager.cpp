#include "SaveManager.h"

SaveManager::SaveManager() { }
//make this implementation neater

bool SaveManager::save(Circuit& circuit, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    out << "CKT_VERSION 1\n\n";

    saveCircuitState(out, circuit);
    saveComponents(out, circuit.getComponents());
    saveWires(out, circuit.getWires());
    saveElectricalNodes(out, circuit.getElectricalNodes());
    saveComponentIndexMap(out, circuit.getComponentIDToIndex());

    return true;
}

bool SaveManager::load(const std::string& filename, CircuitData& outData) {
    std::ifstream in(filename);
    if (!in.is_open()) return false;

    std::string line;

    if (std::getline(in, line) && line.rfind("CKT_VERSION", 0) == 0) {
        // do nothing, just skip
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line == "CIRCUIT_STATE") break; // found the start of metadata
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line == "END") break;

        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "nextComponentID") ss >> outData.nextComponentID;
        else if (token == "nextNodeID") ss >> outData.nextNodeID;
        else if (token == "nextWireID") ss >> outData.nextWireID;
        else if (token == "isSimulating") ss >> outData.isSimulating;
    }

    if (!parseComponents(in, outData.components)) {
        std::cerr << "[Error] Failed to parse components\n";
        return false;
    }

    if (!parseWires(in, outData.wires)) {
        std::cerr << "[Error] Failed to parse wires\n";
        return false;
    }

    if (!parseElectricalNodes(in, outData.electricalNodes)) {
        std::cerr << "[Error] Failed to parse electrical nodes\n";
        return false;
    }

    if (!parseComponentIndexMap(in, outData.componentIDToIndex)) {
        std::cerr << "[Error] Failed to parse component index map\n";
        return false;
    }

    return true;
}

void SaveManager::saveCircuitState(std::ostream& out, const Circuit& circuit) {
    out << "CIRCUIT_STATE\n";
    out << "nextComponentID " << circuit.getNextComponentID() << "\n";
    out << "nextNodeID " << circuit.getNextNodeID() << "\n";
    out << "nextWireID " << circuit.getNextWireID() << "\n";
    out << "END\n\n";
}

void SaveManager::saveComponents(std::ostream& out, const std::vector<Component>& components) {
    out << "COMPONENTS\n";
    out << "COMPONENT_COUNT " << components.size() << "\n\n";

    for (const auto& c : components) {
        out << "COMPONENT\n";
        out << "id " << c.id << "\n";
        out << "nodeA " << c.nodeA << "\n";
        out << "nodeB " << c.nodeB << "\n";
        out << "type " << toInt(c.type) << "\n";
        out << "label " << c.label << "\n";
        out << "value " << c.value << "\n";
        out << "position " << c.position.x << " " << c.position.y << "\n";
        out << "rotation " << c.rotation << "\n";
        out << "current " << c.current << "\n";
        out << "voltage " << c.voltage << "\n";
        out << "isClosed " << c.isClosed << "\n";
        out << "wireA " << c.A_WireNodeReference.wireID << " " << c.A_WireNodeReference.nodeID << "\n";
        out << "wireB " << c.B_WireNodeReference.wireID << " " << c.B_WireNodeReference.nodeID << "\n";
        out << "END\n\n";
    }

    out << "END_COMPONENTS\n\n";
}

void SaveManager::saveWires(std::ostream& out, std::unordered_map<int, Wire>& wires) {
    out << "WIRES\n";
    out << "WIRE_COUNT " << wires.size() << "\n\n";

    for (auto& [wireID, wire] : wires) {
        out << "WIRE\n";
        out << "id " << wire.ID << "\n";
        out << "nextNodeID " << wire.getNextNodeID() << "\n";

        const auto& graph = wire.getGraph();
        out << "NODE_COUNT " << graph.size() << "\n";

        for (const auto& [nodeID, node] : graph) {
            out << "NODE\n";
            out << "id " << node.id << "\n";
            out << "pos " << node.position.x << " " << node.position.y << "\n";
            out << "belongsTo " << node.belongsTo << "\n";
            out << "isAnchor " << node.isAnchor << "\n";

            out << "neighbors";
            for (int n : node.neighbors)
                out << " " << n;
            out << "\n";

            out << "END\n";
        }

        out << "ENDWIRE\n\n";
    }

    out << "END_WIRES\n\n";
}

void SaveManager::saveElectricalNodes(std::ostream& out, const std::unordered_map<int, ElectricalNode>& nodes) {
    out << "ELECTRICAL_NODES\n";
    out << "ELECTRICAL_NODE_COUNT " << nodes.size() << "\n\n";

    for (const auto& [id, node] : nodes) {
        out << "ELECTRICAL_NODE\n";
        out << "id " << node.id << "\n";
        out << "CONNECTION_COUNT " << node.connections.size() << "\n";

        for (const auto& conn : node.connections) {
            out << "CONNECTION " << conn.componentID << " " << static_cast<int>(conn.lead) << "\n";
        }

        out << "END\n\n";
    }

    out << "END_ELECTRICAL_NODES\n\n";
}

void SaveManager::saveComponentIndexMap(std::ostream& out, const std::unordered_map<int, int>& map) {
    out << "COMPONENT_INDEX_MAP\n";
    out << "MAP_COUNT " << map.size() << "\n";

    for (const auto& [componentID, index] : map) {
        out << "MAP " << componentID << " " << index << "\n";
    }

    out << "END_COMPONENT_INDEX_MAP\n";
}

int SaveManager::toInt(ComponentType t) {
    return static_cast<int>(t);
}

ComponentType SaveManager::toComponentType(int v) {
    return static_cast<ComponentType>(v);
}

bool SaveManager::parseComponents(std::istream& in, std::vector<Component>& outComponents) {
    std::string line;

    if (!getlineNonEmpty(in, line) || line != "COMPONENTS") {
        std::cerr << "Expected COMPONENTS\n";
        return false;
    }

    if (!getlineNonEmpty(in, line)) return false;

    std::istringstream ssCount(line);
    std::string token;
    int componentCount = 0;
    ssCount >> token >> componentCount;
    if (token != "COMPONENT_COUNT") {
        std::cerr << "[ERROR] EXPECTED 'COMPONENT_COUNT', got: '" << token << "'\n";
        return false;
    }

    // parse each component
    for (int i = 0; i < componentCount; ++i) {
        // skip blank lines until "COMPONENT"
        do {
            if (!std::getline(in, line)) return false;
            trim(line);
        } while (line.empty());

        if (line != "COMPONENT") {
            std::cerr << "Expected COMPONENT, got '" << line << "'\n";
            return false;
        }

        // temporary component to fill
        Component c(0, 0, ComponentType::Resistor);

        // read all lines until "END"
        while (std::getline(in, line)) {
            trim(line);
            if (line.empty()) continue;
            if (line == "END") break;

            std::istringstream ss(line);
            ss >> token;

            if (token == "id") ss >> c.id;
            else if (token == "nodeA") ss >> c.nodeA;
            else if (token == "nodeB") ss >> c.nodeB;
            else if (token == "type") {
                int t; ss >> t;
                c.type = toComponentType(t);
            }
            else if (token == "label") {
                std::string label;
                std::getline(ss, label);
                if (!label.empty() && label[0] == ' ') label.erase(0, 1);
                c.label = label;
            }
            else if (token == "value") ss >> c.value;
            else if (token == "position") ss >> c.position.x >> c.position.y;
            else if (token == "rotation") ss >> c.rotation;
            else if (token == "current") ss >> c.current;
            else if (token == "voltage") ss >> c.voltage;
            else if (token == "isClosed") ss >> c.isClosed;
            else if (token == "wireA") ss >> c.A_WireNodeReference.wireID >> c.A_WireNodeReference.nodeID;
            else if (token == "wireB") ss >> c.B_WireNodeReference.wireID >> c.B_WireNodeReference.nodeID;
            else {
                std::cerr << "[WARNING] Unknown token in component: " << token << "\n";
            }
        }

        outComponents.push_back(c);
    }

    if (!getlineNonEmpty(in, line) || line != "END_COMPONENTS") {
        std::cerr << "Expected END_COMPONENTS\n";
        return false;
    }

    return true;
}

bool SaveManager::parseWires(std::istream& in, std::unordered_map<int, Wire>& outWires) {
    std::string line;
    // Skip blank lines until "WIRES"
    if (!getlineNonEmpty(in, line) || line != "WIRES") {
        std::cerr << "[Error] Expected WIRES section, got: '" << line << "'\n";
        return false;
    }

    // read WIRE_COUNT
    if (!getlineNonEmpty(in, line)) return false;

    std::istringstream ssCount(line);
    std::string token;
    int wireCount = 0;
    ssCount >> token >> wireCount;
    if (token != "WIRE_COUNT") {
        std::cerr << "[Error] Expected WIRE_COUNT, got: '" << token << "'\n";
        return false;
    }

    for (int i = 0; i < wireCount; ++i) {


        // skip blank lines until "WIRE"
        if (!getlineNonEmpty(in, line) || line != "WIRE") {
            std::cerr << "[Error] Expected WIRE, got: '" << line << "'\n";
            return false;
        }


        int wireID = -1;
        int nextNodeID = 0;
        std::map<int, Node> graph;

        // read wire details until ENDWIRE
        while (std::getline(in, line)) {
            trim(line);
            if (line.empty()) continue;
            if (line == "ENDWIRE") break;

            std::istringstream ss(line);
            ss >> token;

            if (token == "id") ss >> wireID;
            else if (token == "nextNodeID") ss >> nextNodeID;
            else if (token == "NODE_COUNT") {
                int nodeCount; ss >> nodeCount; (void)nodeCount; // unused
            }
            else if (token == "NODE") {
                Node node;
                while (std::getline(in, line)) {
                    trim(line);
                    if (line.empty()) continue;
                    if (line == "END") break;

                    std::istringstream ssNode(line);
                    ssNode >> token;

                    if (token == "id") ssNode >> node.id;
                    else if (token == "pos") ssNode >> node.position.x >> node.position.y;
                    else if (token == "belongsTo") ssNode >> node.belongsTo;
                    else if (token == "isAnchor") ssNode >> node.isAnchor;
                    else if (token == "neighbors") {
                        node.neighbors.clear();
                        int n;
                        while (ssNode >> n) node.neighbors.insert(n);
                    }
                }
                graph[node.id] = node;
            }
        }

        // construct Wire with parsed graph
        outWires.emplace(wireID, Wire({ 0.f, 0.f }, wireID, graph, nextNodeID));
        
    }

    if (!getlineNonEmpty(in, line) || line != "END_WIRES") {
        std::cerr << "[Error] Expected END_WIRES, got: '" << line << "'\n";
        return false;
    }


    return true;
}

bool SaveManager::parseElectricalNodes(std::istream & in, std::unordered_map<int, ElectricalNode>&outNodes) {
    std::string line;


    if (!getlineNonEmpty(in, line) || line != "ELECTRICAL_NODES") {
        std::cerr << "[Error] Expected ELECTRICAL_NODES section, got: '" << line << "'\n";
        return false;
    }


    // read ELECTRICAL_NODE_COUNT
    if (!getlineNonEmpty(in, line)) return false;

    std::istringstream ssCount(line);
    std::string token;
    int nodeCount = 0;
    ssCount >> token >> nodeCount;
    if (token != "ELECTRICAL_NODE_COUNT") {
        std::cerr << "[Error] Expected ELECTRICAL_NODE_COUNT, got: '" << token << "'\n";
        return false;
    }

    for (int i = 0; i < nodeCount; ++i) {
        // skip blank lines until "ELECTRICAL_NODE"
        if (!getlineNonEmpty(in, line) || line != "ELECTRICAL_NODE") {
            std::cerr << "[Error] Expected ELECTRICAL_NODE, got: '" << line << "'\n";
            return false;
        }

        ElectricalNode node;
        int connectionCount = 0;

        while (std::getline(in, line)) {
            trim(line);
            if (line.empty()) continue;
            if (line == "END") break;

            std::istringstream ss(line);
            ss >> token;

            if (token == "id") ss >> node.id;
            else if (token == "CONNECTION_COUNT") {
                ss >> connectionCount;
            }
            else if (token == "CONNECTION") {
                ElectricalConnection conn;
                int leadInt;
                ss >> conn.componentID >> leadInt;
                conn.lead = static_cast<Lead>(leadInt);
                node.connections.push_back(conn);
            }
        }

        outNodes[node.id] = node;
    }


    if (!getlineNonEmpty(in, line) || line != "END_ELECTRICAL_NODES") {
        std::cerr << "[Error] Expected END_ELECTRICAL_NODES, got: '" << line << "'\n";
        return false;
    }

    return true;
}

bool SaveManager::parseComponentIndexMap(std::istream& in, std::unordered_map<int, int>& outMap) {
    std::string line;
    
    // skip blank lines until "COMPONENT_INDEX_MAP"
    if (!getlineNonEmpty(in, line) || line != "COMPONENT_INDEX_MAP") {
        std::cerr << "[Error] Expected COMPONENT_INDEX_MAP section, got: '" << line << "'\n";
        return false;
    }

    

    // read MAP_COUNT
    if (!getlineNonEmpty(in, line)) return false;

    std::istringstream ssCount(line);
    std::string token;
    int mapCount = 0;
    ssCount >> token >> mapCount;
    if (token != "MAP_COUNT") {
        std::cerr << "[Error] Expected MAP_COUNT, got: '" << token << "'\n";
        return false;
    }

    for (int i = 0; i < mapCount; ++i) {
        if (!getlineNonEmpty(in, line)) return false;


        std::istringstream ss(line);
        int componentID, index;
        ss >> token >> componentID >> index;
        if (token != "MAP") {
            std::cerr << "[Error] Expected MAP, got: '" << token << "'\n";
            return false;
        }

        outMap[componentID] = index;
    }

    // skip blank lines until END_COMPONENT_INDEX_MAP
    if (!getlineNonEmpty(in, line) || line != "END_COMPONENT_INDEX_MAP") {
        std::cerr << "[Error] Expected END_COMPONENT_INDEX_MAP, got: '" << line << "'\n";
        return false;
    }

    return true;
}

void SaveManager::trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

bool SaveManager::getlineNonEmpty(std::istream& in, std::string& line) {
    while (std::getline(in, line)) {
        trim(line);
        if (!line.empty()) return true;
    }
    return false;
}
