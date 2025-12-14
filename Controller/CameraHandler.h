#pragma once
#include <iostream>
#include "InputHandler.h"
#include "../Config.h"
#include "SFML/Graphics.hpp"

class CameraHandler : public InputHandler {
public:
	CameraHandler(sf::RenderWindow& Window);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onScroll(const sf::Event::MouseWheelScrollEvent& event) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override;

	const sf::View& GetView() const { return view; }

private:
	sf::View view;
	sf::RenderWindow& window;
	sf::Vector2f lastMousePosition;
	sf::Vector2i lastMousePixel;

	bool dragging;

	void zoomAt(float factor, const sf::Vector2i& pixel);
};

