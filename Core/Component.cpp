#include "Component.h"

Component::Component(int A, int B, const ComponentType& ty, double val)
    : nodeA(A), nodeB(B), type(ty), value(val), current(0), voltage(0), rotation(0)
{
    switch (ty) {
    case ComponentType::VoltageSource:
        voltage = val;
        break;
    case ComponentType::CurrentSource:
        current = val;
        break;
    case ComponentType::Switch:
        isClosed = false;
        break;
    default:
        break;
    }
}

void Component::setPosition(const sf::Vector2f& pos) {
    position = pos;
}

void Component::setRotation(float rot) {
    rotation = rot;
}

void Component::startDrag(const sf::Vector2f& worldPos) {
    selected = true;
}

void Component::dragTo(const sf::Vector2f& worldPos) {
    if (!selected) return;

    sf::Vector2f target = worldPos;
    sf::Vector2f snapped(std::round(target.x / gridSize) * gridSize,std::round(target.y / gridSize) * gridSize);
    setPosition(snapped);
}

void Component::stopDrag() {
    
}