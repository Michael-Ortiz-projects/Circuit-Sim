#include "Camera.h"

Camera::Camera(float width, float height) {
	dragging = false;
	view.setSize(SCR_WIDTH, -SCR_HEIGHT);
	view.setCenter(SCR_WIDTH / 2, SCR_HEIGHT / 2);
}

void Camera::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseWheelScrolled && !dragging) {
        if (event.mouseWheelScroll.delta != 0) {
            float zoomFactor = (event.mouseWheelScroll.delta > 0) ? 0.95f : 1.05f;
            zoomAt(zoomFactor, sf::Mouse::getPosition(window), window);
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Right) {
            dragging = true;
            lastMousePosition = sf::Mouse::getPosition(window);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Right) {
            dragging = false;
        }
    }
}

void Camera::Update(const sf::RenderWindow& window) {
    if (dragging) {
        sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
        sf::Vector2i mouseDelta = currentMousePosition - lastMousePosition;

        sf::Vector2f worldDelta = window.mapPixelToCoords(currentMousePosition) - window.mapPixelToCoords(lastMousePosition);

        view.move(-worldDelta);

        lastMousePosition = currentMousePosition;
    }
}

void Camera::zoomAt(float factor, const sf::Vector2i& pixel, const sf::RenderWindow& window) {
    sf::Vector2f mouseWorldPosition = window.mapPixelToCoords(pixel, view);

    view.zoom(factor);

    sf::Vector2f new_mouseWorldPosition = window.mapPixelToCoords(pixel, view);

    sf::Vector2f offset = mouseWorldPosition - new_mouseWorldPosition;
    view.move(offset);
}

void Camera::Apply(sf::RenderWindow& window) {
    window.setView(view);
}