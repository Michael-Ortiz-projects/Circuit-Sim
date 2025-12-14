#include "Controller.h"
#include "InputHandler.h"

Controller::Controller(Circuit& Circuit, std::vector<SchematicComponent>& Components, sf::RenderWindow& Window, UI_Manager& UI)
	: circuit(Circuit), components(Components), dragHandler(Components), cameraHandler(Window), currentHandler(nullptr), window(Window), ui(UI)
	{ }


void Controller::onMousePress(const sf::Event::MouseButtonEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y });

	

	if (event.button == sf::Mouse::Button::Left) {
		SchematicComponent* clickedComponent = findComponentAt(worldMousePosition);
		if (clickedComponent) {
			currentHandler = &dragHandler;
			dragHandler.setDraggedComponent(*clickedComponent);
		}
	}

	if (event.button == sf::Mouse::Button::Right) {
		currentHandler = &cameraHandler;
	}


	if (currentHandler) currentHandler->onMousePress(worldMousePosition);
}

void Controller::onMouseMove(const sf::Event::MouseMoveEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y });
	if (currentHandler) currentHandler->onMouseMove(worldMousePosition);
}

void Controller::onScroll(const sf::Event::MouseWheelScrollEvent& event) {
	currentHandler = &cameraHandler;
	if (currentHandler) currentHandler->onScroll(event);
	if (currentHandler) {
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
		}
	}
}

void Controller::onMouseRelease(const sf::Event::MouseButtonEvent& event) {
	sf::Vector2f worldMousePosition = window.mapPixelToCoords({ event.x, event.y });

	if (currentHandler) {
		currentHandler->onMouseRelease(worldMousePosition);
	}
	if (currentHandler) {
		if (currentHandler->shouldRelease()) {
			currentHandler = nullptr;
		}
	}
}

void Controller::onKeyPress(const sf::Event::KeyEvent& event) {
	if (currentHandler) currentHandler->onKeyPress(event);

	else if (event.code == sf::Keyboard::Escape) {
		window.close();
	}
}

void Controller::setHandler(InputHandler* handler) {
	currentHandler = handler;
}

SchematicComponent* Controller::findComponentAt(const sf::Vector2f point) {
	for (auto& comp : components) {
		if (comp.spriteContainsPoint(point)) {
			return &comp;
		}
	}
	return nullptr;
}

sf::View Controller::getView() {
	return cameraHandler.GetView();
}