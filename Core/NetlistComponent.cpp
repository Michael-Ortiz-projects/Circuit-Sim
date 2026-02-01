#include "NetlistComponent.h"

NetlistComponent::NetlistComponent(const ComponentType& ty, double val)
    : type(ty), value(val) { 
    switch (type) {
    case ComponentType::VoltageSource:
    case ComponentType::CurrentSource:
    case ComponentType::Resistor:
    case ComponentType::Capacitor:
    case ComponentType::Inductor:
    case ComponentType::Switch:
        terminals.push_back({ 0, -1, PinRole::Negative });
        terminals.push_back({ 1, -1, PinRole::Positive });
        break;
    case ComponentType::Ground:
        terminals.push_back({ 0, -1, PinRole::Negative });
        break;
    }
}

NetlistComponent::NetlistComponent() { }

void NetlistComponent::setType(ComponentType Type) {
    type = Type;
}

void NetlistComponent::setValue(double Value) {
    value = Value;
}

bool NetlistComponent::terminalValid(int terminalID) {
    auto it = std::find_if(terminals.begin(), terminals.end(), [&](const NetlistTerminal& terminal) {return terminal.terminalID == terminalID;});
    return it != terminals.end();
}