#pragma once
#include "SFML/Graphics.hpp"

class Grid {
public:
	Grid(float spacing);
	void draw(sf::RenderWindow& window, const sf::View& view);
	void setSpacing(float spacing);

private:
	float gridSpacing;
	sf::Color clearColor = sf::Color(18, 20, 23);
	sf::Color gridColor = sf::Color(40, 40, 48);
};