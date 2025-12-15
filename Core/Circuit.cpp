#include "Circuit.h"

int Circuit::AddComponent(const ComponentType& type) {
    Component temp_component;
    
    switch (type) {
    case ComponentType::Resistor:
        temp_component = Component(-1, -1, ComponentType::Resistor, 500);
        break;

    case ComponentType::VoltageSource:
        temp_component = Component(-1, -1, ComponentType::VoltageSource, 5);
        break;

    case ComponentType::CurrentSource:
        temp_component = Component(-1, -1, ComponentType::CurrentSource, 1);
        break;

    case ComponentType::Capacitor:
        temp_component = Component(-1, -1, ComponentType::Capacitor, 0.00001);
        break;

    case ComponentType::Inductor:
        temp_component = Component(-1, -1, ComponentType::Inductor, 0.00001);
        break;

    case ComponentType::Switch:
        temp_component = Component(-1, -1, ComponentType::Switch, 0);
        break;

    default:
        return -1;
    }

    temp_component.identification_number = nextComponentID;
    components.emplace_back(temp_component);
    return nextComponentID++;

}

Component* Circuit::GetComponentByID(int id) {
    for (auto& comp : components) {
        if (comp.identification_number == id)
            return &comp;
    }
    return nullptr;
}

void Circuit::RemoveComponent(int id) {
    for (int i = 0; i < components.size(); i++) {
        if (components[i].identification_number == id) {            
            components.erase(components.begin() + i);
        }
    }
}
void Circuit::ComputeTopology() { //DO THIS

}
