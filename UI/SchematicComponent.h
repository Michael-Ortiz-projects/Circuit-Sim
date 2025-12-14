#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/Component.h"

class SchematicComponent {

public:


	SchematicComponent(Component* component, sf::Vector2f position, float rotation);
	
	void setPosition(const sf::Vector2f& pos);

	void setRotation(float rot);

	void startDrag(const sf::Vector2f& mouseWorldPos);

	void dragTo(const sf::Vector2f& mouseWorldPos);

	void stopDrag();

	bool spriteContainsPoint(const sf::Vector2f point);

	const sf::Vector2f& getPosition() const;

	float getRotation() const;

	sf::Sprite& getSprite() ;

	Component* getRealComponent() const;

	ComponentType getType() const;
	

private:
	std::string label;
	sf::Vector2f position;
	float rotation;

	bool dragging;
	sf::Vector2f dragOffset;

	Component* real_component;
	sf::Sprite sprite;

};

