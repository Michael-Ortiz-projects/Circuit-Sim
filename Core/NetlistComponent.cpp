#include "NetlistComponent.h"

NetlistComponent::NetlistComponent(const ComponentType& ty, double val)
    : type(ty), value(val), expressionString(std::to_string(val)) { 
    switch (type) {
    case ComponentType::VoltageSource:
    case ComponentType::CurrentSource:
    case ComponentType::Resistor:
    case ComponentType::Capacitor:
    case ComponentType::Inductor:
    case ComponentType::Switch:
    case ComponentType::ACCurrentSource:
    case ComponentType::ACVoltageSource:
        terminals.push_back({ 0, -1, PinRole::Negative });
        terminals.push_back({ 1, -1, PinRole::Positive });
        break;

    case ComponentType::VCVS:
    case ComponentType::VCCS:
    case ComponentType::CCVS:
    case ComponentType::CCCS:
        terminals.push_back({ 0, -1, PinRole::Negative });
        terminals.push_back({ 1, -1, PinRole::Positive });
        terminals.push_back({ 2, -1, PinRole::ControlNegative});
        terminals.push_back({ 3, -1, PinRole::ControlPositive});
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