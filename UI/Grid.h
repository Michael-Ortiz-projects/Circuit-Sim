#pragma once
#include "SFML/Graphics.hpp"

class Grid {
public:
	Grid(float spacing);
	void draw(sf::RenderWindow& window, const sf::View& view);
	void setSpacing(float spacing);

private:
	float gridSpacing;
	sf::Color clearColor = sf::Color(30, 30, 30);
	sf::Color gridColor = sf::Color(40, 40, 48);
};