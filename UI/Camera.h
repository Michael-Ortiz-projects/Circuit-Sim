#pragma once
#include "SFML/Graphics.hpp"
#include "../Config.h"


class Camera
{
public:
	
	Camera(float width, float height);

	void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
	
	void Update(const sf::RenderWindow& window);

	void Apply(sf::RenderWindow& window);

	const sf::View& GetView() const { return view; }

private:
	sf::View view;

	bool dragging = false;
	sf::Vector2i lastMousePosition;

	void zoomAt(float factor, const sf::Vector2i& pixel, const sf::RenderWindow& window);
};

