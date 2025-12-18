#pragma once
#include "UI/Button.h"
#include "Controller/Controller.h"
#include "iomanip"
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
            << "  ID:        " << component.identification_number << '\n'
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
private:
    static inline bool enabled = true; // default on
};


