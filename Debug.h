#pragma once
#include "UI/Button.h"
#include "Core/NetlistComponent.h"
#include "iomanip"
#include "Core/ElectricalNode.h"
#include "Core/Wire.h"
class Debug
{
public:
    static void setEnabled(bool e) { enabled = e; }

    static void UICommand(EditorUICommand cmd) {
        if (!enabled) return;
        switch (cmd) {
        case EditorUICommand::None: std::cout << "EditorUI Command = None\n\n"; break;
        case EditorUICommand::ToggleMenu: std::cout << "EditorUI Command = ToggleMenu\n\n"; break;
        case EditorUICommand::PlaceVoltageSource: std::cout << "EditorUI Command = PlaceVoltageSource\n\n"; break;
        case EditorUICommand::PlaceResistor: std::cout << "EditorUI Command = PlaceResistor\n\n"; break;
        case EditorUICommand::PlaceGround: std::cout << "EditorUI Command = PlaceGround\n\n"; break;

        case EditorUICommand::ApplyEdit: std::cout << "EditorUI Command = ApplyEdit\n\n"; break;
        case EditorUICommand::CancelEdit: std::cout << "EditorUI Command = CancelEdit\n\n"; break;
        }
    }

    static void setHandler(const char* name) {
        std::cout << "[Controller] Handler set: " << name << "\n\n";
    }

    static void componentData(NetlistComponent component) {
        std::cout
            << "Component {\n"
            << "  ID:           " << component.id << '\n'
            << "  Type:      " << ComponentType_to_String(component.type) << '\n'
            << "  Terminals:  \n     ";
        for (NetlistTerminal& T : component.terminals) {
            std::cout << T.terminalID << " -> " << T.electricalNode << "\n     ";
        }
        
        std::cout
            << "\n  Value:     " << component.value << '\n'
            
            << "}\n";
    }


    static std::string ComponentType_to_String(ComponentType type) {
        switch (type) {
        case ComponentType::VoltageSource:
            return "Voltage Source";
        case ComponentType::Resistor:
            return "Resistor";
        case ComponentType::CurrentSource:
            return "Current Source";
        case ComponentType::VCVS:
            return "VCVS";
        case ComponentType::VCCS:
            return "VCCS";
        case ComponentType::CCVS:
            return "CCVS";
        case ComponentType::CCCS:
            return "CCCS";
        case ComponentType::Capacitor:
            return "Capacitor";
        case ComponentType::Inductor:
            return "Inductor";
        case ComponentType::Switch:
            return "Switch";
        case ComponentType::Ground:
            return "Ground";
        case ComponentType::ACVoltageSource:
            return "ACVS";
        case ComponentType::ACCurrentSource:
            return "ACCS";
        }
    }

    static std::string printVector2f(sf::Vector2f vector) {
        std::cout << "(" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + ")\n";
        return "(" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + ")\n";
    }

    static void debugPrintView(const sf::View& view) {
        auto c = view.getCenter();
        auto s = view.getSize();
        auto vp = view.getViewport();

        std::cout
            << "View center: (" << c.x << ", " << c.y << ")\n"
            << "View size:   (" << s.x << ", " << s.y << ")\n"
            << "Viewport:   (" << vp.left << ", " << vp.top
            << ", " << vp.width << ", " << vp.height << ")\n\n";
    }

    static void debugPrintWire(Wire& wire) {
        std::cout << "Wire ID: " << wire.ID << "\n";
        std::cout << "Next Node ID: " << wire.getNextNodeID() << "\n";
        std::cout << "Current Stem Node: " << wire.getStemNode() << "\n";
        std::cout << "Graph nodes:\n";

        for (const auto& [nodeID, node] : wire.getGraph()) {
            std::cout << "  Node " << nodeID
                << " at (" << node.position.x << ", " << node.position.y << ")";

            bool hasNeighbors = false;
            for (int neighborID : node.neighbors) {
                if (neighborID != -1) {
                    if (!hasNeighbors) {
                        std::cout << " -> Neighbors: ";
                        hasNeighbors = true;
                    }
                    std::cout << neighborID << "   ";
                }
            }

            if (wire.isAnchor(nodeID)) {
                std::cout << "Anchor Node ";
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    static void printElectricalNode(const ElectricalNode& node) {
        std::cout << "ElectricalNode ID: " << node.id << '\n';

        if (node.connections.empty()) {
            std::cout << "  Connections: (none)\n";
            return;
        }

        std::cout << "  Connections:\n";
        for (const auto& conn : node.connections) {
            std::cout << "    -> Component "
                << conn.componentID
                << ", Terminal "
                << conn.terminalID
                << '\n';
        }
    }

private:
    static inline bool enabled = true;
};


