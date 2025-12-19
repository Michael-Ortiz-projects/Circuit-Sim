#include "SchematicComponent.h"

SchematicComponent::SchematicComponent(int id, sf::Vector2f pos, int rot, ComponentType t)
	: componentID(id), position(pos), rotation(rot), type(t), selected(false) {
	sprite.setPosition(position);
	sprite.setOrigin({ 360, 240 });
	sprite.setRotation(rotation);
	node_A = position - sf::Vector2f(120, 0);
	node_B = position + sf::Vector2f(120, 0);
	hitBox.setPosition(pos);
	hitBox.setSize({ 210 / 2, 140 / 2});
	hitBox.setOrigin(hitBox.getSize() * 0.5f);
	hitBox.setOutlineThickness(1);
	hitBox.setOutlineColor(sf::Color::Blue);
	hitBox.setFillColor(sf::Color::Transparent);
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

void SchematicComponent::startDrag(const sf::Vector2f& mouseWorldPos) {
	std::cout << "drag started\n";
	dragging = true;
	selected = true;
	dragOffset = sprite.getPosition() - mouseWorldPos;
}

void SchematicComponent::dragTo(const sf::Vector2f& mouseWorldPos) { //needs to be updated for rotation	
	if (dragging) {
		sf::Vector2f gridSnappedPostion(std::round(mouseWorldPos.x / gridSize) * gridSize, std::round(mouseWorldPos.y / gridSize) * gridSize);
		setPosition(gridSnappedPostion);
		std::cout << Debug::printVector2f(position);
	}
}

void SchematicComponent::stopDrag() {
	dragging = false;
}

void SchematicComponent::setTexture(const sf::Texture& texture) {
	sprite.setTexture(texture, true);
	
	sprite.setScale(.166f, .166f);
}

bool SchematicComponent::hitBoxContainsPoint(const sf::Vector2f point) {

	return hitBox.getGlobalBounds().contains(point);
}



const sf::Vector2f& SchematicComponent::getPosition() const { return position; }

float SchematicComponent::getRotation() const { return rotation; }

sf::Sprite& SchematicComponent::getSprite()  { return sprite; }

sf::RectangleShape& SchematicComponent::getHitBox() { return hitBox; }
ComponentType SchematicComponent::getType() {
	return type;
}

	