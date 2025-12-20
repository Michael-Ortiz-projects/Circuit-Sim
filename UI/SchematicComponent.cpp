#include "SchematicComponent.h"
#include <cmath>

SchematicComponent::SchematicComponent(int id, sf::Vector2f pos, float rot, ComponentType t)
    : componentID(id), position(pos), rotation(rot), type(t)
{
    sprite.setPosition(position);
    sprite.setOrigin({ 360.f, 240.f });
    sprite.setRotation(rotation);

    hitBox.setPosition(position);
    hitBox.setSize({ 105.f, 70.f });
    hitBox.setOrigin(hitBox.getSize() * 0.5f);
    hitBox.setFillColor(sf::Color::Transparent);
    hitBox.setOutlineThickness(1.f);
    hitBox.setOutlineColor(sf::Color::Blue);
}

SchematicComponent::SchematicComponent(const Component& comp)
    : componentID(comp.id), position(comp.position), rotation(comp.rotation), type(comp.type), selected(comp.selected)
{
    sprite.setPosition(position);
    sprite.setOrigin({ 360.f, 240.f });
    sprite.setRotation(rotation);

    hitBox.setPosition(position);
    hitBox.setSize({ 105.f, 70.f });
    hitBox.setOrigin(hitBox.getSize() * 0.5f);
    hitBox.setFillColor(sf::Color::Transparent);
    hitBox.setOutlineThickness(1.f);
    hitBox.setOutlineColor(sf::Color::Blue);
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

void SchematicComponent::startDrag(const sf::Vector2f& worldPos) {
    dragging = true;
    selected = true;
    dragOffset = position - worldPos;
}

void SchematicComponent::dragTo(const sf::Vector2f& worldPos) {
    if (!dragging) return;

    sf::Vector2f target = worldPos + dragOffset;
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