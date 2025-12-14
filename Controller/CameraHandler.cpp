#include "CameraHandler.h"

CameraHandler::CameraHandler(sf::RenderWindow& Window)
	: window(Window)
{
	view.setSize(SCR_WIDTH, SCR_HEIGHT); // positive
	view.setCenter(SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);
	dragging = false;
}

void CameraHandler::onMousePress(const sf::Vector2f& worldPos) {
	lastMousePixel = window.mapCoordsToPixel(worldPos, view);
	dragging = true;
}

void CameraHandler::onMouseMove(const sf::Vector2f& worldPos) {
	sf::Vector2i pixelPos = window.mapCoordsToPixel(worldPos, view);

	if (!dragging) return;

	sf::Vector2f lastWorld = window.mapPixelToCoords(lastMousePixel, view);
	sf::Vector2f currentWorld = window.mapPixelToCoords(pixelPos, view);

	view.move(lastWorld - currentWorld);
	window.setView(view);

	lastMousePixel = pixelPos;
}

void CameraHandler::onScroll(const sf::Event::MouseWheelScrollEvent& event) {
	float zoomFactor = (event.delta > 0) ? 0.95f : 1.05f;
	std::cout << "Zoom factor: " << zoomFactor << ", delta: " << event.delta << "\n";

	sf::Vector2i mousePixel = sf::Mouse::getPosition(window);

	zoomAt(zoomFactor, mousePixel);

	window.setView(view);
}

void CameraHandler::onMouseRelease(const sf::Vector2f& worldPos) {
	dragging = false;
}

bool CameraHandler::shouldRelease() const {
	return !dragging;
}

void CameraHandler::zoomAt(float factor, const sf::Vector2i& pixel) {
	sf::Vector2f mouseWorldPosition = window.mapPixelToCoords(pixel, view);

	view.zoom(factor);

	sf::Vector2f new_mouseWorldPosition = window.mapPixelToCoords(pixel, view);

	sf::Vector2f offset = mouseWorldPosition - new_mouseWorldPosition;
	view.move(offset);
}
