#include "SchematicComponent.h"
#include <cmath>
#include <sstream>




SchematicComponent::SchematicComponent(const NetlistComponent& comp, const sf::Vector2f canvasPos, sf::Font& font)
    : componentID(comp.id), position(canvasPos), rotation(0), type(comp.type), value(comp.value), display(font, sf::Vector2f(canvasPos.x, canvasPos.y - 70), sf::Vector2f(100.f, 20.f), 12, false)
{
    // initialize display text
    label = Debug::ComponentType_to_String(type);
    display.setLabel(label);
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    display.setValue(ss.str());
    displayOffset = { 0, -70 };
    display.centerAt(canvasPos + displayOffset);
    sprite.setPosition(position);
    sprite.setOrigin({ 360.f, 240.f });
    sprite.setRotation(rotation);

    switch (type) {
    case ComponentType::VoltageSource:
    case ComponentType::Resistor:
    case ComponentType::CurrentSource:
    case ComponentType::Capacitor:
    case ComponentType::Inductor:
    case ComponentType::Switch:
        schematicTerminals.push_back({ 0, sf::Vector2f(-60, 0), {-1, -1} });
        schematicTerminals.push_back({ 1, sf::Vector2f(60, 0), {-1, -1} });
        hitBox.setSize({ 105.f, 70.f });
        hitBox.setOrigin(hitBox.getSize() * 0.5f);
        break;

    case ComponentType::VCVS:
    case ComponentType::VCCS:
    case ComponentType::CCVS:
    case ComponentType::CCCS:
        schematicTerminals.push_back({ 0, sf::Vector2f(-60, 0),  { -1, -1 } });
        schematicTerminals.push_back({ 1, sf::Vector2f(60, 0),  { -1, -1 } });
        schematicTerminals.push_back({ 2, sf::Vector2f(-20, -40),  { -1, -1 } });
        schematicTerminals.push_back({ 3, sf::Vector2f(20, -40),  { -1, -1 } });
        hitBox.setSize({ 105.f, 70.f });
        hitBox.setOrigin(hitBox.getSize() * 0.5f);
        break;
    case ComponentType::Ground:
        schematicTerminals.push_back({ 0, sf::Vector2f(0, -20), {-1, -1} });
        hitBox.setSize({ 40.f, 30.f });
        hitBox.setOrigin(hitBox.getSize() * 0.5f - sf::Vector2f(0, 10));
        
        break;
    }

    // initialize hitbox
    hitBox.setFillColor(sf::Color::Transparent);
    hitBox.setOutlineColor(sf::Color(63, 182, 168));
    hitBox.setOutlineThickness(1.f);
    hitBox.setPosition(position);

}
void SchematicComponent::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sprite.setPosition(pos);
    hitBox.setPosition(pos);
    display.centerAt(pos + displayOffset);

}

void SchematicComponent::setRotation(float rot) {
    //std::cout << "setRotation running with requrested rotation of " << rot << "\n";
    rotation = rot;
    sprite.setRotation(rot);

    if (static_cast<int>(rotation) % 180 == 0) {
        displayOffset = { 0, -70 };
    }
    else if (static_cast<int>(rotation) % 180 == 90) {
        displayOffset = { 80, -20 };
    }
    display.centerAt(position + displayOffset);
}

void SchematicComponent::setLabel(std::string string) {
    label = string;
    display.setLabel(label);
    display.centerAt(position + displayOffset);
}

void SchematicComponent::setValue(double val) {
    value = val;
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    display.setValue(ss.str());
    
}

void SchematicComponent::startDrag(const sf::Vector2f& worldPos) {
    dragging = true;
    dragOffset = position - worldPos;
    //selected = true;
}

void SchematicComponent::dragTo(const sf::Vector2f& worldPos) {
    if (!dragging) return;

    sf::Vector2f target = worldPos;
    sf::Vector2f snapped(
        std::round(target.x / gridSize) * gridSize,
        std::round(target.y / gridSize) * gridSize
    );
    setPosition(snapped);
}

void SchematicComponent::stopDrag() {
    dragging = false;
    //selected = false;
}

void SchematicComponent::setTexture(const sf::Texture& texture) {
    sprite.setTexture(texture, true);
    sprite.setScale(0.166f, 0.166f);
}

bool SchematicComponent::hitBoxContainsPoint(const sf::Vector2f& point) {
    return hitBox.getGlobalBounds().contains(point);
}

const sf::Vector2f& SchematicComponent::getPosition() const { return position; }
float SchematicComponent::getRotation() const { return rotation; }
sf::Sprite& SchematicComponent::getSprite() { return sprite; }
sf::RectangleShape& SchematicComponent::getHitBox() { return hitBox; }
ComponentType SchematicComponent::getType() const { return type; }



ComponentInfoDisplay& SchematicComponent::getComponentInfoDisplay() {
    return display;
}

void SchematicComponent::updateDisplay() {

}