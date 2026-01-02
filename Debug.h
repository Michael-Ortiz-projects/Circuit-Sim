#pragma once
#include "UI/Button.h"
#include "Core/Component.h"
#include "iomanip"
#include "Core/ElectricalNode.h"
#include "Core/Wire.h"
class Debug
{
public:
    static void setEnabled(bool e) { enabled = e; }

    static void UICommand(UICommand cmd) {
        if (!enabled) return;
        switch (cmd) {
        case UICommand::None: std::cout << "UI Command = None\n\n"; break;
        case UICommand::ToggleMenu: std::cout << "UI Command = ToggleMenu\n\n"; break;
        case UICommand::PlaceVoltageSource: std::cout << "UI Command = PlaceVoltageSource\n\n"; break;
        case UICommand::PlaceResistor: std::cout << "UI Command = PlaceResistor\n\n"; break;
        }
    }

    static void setHandler(const char* name) {
        std::cout << "[Controller] Handler set: " << name << "\n\n";
    }

    static void componentData(Component component) {
        std::cout
            << "Component {\n"
            << "  ID:        " << '\n'
            << "  Type:      " << ComponentType_to_String(component.type) << '\n'
            << "  Nodes:     " << component.nodeA << " -> " << component.nodeB << '\n'
            << "  Value:     " << component.value << '\n'
            << "  Voltage:   " << std::fixed << std::setprecision(4) << component.voltage << " V\n"
            << "  Current:   " << std::fixed << std::setprecision(6) << component.current << " A\n"
            << "  Closed:    " << (component.isClosed ? "true" : "false") << '\n'
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
        case ComponentType::Capacitor:
            return "Capacitor";
        case ComponentType::Inductor:
            return "Inductor";
        case ComponentType::Switch:
            return "Switch";
        }
    }

    static std::string lead_to_string(Lead lead) {
        switch (lead) {
        case Lead::A:
            return "A";

        case Lead::B:
            return "B";

        case Lead::Null:
            return "Null";
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

            if (!node.neighbors.empty()) {
                std::cout << " -> Neighbors: ";
                for (int neighborID : node.neighbors) {
                    std::cout << neighborID << " ";
                }
            }
            if (wire.isJunction(nodeID)) {
                std::cout << " Junction Node ";
            }
            if (wire.isAnchor(nodeID)) {
                std::cout << "Anchor Node ";
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }
private:
    static inline bool enabled = true;
};


