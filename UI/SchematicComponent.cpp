#include "SchematicComponent.h"

SchematicComponent::SchematicComponent(int id, sf::Vector2f pos, int rot, ComponentType t)
	: componentID(id), position(pos), rotation(rot), type(t)
{
	sprite.setPosition(position);
	sprite.setRotation(rotation);

}

void SchematicComponent::setPosition(const sf::Vector2f& pos) {
	position = pos;
	sprite.setPosition(pos);
}

void SchematicComponent::setRotation(float rot) {
	rotation = rot;
	sprite.setRotation(rot);
}

void SchematicComponent::startDrag(const sf::Vector2f& mouseWorldPos) {
	std::cout << "drag started\n";
	dragging = true;
	dragOffset = sprite.getPosition() - mouseWorldPos;
}

void SchematicComponent::dragTo(const sf::Vector2f& mouseWorldPos) { //needs to be updated for rotation	
	if (dragging) {
		sf::Vector2f gridSnappedPostion(std::round(mouseWorldPos.x / gridSize) * gridSize, std::round(mouseWorldPos.y / gridSize) * gridSize); //30 is grid size
		sf::Vector2f halfSpriteSize(sprite.getTexture()->getSize().x / 12, sprite.getTexture()->getSize().y / 12);
		sprite.setPosition(gridSnappedPostion - halfSpriteSize);
	}
}

void SchematicComponent::stopDrag() {
	dragging = false;
}

void SchematicComponent::setTexture(const sf::Texture& texture) {
	sprite.setTexture(texture, true);
	
	sprite.setScale(.166f, .166f);
}

bool SchematicComponent::spriteContainsPoint(const sf::Vector2f point) {
	//std::cout << "Sprite Global Bounds: (" << sprite.getGlobalBounds().getPosition().x << ", " <<  sprite.getGlobalBounds().getPosition().y << ") to "
	//		  << "(" << sprite.getGlobalBounds().getPosition().x + sprite.getGlobalBounds().width << ", " << sprite.getGlobalBounds().getPosition().y + sprite.getGlobalBounds().height << ")\n";
	//std::cout << "Point (" << point.x << ", " << point.y << ")\n";
	return sprite.getGlobalBounds().contains(point);
}



const sf::Vector2f& SchematicComponent::getPosition() const { return position; }

float SchematicComponent::getRotation() const { return rotation; }

sf::Sprite& SchematicComponent::getSprite()  { return sprite; }

ComponentType SchematicComponent::getType() {
	return type;
}

	