#include "SchematicComponent.h"
#include <cmath>
#include <sstream>




SchematicComponent::SchematicComponent(const Component& comp, sf::Font& font)
    : componentID(comp.id), position(comp.position), rotation(comp.rotation), type(comp.type), value(comp.value), display(font, sf::Vector2f(comp.position.x, comp.position.y - 100), sf::Vector2f(100.f, 20.f), false)
{
    // Initialize display text
    label = "sample label";
    display.setLabel(label);
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    display.setValue(ss.str());

    sprite.setPosition(position);
    sprite.setOrigin({ 360.f, 240.f });
    sprite.setRotation(rotation);

    // Initialize hitbox (optional)
    hitBox.setSize({ 105.f, 70.f });
    hitBox.setFillColor(sf::Color::Transparent);
    hitBox.setOutlineColor(sf::Color(63, 182, 168));
    hitBox.setOutlineThickness(1.f);
    hitBox.setOrigin(hitBox.getSize() * 0.5f);
    hitBox.setPosition(position);
}
void SchematicComponent::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sprite.setPosition(pos);
    hitBox.setPosition(pos);
}

void SchematicComponent::setRotation(float rot) {
    rotation = rot;
    sprite.setRotation(rot);
}

void SchematicComponent::setLabel(std::string string) {
    label = string;
}

void SchematicComponent::setValue(double val) {
    value = val;
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    display.setValue(ss.str());
    
}

void SchematicComponent::startDrag(const sf::Vector2f& worldPos) {
    dragging = true;
    selected = true;
    dragOffset = position - worldPos;
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

sf::Vector2f SchematicComponent::getLeadPositionA() const {
    float rad = rotation * 3.14159265f / 180.f;
    std::cout << rotation;

    return position + sf::Vector2f(
        leadOffsetA.x * cos(rad) - leadOffsetA.y * sin(rad),
        leadOffsetA.x * sin(rad) + leadOffsetA.y * cos(rad)
    );
}

sf::Vector2f SchematicComponent::getLeadPositionB() const {
    float rad = rotation * 3.14159265f / 180.f;
    return position + sf::Vector2f(
        leadOffsetB.x * cos(rad) - leadOffsetB.y * sin(rad),
        leadOffsetB.x * sin(rad) + leadOffsetB.y * cos(rad)
    );
}

ComponentInfoDisplay& SchematicComponent::getComponentInfoDisplay() {
    return display;
}

bool SchematicComponent::nearLeadA(sf::Vector2f& point) {
    sf::Vector2f distance = point - getLeadPositionA();
    float r = nodeSelectionRadius;
    return (distance.x * distance.x + distance.y * distance.y) <= r * r;
}

bool SchematicComponent::nearLeadB(sf::Vector2f& point) {
    sf::Vector2f distance = point - getLeadPositionB();
    float r = nodeSelectionRadius;
    return (distance.x * distance.x + distance.y * distance.y) <= r * r;
}

void SchematicComponent::updateDisplay() {

}