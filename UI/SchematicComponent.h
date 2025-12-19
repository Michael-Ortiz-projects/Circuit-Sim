#pragma once
#include "SFML/Graphics.hpp"
#include "../Core/Component.h"
#include "../Config.h"
#include "../Debug.h"

class SchematicComponent {

public:
	sf::Vector2f position;
	sf::Vector2f node_A;
	sf::Vector2f node_B;
	float rotation;
	int componentID;
	bool selected;

	SchematicComponent(int id, sf::Vector2f pos, int rot, ComponentType t);
	
	void setPosition(const sf::Vector2f& pos);

	void setRotation(float rot);

	void startDrag(const sf::Vector2f& mouseWorldPos);

	void dragTo(const sf::Vector2f& mouseWorldPos);

	void stopDrag();
	
	void setTexture(const sf::Texture& texture);

	bool hitBoxContainsPoint(const sf::Vector2f point);

	const sf::Vector2f& getPosition() const;

	float getRotation() const;

	sf::Sprite& getSprite();	

	sf::RectangleShape& getHitBox();

	ComponentType getType();

private:
	std::string label;
	ComponentType type;

	bool dragging;
	sf::Vector2f dragOffset;

	sf::Sprite sprite;
	sf::RectangleShape hitBox;

};

