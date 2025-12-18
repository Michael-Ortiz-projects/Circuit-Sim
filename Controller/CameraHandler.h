#pragma once
#include <iostream>
#include "InputHandler.h"
#include "../Config.h"
#include "SFML/Graphics.hpp"

class CameraHandler : public InputHandler {
public:
	CameraHandler(sf::RenderWindow& Window, sf::View& CanvasView);

	void onMousePress(const sf::Vector2f& worldPos) override;

	void onMouseMove(const sf::Vector2f& worldPos) override;

	void onScroll(const sf::Event::MouseWheelScrollEvent& event) override;

	void onMouseRelease(const sf::Vector2f& worldPos) override;

	bool shouldRelease() const override;

private:
	sf::View& canvasView;
	sf::RenderWindow& window;
	sf::Vector2f lastMousePosition;
	sf::Vector2i lastMousePixel;
	float CameraZoom = 1.0f;
	bool dragging;

	void zoomAt(float factor, const sf::Vector2i& pixel);
};

