#include "SchematicComponent.h"

SchematicComponent::SchematicComponent(Component* component, sf::Vector2f pos, float rot) {
	real_component = component;
	position = pos;
	rotation = rot;
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
		sf::Vector2f gridSnappedPostion(std::round(mouseWorldPos.x / 30.0f) * 30.0f, std::round(mouseWorldPos.y / 30.0f) * 30.0f); //30 is grid size
		sf::Vector2f halfSpriteSize(sprite.getTexture()->getSize().x / 2, sprite.getTexture()->getSize().y / 2);
		sprite.setPosition(gridSnappedPostion - halfSpriteSize);
	}
}

void SchematicComponent::stopDrag() {
	dragging = false;
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

Component* SchematicComponent::getRealComponent() const { return real_component; }

ComponentType SchematicComponent::getType() const { return real_component->type; }