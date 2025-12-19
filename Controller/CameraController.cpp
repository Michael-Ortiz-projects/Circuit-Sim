#include "CameraController.h"

CameraController::CameraController(sf::RenderWindow& Window, sf::View& view) 
	: window(Window), canvasView(view) { }

void CameraController::onMousePress(const sf::Vector2i& pixelPos) {
    lastMousePixel = pixelPos;
    dragging = true;
    grabWorldPos = window.mapPixelToCoords(pixelPos, canvasView);
}

void CameraController::onMouseMove(const sf::Vector2i& pixelPos) {
    if (!dragging) return;

    sf::Vector2f currentWorld =
        window.mapPixelToCoords(pixelPos, canvasView);

    sf::Vector2f delta = grabWorldPos - currentWorld;
    std::cout << Debug::printVector2f(delta);
    canvasView.move(delta);
    window.setView(canvasView);
    Debug::debugPrintView(canvasView);
}

void CameraController::onMouseRelease() {
    dragging = false;
}

void CameraController::onScroll(const sf::Event::MouseWheelScrollEvent& event) {
    float factor = (event.delta > 0) ? (zoomFactor < minZoom) ? 1.0f : 0.95f : (zoomFactor > maxZoom) ? 1.0f : 1.05f;

    zoomFactor *= factor;
    sf::Vector2f beforeZoom = window.mapPixelToCoords( { (event.x), (event.y) }, canvasView);

    canvasView.zoom(factor);

    sf::Vector2f afterZoom = window.mapPixelToCoords( { (event.x), (event.y) }, canvasView);

    canvasView.move(beforeZoom - afterZoom);
}

void CameraController::onKeyPress(const sf::Event::KeyEvent& event) {
    if (event.code == sf::Keyboard::Escape)
        window.close();
}


void CameraController::setSensitivity(float s) {
    //sensitivity = s;
}
