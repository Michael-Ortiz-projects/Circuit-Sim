#include "CameraHandler.h"

CameraHandler::CameraHandler(sf::RenderWindow& Window, sf::View& CanvasView)
	: window(Window), canvasView(CanvasView)  { }

void CameraHandler::onMousePress(const sf::Vector2f& worldPos) {
	lastMousePixel = window.mapCoordsToPixel(worldPos, canvasView);
	dragging = true;
	std::cout << "Camera Handler On Mouse Press completed\n\n";
}

void CameraHandler::onMouseMove(const sf::Vector2f& worldPos) {
	sf::Vector2i pixelPos = window.mapCoordsToPixel(worldPos, canvasView);
	if (!dragging) return;


	
	canvasView.move(sf::Vector2f(lastMousePixel - pixelPos) * CameraZoom);

	lastMousePixel = pixelPos;


	std::cout << "Camera Handler On Mouse Move completed\n\n";

}

void CameraHandler::onScroll(const sf::Event::MouseWheelScrollEvent& event) {
	CameraZoom = (canvasView.getSize().x / SCR_WIDTH);
	std::cout << "Zoom: " << CameraZoom << std::endl;
	float zoomFactor = (event.delta > 0) ? (CameraZoom > .368f ? .95f : 1.0f) : (CameraZoom < 2 ? 1.05 : 1.0f);
	std::cout << "Zoom factor: " << zoomFactor << ", delta: " << event.delta << "\n";

	sf::Vector2i mousePixel = sf::Mouse::getPosition(window);

	zoomAt(zoomFactor, mousePixel);

	window.setView(canvasView);
}

void CameraHandler::onMouseRelease(const sf::Vector2f& worldPos) {
	dragging = false;
}

bool CameraHandler::shouldRelease() const {
	return !dragging;
}

void CameraHandler::zoomAt(float factor, const sf::Vector2i& pixel) {
	sf::Vector2f mouseWorldPosition = window.mapPixelToCoords(pixel, canvasView);

	canvasView.zoom(factor);


	sf::Vector2f new_mouseWorldPosition = window.mapPixelToCoords(pixel, canvasView);

	sf::Vector2f offset = mouseWorldPosition - new_mouseWorldPosition;
	canvasView.move(offset);
}
